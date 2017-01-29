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

	Copyright (C) 1997-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXDocumentManager.h>
#include <JXFileDocument.h>
#include <JXDocumentMenu.h>
#include <JXUpdateDocMenuTask.h>
#include <JXTimerTask.h>
#include <JXImage.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

static const JCharacter* kNewDocName = "Untitled ";
const JInteger kFirstShortcut        = 0;
const JInteger kLastShortcut         = 9;

static const JCharacter kShortcutChar[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

const Time kDefaultSafetySavePeriod = 30000;	// 30 seconds (milliseconds)
const JSize kSecondsToMilliseconds  = 1000;

// JBroadcaster message types

const JCharacter* JXDocumentManager::kDocMenuNeedsUpdate =
	"DocMenuNeedsUpdate::JXDocumentManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocumentManager::JXDocumentManager
	(
	const JBoolean wantShortcuts
	)
	:
	itsWantShortcutFlag( wantShortcuts )
{
	itsDocList = jnew JArray<DocInfo>;
	assert( itsDocList != NULL );
	itsDocList->SetCompareFunction(CompareDocNames);
	itsDocList->SetSortOrder(JListT::kSortAscending);

	itsNewDocCount = 0;

	itsFileMap = jnew JArray<FileMap>;
	assert( itsFileMap != NULL );

	itsPerformSafetySaveFlag = kJTrue;

	itsSafetySaveTask = jnew JXTimerTask(kDefaultSafetySavePeriod);
	assert( itsSafetySaveTask != NULL );
	ListenTo(itsSafetySaveTask);

	itsUpdateDocMenuTask = NULL;

	JXDisplay* d = (JXGetApplication())->GetCurrentDisplay();
	itsDefaultMenuIcon = jnew JXImage(d, jx_plain_file_small);
	assert( itsDefaultMenuIcon != NULL );

	JXSetDocumentManager(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocumentManager::~JXDocumentManager()
{
	assert( itsDocList->IsEmpty() );
	jdelete itsDocList;

	const JSize count = itsFileMap->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FileMap file = itsFileMap->GetElement(i);
		jdelete file.oldName;
		jdelete file.newName;
		}
	jdelete itsFileMap;

	jdelete itsSafetySaveTask;
	jdelete itsUpdateDocMenuTask;
//	jdelete itsDefaultMenuIcon;	// can't because JXDisplay already deleted
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

	const JSize docCount = itsDocList->GetElementCount();
	for (JInteger i=kFirstShortcut; i<=kLastShortcut; i++)
		{
		JBoolean found = kJFalse;
		for (JIndex j=1; j<=docCount; j++)
			{
			if ((itsDocList->GetElement(j)).shortcut == i)
				{
				found = kJTrue;
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

	itsDocList->AppendElement(info);
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
	const JSize count = itsDocList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		DocInfo info = itsDocList->GetElement(i);
		if (info.doc == doc)
			{
			itsDocList->RemoveElement(i);

			// assign the shortcut to the first item that doesn't have a shortcut

			if (info.shortcut != kNoShortcutForDoc)
				{
				for (JIndex j=1; j<=count-1; j++)
					{
					DocInfo info1 = itsDocList->GetElement(j);
					if (info1.shortcut == kNoShortcutForDoc)
						{
						info1.shortcut = info.shortcut;
						itsDocList->SetElement(j, info1);
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
	if (itsUpdateDocMenuTask == NULL)
		{
		itsUpdateDocMenuTask = jnew JXUpdateDocMenuTask(this);
		assert( itsUpdateDocMenuTask != NULL );
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

	Call this with kJTrue if a document must remain open even if nobody else
	needs it.

 ******************************************************************************/

void
JXDocumentManager::DocumentMustStayOpen
	(
	JXDocument*		doc,
	const JBoolean	stayOpen
	)
{
	const JSize count = itsDocList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		DocInfo info = itsDocList->GetElement(i);
		if (info.doc == doc)
			{
			info.keepOpen = stayOpen;
			itsDocList->SetElement(i, info);
			break;
			}
		}

	if (stayOpen == kJFalse)
		{
		CloseDocuments();
		}
}

/******************************************************************************
 OKToCloseDocument

	Returns kJTrue if the given document can be closed.

 ******************************************************************************/

JBoolean
JXDocumentManager::OKToCloseDocument
	(
	JXDocument* doc
	)
	const
{
	const JSize count = itsDocList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const DocInfo info = itsDocList->GetElement(i);
		if (info.doc != doc && (info.doc)->NeedDocument(doc))
			{
			return kJFalse;
			}
		}

	return kJTrue;
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
	JSize count = itsDocList->GetElementCount();

	JSize closeCount;
	do
		{
		closeCount = 0;
		for (JIndex i=1; i<=count; i++)
			{
			const DocInfo info = itsDocList->GetElement(i);
			if (!(info.doc)->IsActive() && !(info.keepOpen) &&
				OKToCloseDocument(info.doc) && (info.doc)->Close())
				{
				closeCount++;
				count = itsDocList->GetElementCount();	// several documents may have closed
				}
			}
		}
		while (closeCount > 0);
}

/******************************************************************************
 FileDocumentIsOpen

	If there is a JXFileDocument that uses the specified file, we return it.

	With full RTTI support to allow safe downcasting, this could be generalized
	to DocumentIsOpen() by defining a pure virtual IsEqual() function in
	JXDocument and requiring derived classes to implement it appropriately.

 ******************************************************************************/

JBoolean
JXDocumentManager::FileDocumentIsOpen
	(
	const JCharacter*	fileName,
	JXFileDocument**	doc
	)
	const
{
	*doc = NULL;

	// check that the file exists

	if (!JFileExists(fileName))
		{
		return kJFalse;
		}

	// search for an open JXFileDocument that uses this file

	const JSize count = itsDocList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const DocInfo info            = itsDocList->GetElement(i);
		const JXFileDocument* fileDoc = dynamic_cast<const JXFileDocument*>(info.doc);
		if (fileDoc != NULL)
			{
			JBoolean onDisk;
			const JString docName = fileDoc->GetFullName(&onDisk);

			if (onDisk && JSameDirEntry(fileName, docName))
				{
				*doc = const_cast<JXFileDocument*>(fileDoc);
				return kJTrue;
				}
			}
		}

	return kJFalse;
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

JBoolean
JXDocumentManager::FindFile
	(
	const JCharacter*	fileName,
	const JCharacter*	currPath,
	JString*			newFileName,
	const JBoolean		askUser
	)
	const
{
	// if the file exists, we are done

	if (JFileExists(fileName))
		{
		*newFileName = fileName;
		return kJTrue;
		}

	// search the directory tree below currPath

	JString path, name, newPath;
	JSplitPathAndName(fileName, &path, &name);

	if (JSearchSubdirs(currPath, name, kJTrue, kJTrue, &newPath))
		{
		*newFileName = newPath + name;
		return kJTrue;
		}

	// check for known case of move/rename

	if (SearchFileMap(fileName, newFileName))
		{
		return kJTrue;
		}

	// ask the user to find it

	if (askUser)
		{
		JString instrMsg = "Unable to locate ";
		instrMsg += fileName;
		instrMsg += "\n\nPlease find it.";

		while ((JGetChooseSaveFile())->ChooseFile("New name of file:", instrMsg, newFileName))
			{
			JString newPath, newName;
			JSplitPathAndName(*newFileName, &newPath, &newName);
			if (newName != name)
				{
				JString warnMsg = name;
				warnMsg += " was requested.\n\nYou selected ";
				warnMsg += newName;
				warnMsg += ".\n\nAre you sure that this is correct?";
				if (!(JGetUserNotification())->AskUserNo(warnMsg))
					{
					continue;
					}
				}

			JString trueName;
			const JBoolean ok = JGetTrueName(*newFileName, &trueName);
			assert( ok );

			FileMap map;
			map.oldName = jnew JString(fileName);
			assert( map.oldName != NULL );
			map.newName = jnew JString(trueName);
			assert( map.newName != NULL );
			itsFileMap->AppendElement(map);

			*newFileName = trueName;
			return kJTrue;
			}
		}

	newFileName->Clear();
	return kJFalse;
}

/******************************************************************************
 SearchFileMap (private)

 ******************************************************************************/

JBoolean
JXDocumentManager::SearchFileMap
	(
	const JCharacter*	fileName,
	JString*			newFileName
	)
	const
{
	const JSize mapCount = itsFileMap->GetElementCount();
	for (JIndex i=mapCount; i>=1; i--)
		{
		FileMap map          = itsFileMap->GetElement(i);
		const JBoolean match = JConvertToBoolean( *(map.oldName) == fileName );
		if (match && JFileExists(*(map.newName)))
			{
			*newFileName = *(map.newName);
			return kJTrue;
			}
		else if (match)		// newName no longer exists (lazy checking)
			{
			jdelete map.oldName;
			jdelete map.newName;
			itsFileMap->RemoveElement(i);
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetNewFileName

	Return a suitable name for a new document.  Since this is often
	called in the constructor for JXFileDocument, we return a JString&

 ******************************************************************************/

const JString&
JXDocumentManager::GetNewFileName()
{
	itsNewDocCount++;
	if (itsNewDocCount > 99)	// really big numbers look silly
		{
		itsNewDocCount = 1;
		}

	itsLastNewFileName = kNewDocName + JString(itsNewDocCount, JString::kBase10);
	return itsLastNewFileName;
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
		DocumentMenusNeedUpdate();
		return;
		}

	menu->RemoveAllItems();

	// It almost always is sorted, so we only pay O(N) instead of O(N^2).
	// (but we can't sort when document is created!)

	if (!itsDocList->IsSorted())
		{
		itsDocList->Sort();
		}

	const JSize count = itsDocList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		DocInfo info        = itsDocList->GetElement(i);
		const JString& name = (info.doc)->GetName();

		menu->AppendItem(name);
		if ((info.doc)->NeedsSave())
			{
			menu->SetItemFontStyle(i, (menu->GetColormap())->GetDarkRedColor());
			}

		const JXImage* icon;
		if ((info.doc)->GetMenuIcon(&icon) &&
			icon->GetDisplay()  == menu->GetDisplay() &&
			icon->GetColormap() == menu->GetColormap())
			{
			menu->SetItemImage(i, const_cast<JXImage*>(icon), kJFalse);
			}

		if (itsWantShortcutFlag &&
			kFirstShortcut <= info.shortcut && info.shortcut <= kLastShortcut)
			{
			const JXMenu::Style style = JXMenu::GetDefaultStyle();
			JString nmShortcut;
			if (style == JXMenu::kWindowsStyle)
				{
				nmShortcut = "Ctrl-0";
				}
			else
				{
				assert( style == JXMenu::kMacintoshStyle );
				nmShortcut = "Meta-0";
				}
			nmShortcut.SetCharacter(nmShortcut.GetLength(),
									kShortcutChar [ info.shortcut ]);
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
	const DocInfo info = itsDocList->GetElement(index);
	(info.doc)->Activate();
}

/******************************************************************************
 GetDocument

	The index is the JXDocumentMenu item index.

	We return JBoolean because the document might have been closed while
	the menu was open.

 ******************************************************************************/

JBoolean
JXDocumentManager::GetDocument
	(
	const JIndex	index,
	JXDocument**	doc
	)
	const
{
	if (itsDocList->IndexValid(index))
		{
		*doc = (itsDocList->GetElement(index)).doc;
		return kJTrue;
		}
	else
		{
		*doc = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 ShouldSafetySave

 ******************************************************************************/

void
JXDocumentManager::ShouldSafetySave
	(
	const JBoolean doIt
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
	const JSize docCount = itsDocList->GetElementCount();
	for (JIndex i=1; i<=docCount; i++)
		{
		DocInfo info = itsDocList->GetElement(i);
		(info.doc)->SafetySave(reason);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDocumentManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSafetySaveTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		SafetySave(JXDocumentManager::kTimer);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 CompareDocNames (static private)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

JListT::CompareResult
JXDocumentManager::CompareDocNames
	(
	const DocInfo& d1,
	const DocInfo& d2
	)
{
	return JCompareStringsCaseInsensitive(const_cast<JString*>(&((d1.doc)->GetName())),
										  const_cast<JString*>(&((d2.doc)->GetName())));
}
