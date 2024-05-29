/******************************************************************************
 JXDocumentManager.cpp

	Singleton class that manages all JXDocuments and provides the following
	services:

	1)  GetNewFileName()

		Convenient source of names for newly created documents.

	2)  JXDocumentMenu

		Instant menu of all open documents

	3)  Safety save

		Periodically save all unsaved documents in temporary files.
		Also protects against X Server crashes and calls to assert().

		While it would be nice to also catch Ctrl-C, this must remain
		uncaught because it provides a way to quickly stop a runaway
		program.  Also, in practice, X programs are usually backgrounded,
		in which case Ctrl-C is irrelevant.

	4)  Dependencies between documents

		Classes derived from JXDocument can override NeedDocument()
		if they require that other documents stay open.  When closed,
		such required documents merely deactivate.

	5)  Searching for files that are supposed to exist

		Dependencies between documents usually imply that one document
		stores the file names of other documents that it has to open.
		If the user moves or renames these files, call FindFile() to
		search for them.

		Derived classes can override this function to search application
		specific directories.

	A derived class is the best place to put all the code associated
	with determining a file's type and creating the appropriate derived
	class of JXDocument.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1997-2001 by John Lindal.

 ******************************************************************************/

#include "JXDocumentManager.h"
#include "JXFileDocument.h"
#include "JXDocumentMenu.h"
#include "JXUrgentFunctionTask.h"
#include "JXFunctionTask.h"
#include "JXImage.h"
#include "JXImageCache.h"
#include "JXDisplay.h"
#include "JXColorManager.h"
#include "JXChooseFileDialog.h"
#include "JXFunctionTask.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

#include "jx_plain_file_small.xpm"

const JInteger kFirstShortcut = 0;
const JInteger kLastShortcut  = 9;

static const JUtf8Byte kShortcutChar[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

const Time kDefaultSafetySavePeriod = 30000;	// 30 seconds (milliseconds)
const JSize kSecondsToMilliseconds  = 1000;

// JBroadcaster message types

const JUtf8Byte* JXDocumentManager::kDocMenuNeedsUpdate =
	"DocMenuNeedsUpdate::JXDocumentManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocumentManager::JXDocumentManager
	(
	const bool wantShortcuts
	)
	:
	itsWantShortcutFlag( wantShortcuts )
{
	itsDocList = jnew JArray<DocInfo>;
	itsDocList->SetCompareFunction(CompareDocNames);
	itsDocList->SetSortOrder(JListT::kSortAscending);

	itsNewDocCount = 0;

	itsFileMap = jnew JArray<FileMap>;

	itsPerformSafetySaveFlag = true;

	itsSafetySaveTask = jnew JXFunctionTask(kDefaultSafetySavePeriod,
		std::bind(&JXDocumentManager::SafetySave, this, JXDocumentManager::kTimer),
		"JXDocumentManager::SafetySaveTask");

	itsUpdateDocMenuTask = nullptr;

	JXImageCache* cache = JXGetApplication()->GetCurrentDisplay()->GetImageCache();
	itsDefaultMenuIcon  = cache->GetImage(jx_plain_file_small);

	JXSetDocumentManager(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocumentManager::~JXDocumentManager()
{
	assert( itsDocList->IsEmpty() );
	jdelete itsDocList;

	for (const auto& file : *itsFileMap)
	{
		jdelete file.oldName;
		jdelete file.newName;
	}
	jdelete itsFileMap;

	jdelete itsSafetySaveTask;
	// cannot delete itsUpdateDocMenuTask
}

/******************************************************************************
 DocumentCreated (virtual)

 ******************************************************************************/

void
JXDocumentManager::DocumentCreated
	(
	JXDocument* doc
	)
{
	DocInfo info(doc);

	// find the first unused shortcut, if any

	for (JInteger i=kFirstShortcut; i<=kLastShortcut; i++)
	{
		bool found = false;
		for (const auto& info : *itsDocList)
		{
			if (info.shortcut == i)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			info.shortcut = i;
			break;
		}
	}

	// insert the new document -- can't sort until later

	itsDocList->AppendItem(info);
	if (itsPerformSafetySaveFlag)
	{
		itsSafetySaveTask->Start();
	}

	// ensure menu not empty; update the menu shortcuts

	DocumentMenusNeedUpdate();
}

/******************************************************************************
 DocumentDeleted (virtual)

	Automatically called by ~JXDocument.

 ******************************************************************************/

void
JXDocumentManager::DocumentDeleted
	(
	JXDocument* doc
	)
{
	const JSize count = itsDocList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		DocInfo info = itsDocList->GetItem(i);
		if (info.doc == doc)
		{
			itsDocList->RemoveItem(i);

			// assign the shortcut to the first item that doesn't have a shortcut

			if (info.shortcut != kNoShortcutForDoc)
			{
				for (JIndex j=1; j<=count-1; j++)
				{
					DocInfo info1 = itsDocList->GetItem(j);
					if (info1.shortcut == kNoShortcutForDoc)
					{
						info1.shortcut = info.shortcut;
						itsDocList->SetItem(j, info1);
						break;
					}
				}
			}

			// check if other documents want to close -- recursive

			CloseDocuments();
			break;
		}
	}

	// remove SafetySave() idle task

	if (itsDocList->IsEmpty())
	{
		itsSafetySaveTask->Stop();
	}

	// update the menu shortcuts

	if (itsWantShortcutFlag)
	{
		DocumentMenusNeedUpdate();
	}
}

/******************************************************************************
 DocumentMenusNeedUpdate (private)

 ******************************************************************************/

void
JXDocumentManager::DocumentMenusNeedUpdate()
{
	if (itsUpdateDocMenuTask == nullptr)
	{
		itsUpdateDocMenuTask = jnew JXUrgentFunctionTask(this, [this]()
		{
			itsUpdateDocMenuTask = nullptr;
			UpdateAllDocumentMenus();
		},
		"JXDocumentManager::DocumentMenusNeedUpdate");
		itsUpdateDocMenuTask->Go();
	}
}

/******************************************************************************
 UpdateAllDocumentMenus (private)

	This must be called via an UrgentTask because we can't call GetName()
	for the new document until long after DocumentCreated() is called.

 ******************************************************************************/

void
JXDocumentManager::UpdateAllDocumentMenus()
{
	Broadcast(DocMenuNeedsUpdate());
}

/******************************************************************************
 DocumentMustStayOpen

	Call this with true if a document must remain open even if nobody else
	needs it.

 ******************************************************************************/

void
JXDocumentManager::DocumentMustStayOpen
	(
	JXDocument*	doc,
	const bool	stayOpen
	)
{
	const JSize count = itsDocList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		DocInfo info = itsDocList->GetItem(i);
		if (info.doc == doc)
		{
			info.keepOpen = stayOpen;
			itsDocList->SetItem(i, info);
			break;
		}
	}

	if (stayOpen == false)
	{
		CloseDocuments();
	}
}

/******************************************************************************
 OKToCloseDocument

	Returns true if the given document can be closed.

 ******************************************************************************/

bool
JXDocumentManager::OKToCloseDocument
	(
	JXDocument* doc
	)
	const
{
	return std::all_of(begin(*itsDocList), end(*itsDocList),
			[doc] (const DocInfo& info)
			{ return (info.doc == doc || !(info.doc)->NeedDocument(doc)); });
}

/******************************************************************************
 CloseDocuments

	Close invisible documents that are no longer needed by any other documents.

	An iterator would be cleaner, but since it is recursive and keeps looping
	until nothing more is closed, this method wastes less stack space.

 ******************************************************************************/

void
JXDocumentManager::CloseDocuments()
{
	JSize count = itsDocList->GetItemCount();

	JSize closeCount;
	do
	{
		closeCount = 0;
		for (JIndex i=1; i<=count; i++)
		{
			const DocInfo info = itsDocList->GetItem(i);
			if (!info.doc->IsActive() && !info.keepOpen &&
				OKToCloseDocument(info.doc) && info.doc->Close())
			{
				closeCount++;
				count = itsDocList->GetItemCount();	// several documents may have closed
			}
		}
	}
		while (closeCount > 0);
}

/******************************************************************************
 DocumentsNeedSave

 ******************************************************************************/

bool
JXDocumentManager::DocumentsNeedSave()
	const
{
	for (const auto& info : *itsDocList)
	{
		if (info.doc->NeedsSave())
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 SaveAllFileDocuments

 ******************************************************************************/

bool
JXDocumentManager::SaveAllFileDocuments
	(
	const bool saveUntitled
	)
{
	bool needsSave = false;

	for (const auto& info : *itsDocList)
	{
		JXFileDocument* doc = dynamic_cast<JXFileDocument*>(info.doc);
		if (doc != nullptr &&
			(saveUntitled || doc->ExistsOnDisk()) && !doc->Save())
		{
			needsSave = true;
		}
	}

	return !needsSave;
}

/******************************************************************************
 FileDocumentIsOpen

	If there is a JXFileDocument that uses the specified file, we return it.

	With full RTTI support to allow safe downcasting, this could be generalized
	to DocumentIsOpen() by defining a pure virtual IsEqual() function in
	JXDocument and requiring derived classes to implement it appropriately.

 ******************************************************************************/

bool
JXDocumentManager::FileDocumentIsOpen
	(
	const JString&		fileName,
	JXFileDocument**	doc
	)
	const
{
	*doc = nullptr;

	// check that the file exists

	if (!JFileExists(fileName))
	{
		return false;
	}

	// search for an open JXFileDocument that uses this file

	for (const auto& info : *itsDocList)
	{
		const auto* fileDoc = dynamic_cast<const JXFileDocument*>(info.doc);
		if (fileDoc != nullptr)
		{
			bool onDisk;
			const JString docName = fileDoc->GetFullName(&onDisk);

			if (onDisk && JSameDirEntry(fileName, docName))
			{
				*doc = const_cast<JXFileDocument*>(fileDoc);
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 FindFile (virtual)

	Searches for the given file, assuming that it has been moved elsewhere.
	currPath should be the path the user selected.  This can help in the cases
	where an entire directory subtree was moved because then the requested file
	is probably still somewhere in the subtree.

	This function is virtual so derived classes can provide additional
	search techniques.

 ******************************************************************************/

bool
JXDocumentManager::FindFile
	(
	const JString&	fileName,
	const JString&	currPath,
	JString*		newFileName,
	const bool		askUser
	)
	const
{
	// if the file exists, we are done

	if (JFileExists(fileName))
	{
		*newFileName = fileName;
		return true;
	}

	// search the directory tree below currPath

	JString path, name, newPath;
	JSplitPathAndName(fileName, &path, &name);

	if (JSearchSubdirs(currPath, name, true, JString::kCompareCase, &newPath))
	{
		*newFileName = newPath + name;
		return true;
	}

	// check for known case of move/rename

	if (SearchFileMap(fileName, newFileName))
	{
		return true;
	}

	// ask the user to find it

	if (askUser)
	{
		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		JString instrMsg = JGetString("PleaseFind::JXDocumentManager", map, sizeof(map));

		while (true)
		{
			auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, JString::empty, JString::empty, instrMsg);
			if (!dlog->DoDialog())
			{
				break;
			}

			*newFileName = dlog->GetFullName();

			JString newPath, newName;
			JSplitPathAndName(*newFileName, &newPath, &newName);
			if (newName != name)
			{
				const JUtf8Byte* map2[] =
				{
					"name1", name.GetBytes(),
					"name2", newName.GetBytes()
				};
				JString warnMsg = JGetString("WarnDifferentName::JXDocumentManager", map2, sizeof(map2));
				if (!JGetUserNotification()->AskUserNo(warnMsg))
				{
					continue;
				}
			}

			JString trueName;
			const bool ok = JGetTrueName(*newFileName, &trueName);
			assert( ok );

			FileMap map;
			map.oldName = jnew JString(fileName);
			map.newName = jnew JString(trueName);
			itsFileMap->AppendItem(map);

			*newFileName = trueName;
			return true;
		}
	}

	newFileName->Clear();
	return false;
}

/******************************************************************************
 SearchFileMap (private)

 ******************************************************************************/

bool
JXDocumentManager::SearchFileMap
	(
	const JString&	fileName,
	JString*		newFileName
	)
	const
{
	const JSize mapCount = itsFileMap->GetItemCount();
	for (JIndex i=mapCount; i>=1; i--)
	{
		FileMap map          = itsFileMap->GetItem(i);
		const bool match = *(map.oldName) == fileName;
		if (match && JFileExists(*(map.newName)))
		{
			*newFileName = *(map.newName);
			return true;
		}
		else if (match)		// newName no longer exists (lazy checking)
		{
			jdelete map.oldName;
			jdelete map.newName;
			itsFileMap->RemoveItem(i);
		}
	}

	return false;
}

/******************************************************************************
 GetNewFileName

	Return a suitable name for a new document.  Since this is often
	called in the constructor for JXFileDocument, we return a JString&

 ******************************************************************************/

JString
JXDocumentManager::GetNewFileName()
{
	itsNewDocCount++;
	if (itsNewDocCount > 99)	// really big numbers look silly
	{
		itsNewDocCount = 1;
	}

	const JString indexStr(itsNewDocCount);
	const JUtf8Byte* map[] =
	{
		"i", indexStr.GetBytes()
	};
	return JGetString("NewDocName::JXDocumentManager", map, sizeof(map));
}

/******************************************************************************
 UpdateDocumentMenu

 ******************************************************************************/

void
JXDocumentManager::UpdateDocumentMenu
	(
	JXDocumentMenu* menu
	)
{
	if (menu->IsOpen())
	{
		auto* task = jnew JXFunctionTask(100, [menu]()
		{
			JXGetDocumentManager()->UpdateDocumentMenu(menu);
		},
		"JXDocumentManager::UpdateDocumentMenu",
		true);
		task->Start();
		return;
	}

	menu->RemoveAllItems();

	// It almost always is sorted, so we only pay O(N) instead of O(N^2).
	// (but we can't sort when document is created!)

	if (!itsDocList->IsSorted())
	{
		itsDocList->Sort();
	}

	// include partial paths for files with same names

	const JSize count = itsDocList->GetItemCount();
	JPtrArray<JString> nameList(JPtrArrayT::kDeleteAll, count+1);

	for (const auto& info : *itsDocList)
	{
		JXFileDocument* fdoc = dynamic_cast<JXFileDocument*>(info.doc);
		if (fdoc != nullptr && fdoc->ExistsOnDisk())
		{
			bool onDisk;
			nameList.Append(fdoc->GetFullName(&onDisk));
		}
		else
		{
			nameList.Append(info.doc->GetName());
		}
	}

	JIndex firstIndex = 0;
	JSize matchCount  = 0;
	for (JIndex i=1; i<=count; i++)
	{
		const JString& n1 = itsDocList->GetItem(i).doc->GetName();
		const JString& n2 = i == count ? JString::empty : itsDocList->GetItem(i+1).doc->GetName();
		if (n2 != n1)
		{
			if (firstIndex > 0)
			{
				for (JIndex j=firstIndex; j<=i; j++)
				{
					JStringIterator iter(nameList.GetItem(j));
					iter.SkipNext(matchCount);
					iter.RemoveAllPrev();
				}
			}
			else
			{
				nameList.SetItem(i, n1, JPtrArrayT::kDelete);
			}

			firstIndex = 0;
			continue;
		}

		const JString* s1 = nameList.GetItem(i);
		const JString* s2 = nameList.GetItem(i+1);
		JSize ml          = JString::CalcCharacterMatchLength(*s1, *s2);

		JStringIterator iter(*s1, JStringIterator::kStartAfterChar, ml);
		JUtf8Character c;
		while (iter.Prev(&c) && c != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
			ml--;
		}
		iter.Invalidate();

		if (ml == 1 && c == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
			ml = 0;		// might as well show full path
		}

		if (firstIndex == 0)
		{
			firstIndex = i;
			matchCount = ml;
		}
		else
		{
			matchCount = JMin(matchCount, ml);
		}
	}

	// build menu

	for (JIndex i=1; i<=count; i++)
	{
		menu->AppendItem(*nameList.GetItem(i));

		const DocInfo info = itsDocList->GetItem(i);
		if (info.doc->NeedsSave())
		{
			menu->SetItemFontStyle(i, JColorManager::GetDarkRedColor());
		}

		const JXImage* icon;
		if (info.doc->GetMenuIcon(&icon) &&
			icon->GetDisplay() == menu->GetDisplay())
		{
			menu->SetItemImage(i, const_cast<JXImage*>(icon), false);
		}

		if (itsWantShortcutFlag &&
			kFirstShortcut <= info.shortcut && info.shortcut <= kLastShortcut)
		{
			JString nmShortcut("Ctrl-");
			nmShortcut.Append(JUtf8Character(kShortcutChar[ info.shortcut ]));
			menu->SetItemNMShortcut(i, nmShortcut);
		}
	}
}

/******************************************************************************
 ActivateDocument

 ******************************************************************************/

void
JXDocumentManager::ActivateDocument
	(
	const JIndex index
	)
{
	const DocInfo info = itsDocList->GetItem(index);
	info.doc->Activate();
}

/******************************************************************************
 GetDocument

	The index is the JXDocumentMenu item index.

	We return bool because the document might have been closed while
	the menu was open.

 ******************************************************************************/

bool
JXDocumentManager::GetDocument
	(
	const JIndex	index,
	JXDocument**	doc
	)
	const
{
	if (itsDocList->IndexValid(index))
	{
		*doc = itsDocList->GetItem(index).doc;
		return true;
	}
	else
	{
		*doc = nullptr;
		return false;
	}
}

/******************************************************************************
 ShouldSafetySave

 ******************************************************************************/

void
JXDocumentManager::ShouldSafetySave
	(
	const bool doIt
	)
{
	itsPerformSafetySaveFlag = doIt;

	if (itsPerformSafetySaveFlag && !itsDocList->IsEmpty())
	{
		itsSafetySaveTask->Start();
	}
	else if (!itsPerformSafetySaveFlag)
	{
		itsSafetySaveTask->Stop();
	}
}

/******************************************************************************
 GetSafetySaveInterval

	Returns the safety save interval in seconds.

 ******************************************************************************/

JSize
JXDocumentManager::GetSafetySaveInterval()
	const
{
	return itsSafetySaveTask->GetPeriod() / kSecondsToMilliseconds;
}

/******************************************************************************
 SetSafetySaveInterval

 ******************************************************************************/

void
JXDocumentManager::SetSafetySaveInterval
	(
	const JSize deltaSeconds
	)
{
	itsSafetySaveTask->SetPeriod(deltaSeconds * kSecondsToMilliseconds);
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
JXDocumentManager::SafetySave
	(
	const SafetySaveReason reason
	)
{
	for (const auto& info : *itsDocList)
	{
		info.doc->SafetySave(reason);
	}
}

/******************************************************************************
 CompareDocNames (static private)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

std::weak_ordering
JXDocumentManager::CompareDocNames
	(
	const DocInfo& d1,
	const DocInfo& d2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&d1.doc->GetName()),
										  const_cast<JString*>(&d2.doc->GetName()));
}
