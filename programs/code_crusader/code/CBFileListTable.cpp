/******************************************************************************
 CBFileListTable.cpp

	Stores a unique ID + the modification time for each file.

	BASE CLASS = JXFileListTable

	Copyright (C) 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "CBFileListTable.h"
#include "CBSymbolUpdatePG.h"
#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include "CBDirList.h"
#include "CBSymbolDirector.h"
#include "CBSymbolList.h"
#include "CBCTreeDirector.h"
#include "CBCTree.h"
#include "CBJavaTreeDirector.h"
#include "CBJavaTree.h"
#include "CBPHPTreeDirector.h"
#include "CBPHPTree.h"
#include "cbGlobals.h"
#include <JDirInfo.h>
#include <jVCSUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kParsingFilesID = "ParsingFiles::CBFileListTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileListTable::CBFileListTable
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileListTable(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsFileInfo = jnew JArray<FileInfo>(100);
	assert( itsFileInfo != NULL );

	itsFileUsage              = NULL;
	itsReparseAllFlag         = kJFalse;
	itsChangedDuringParseFlag = kJFalse;
	itsLastUniqueID           = JFAID::kMinID;

	ListenTo(GetFullNameDataList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileListTable::~CBFileListTable()
{
	jdelete itsFileInfo;
}

/******************************************************************************
 Update

	Update the list of files.

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBFileListTable::Update
	(
	std::ostream&			link,
	CBProjectTree*		fileTree,
	const CBDirList&	dirList,
	CBSymbolDirector*	symbolDir,
	CBCTreeDirector*	cTreeDir,
	CBJavaTreeDirector*	javaTreeDir,
	CBPHPTreeDirector*	phpTreeDir
	)
{
JIndex i;

	// get everybody ready

	CBSymbolUpdatePG pg(link, 10);

	CBSymbolList* symbolList = symbolDir->GetSymbolList();
	CBCTree* cTree           = cTreeDir->GetCTree();
	CBJavaTree* javaTree     = javaTreeDir->GetJavaTree();
	CBPHPTree* phpTree       = phpTreeDir->GetPHPTree();

	const JBoolean reparseAll = JI2B(itsReparseAllFlag             ||
									 symbolList->NeedsReparseAll() ||
									 cTree->NeedsReparseAll()      ||
									 javaTree->NeedsReparseAll()   ||
									 phpTree->NeedsReparseAll());

	if (reparseAll)
		{
		RemoveAllFiles();
		}
	symbolDir->PrepareForListUpdate(reparseAll, pg);
	cTreeDir->PrepareForTreeUpdate(reparseAll);
	javaTreeDir->PrepareForTreeUpdate(reparseAll);
	phpTreeDir->PrepareForTreeUpdate(reparseAll);

	itsChangedDuringParseFlag = reparseAll;

	// create array to track which files still exist

	JArray<JBoolean> fileUsage(1000);
	itsFileUsage = &fileUsage;

	const JSize origFileCount = itsFileInfo->GetElementCount();
	for (i=1; i<=origFileCount; i++)
		{
		fileUsage.AppendElement(kJFalse);
		}

	// process each file

	ScanAll(fileTree, dirList, symbolList, cTree, javaTree, phpTree, pg);

	// collect files that no longer exist

	const JPtrArray<JString>& fileNameList = GetFullNameList();

	const JSize fileCount = itsFileInfo->GetElementCount();
	JPtrArray<JString> deadFileNameList(JPtrArrayT::kDeleteAll, fileCount+1);	// +1 to avoid passing zero
	JArray<JFAID_t> deadFileIDList(fileCount+1);

	for (i=1; i<=fileCount; i++)
		{
		if (!fileUsage.GetElement(i))
			{
			deadFileNameList.Append(*(fileNameList.NthElement(i)));
			deadFileIDList.AppendElement((itsFileInfo->GetElement(i)).id);
			}
		}

	itsFileUsage = NULL;

	// remove non-existent files

	RemoveFiles(deadFileNameList);
	symbolDir->ListUpdateFinished(deadFileIDList, pg);
	cTreeDir->TreeUpdateFinished(deadFileIDList);
	javaTreeDir->TreeUpdateFinished(deadFileIDList);
	phpTreeDir->TreeUpdateFinished(deadFileIDList);

	if (!deadFileIDList.IsEmpty())
		{
		itsChangedDuringParseFlag = kJTrue;
		}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 ScanAll (private)

	Recursively searches for files to parse, starting from each of the
	given directories.

	We do not recurse into symbolic links because this could produce loops,
	which I'm just too lazy to feel like catching, and the user can always
	add those directories separately.

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBFileListTable::ScanAll
	(
	CBProjectTree*		fileTree,
	const CBDirList&	dirList,
	CBSymbolList*		symbolList,
	CBCTree*			cTree,
	CBJavaTree*			javaTree,
	CBPHPTree*			phpTree,
	JProgressDisplay&	pg
	)
{
	const JSize dirCount = dirList.GetElementCount();
	if (dirCount > 0 || (fileTree->GetProjectRoot())->HasChildren())
		{
		pg.VariableLengthProcessBeginning(JGetString(kParsingFilesID), kJFalse, kJTrue);

		JPtrArray<JString> allSuffixList(JPtrArrayT::kDeleteAll);
		CBGetPrefsManager()->GetAllFileSuffixes(&allSuffixList);

		JString fullPath;
		JBoolean recurse;
		for (JIndex i=1; i<=dirCount; i++)
			{
			if (dirList.GetFullPath(i, &fullPath, &recurse))
				{
				ScanDirectory(fullPath, recurse,
							  allSuffixList, symbolList,
							  cTree, javaTree, phpTree,
							  pg);
				}
			}

		fileTree->ParseFiles(this, allSuffixList, symbolList,
							 cTree, javaTree, phpTree, pg);

		pg.ProcessFinished();
		}
}

/******************************************************************************
 ScanDirectory (private -- recursive)

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBFileListTable::ScanDirectory
	(
	const JString&				origPath,
	const JBoolean				recurse,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
{
	JString path;
	JDirInfo* info;
	if (!JGetTrueName(origPath, &path) ||
		!JDirInfo::Create(path, &info))
		{
		return;
		}

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);

		// If it's a directory, recurse.

		if (entry.IsDirectory() && recurse &&
			!entry.IsLink() && !JIsVCSDirectory(entry.GetName()))
			{
			ScanDirectory(entry.GetFullName(), recurse,
						  allSuffixList, symbolList,
						  cTree, javaTree, phpTree,
						  pg);
			}

		// If it's a file ending in one of the suffixes, parse it.

		else if (entry.IsFile())
			{
			JString trueName = entry.GetFullName();
			time_t modTime   = entry.GetModTime();
			if (entry.IsWorkingLink())
				{
				const JBoolean ok = JGetTrueName(entry.GetFullName(), &trueName);
				assert( ok );
				const JError err = JGetModificationTime(trueName, &modTime);
				assert( err.OK() );
				}

			ParseFile(trueName, allSuffixList, modTime,
					  symbolList, cTree, javaTree, phpTree);
			}

		pg.IncrementProgress();
		}

	jdelete info;
}

/******************************************************************************
 ParseFile

	Not private because called by CBFileNode.

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBFileListTable::ParseFile
	(
	const JString&				fullName,
	const JPtrArray<JString>&	allSuffixList,
	const time_t				modTime,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree
	)
{
	if (CBPrefsManager::FileMatchesSuffix(fullName, allSuffixList))
		{
		const CBTextFileType fileType = CBGetPrefsManager()->GetFileType(fullName);
		JFAID_t id;
		if (AddFile(fullName, fileType, modTime, &id))
			{
			itsChangedDuringParseFlag = kJTrue;

			symbolList->FileChanged(fullName, fileType, id);
			cTree->FileChanged(fullName, fileType, id);
			javaTree->FileChanged(fullName, fileType, id);
			phpTree->FileChanged(fullName, fileType, id);
			}
		}
}

/******************************************************************************
 AddFile (private)

	Returns kJTrue if the file is new or changed.  *id is always set to
	the id of the file.

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBFileListTable::AddFile
	(
	const JCharacter*		fullName,
	const CBTextFileType	fileType,
	const time_t			modTime,
	JFAID_t*				id
	)
{
	if (CBExcludeFromFileList(fileType))
		{
		return kJFalse;
		}

	JIndex index;
	const JBoolean isNew = JXFileListTable::AddFile(fullName, &index);
	FileInfo info        = itsFileInfo->GetElement(index);
	*id                  = info.id;

	itsFileUsage->SetElement(index, kJTrue);
	if (isNew)
		{
		return kJTrue;
		}
	else if (modTime != info.modTime)
		{
		info.modTime = modTime;
		itsFileInfo->SetElement(index, info);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UpdateFinished

	The worker thread updates its data, and we need to do the same.

 ******************************************************************************/

void
CBFileListTable::UpdateFinished()
{
	itsReparseAllFlag = kJFalse;
}

/******************************************************************************
 GetFileName

	We assert that the id is valid to save everybody else from having to do it.

 ******************************************************************************/

const JString&
CBFileListTable::GetFileName
	(
	const JFAID_t id
	)
	const
{
	JIndex index;
	const JBoolean found = IDToIndex(id, &index);
	assert( found );
	return *((GetFullNameList()).NthElement(index));
}

/******************************************************************************
 GetFileID

 ******************************************************************************/

JBoolean
CBFileListTable::GetFileID
	(
	const JCharacter*	trueName,
	JFAID_t*			id
	)
	const
{
	JString target(trueName);
	JIndex index;
	if ((GetFullNameList()).SearchSorted(&target, JOrderedSetT::kAnyMatch, &index))
		{
		*id = (itsFileInfo->GetElement(index)).id;
		return kJTrue;
		}
	else
		{
		*id = JFAID::kInvalidID;
		return kJFalse;
		}
}

/******************************************************************************
 IDToIndex (private)

	Return the index of the file with the specified ID.

 ******************************************************************************/

JBoolean
CBFileListTable::IDToIndex
	(
	const JFAID_t	id,
	JIndex*			index
	)
	const
{
	const JSize count = itsFileInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FileInfo info = itsFileInfo->GetElement(i);
		if (info.id == id)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 GetUniqueID (private)

	Return an unused ID.  Return zero if all ID's have been used.

 ******************************************************************************/

JFAID_t
CBFileListTable::GetUniqueID()
	const
{
	if (itsFileInfo->IsEmpty())
		{
		return JFAID::kMinID;
		}

	const JFAID_t prevUniqueID = JMax(itsLastUniqueID, (JFAID_t) itsFileInfo->GetElementCount());

	// this is relevant to the outmost do-while loop

	enum Range
	{
		kAboveLastUniqueID,
		kBelowLastUniqueID,
		kEmpty
	};
	Range idRange = kAboveLastUniqueID;

	do
		{
		// try the larger Id's first

		JFAID_t firstId, lastId;

		if (idRange == kAboveLastUniqueID && prevUniqueID < JFAID::kMaxID)
			{
			firstId = prevUniqueID + 1;
			lastId  = JFAID::kMaxID;
			}
		else if (idRange == kAboveLastUniqueID)
			{
			idRange = kBelowLastUniqueID;
			firstId = JFAID::kMinID;
			lastId  = JFAID::kMaxID;
			}
		else
			{
			assert( idRange == kBelowLastUniqueID );
			firstId = JFAID::kMinID;
			lastId  = prevUniqueID;
			}

		// try all possible id's in the given range

		for (JFAID_t id=firstId; id<=lastId; id++)
			{
			JIndex index;
			if (!IDToIndex(id, &index))
				{
				itsLastUniqueID = id;
				return id;
				}
			}

		if (idRange == kAboveLastUniqueID)
			{
			idRange = kBelowLastUniqueID;
			}
		else if (idRange == kBelowLastUniqueID)
			{
			idRange = kEmpty;
			}
		}
		while (idRange != kEmpty);

	// We've tried everything.  It's time to give up.

	return JFAID::kInvalidID;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBFileListTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
		assert( info != NULL );
		FilesAdded(*info);
		}
	else if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
		assert( info != NULL );
		itsFileInfo->RemoveElements(*info);
		if (itsFileUsage != NULL)
			{
			itsFileUsage->RemoveElements(*info);
			}
		}

	else if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );
		itsFileInfo->MoveElementToIndexWithMsg(*info);
		if (itsFileUsage != NULL)
			{
			itsFileUsage->MoveElementToIndexWithMsg(*info);
			}
		}
	else if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
		assert( info != NULL );
		itsFileInfo->SwapElementsWithMsg(*info);
		if (itsFileUsage != NULL)
			{
			itsFileUsage->SwapElementsWithMsg(*info);
			}
		}
	else if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kSorted))
		{
		assert_msg( 0, "CBFileListTable can't handle full sort of file list" );
		}

	else if (sender == GetFullNameDataList() && message.Is(JOrderedSetT::kElementChanged))
		{
		const JOrderedSetT::ElementChanged* info =
			dynamic_cast<const JOrderedSetT::ElementChanged*>(&message);
		assert( info != NULL );
		UpdateFileInfo(info->GetFirstIndex());
		}

	JXFileListTable::Receive(sender, message);
}

/******************************************************************************
 FilesAdded (private)

 ******************************************************************************/

void
CBFileListTable::FilesAdded
	(
	const JOrderedSetT::ElementsInserted& info
	)
{
	const JPtrArray<JString>& fileNameList = GetFullNameList();

	for (JIndex i = info.GetFirstIndex(); i <= info.GetLastIndex(); i++)
		{
		time_t t;
		const JError err = JGetModificationTime(*(fileNameList.NthElement(i)), &t);
		assert_ok( err );

		itsFileInfo->InsertElementAtIndex(i, FileInfo(GetUniqueID(), t));
		if (itsFileUsage != NULL)
			{
			itsFileUsage->InsertElementAtIndex(i, kJTrue);
			}
		}
}

/******************************************************************************
 UpdateFileInfo (private)

	The file name changed, so we generate a new ID.

 ******************************************************************************/

void
CBFileListTable::UpdateFileInfo
	(
	const JIndex index
	)
{
	FileInfo info = itsFileInfo->GetElement(index);

	info.id = GetUniqueID();

	const JString& fileName = *(GetFullNameList().NthElement(index));
	const JError err        = JGetModificationTime(fileName, &(info.modTime));
	assert_ok( err );

	itsFileInfo->SetElement(index, info);
	if (itsFileUsage != NULL)
		{
		itsFileUsage->SetElement(index, kJTrue);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBFileListTable::ReadSetup
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			symInput,
	const JFileVersion	symVers
	)
{
	std::istream* input          = (projVers <= 41 ? &projInput : symInput);
	const JFileVersion vers = (projVers <= 41 ? projVers   : symVers);
	if (vers < 40 || input == NULL)
		{
		itsReparseAllFlag = kJTrue;
		}
	else
		{
		ReadSetup(*input, vers);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBFileListTable::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	RemoveAllFiles();

	JSize fileCount;
	input >> fileCount;

	StopListening(GetFullNameDataList());

	JString fileName;
	for (JIndex i=1; i<=fileCount; i++)
		{
		JFAID_t id;
		time_t t;
		input >> fileName >> id >> t;

		JIndex index;
		const JBoolean isNew = JXFileListTable::AddFile(fileName, &index);
		assert( isNew );

		itsFileInfo->InsertElementAtIndex(index, FileInfo(id, t));
		}

	ListenTo(GetFullNameDataList());

	if (vers >= 80)
		{
		input >> itsLastUniqueID;
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBFileListTable::WriteSetup
	(
	std::ostream& projOutput,
	std::ostream* symOutput
	)
	const
{
	if (symOutput != NULL)
		{
		const JSize fileCount = itsFileInfo->GetElementCount();
		*symOutput << ' ' << fileCount;

		const JPtrArray<JString>& fileNameList = GetFullNameList();

		for (JIndex i=1; i<=fileCount; i++)
			{
			*symOutput << ' ' << *(fileNameList.NthElement(i));

			const FileInfo info = itsFileInfo->GetElement(i);
			*symOutput << ' ' << info.id;
			*symOutput << ' ' << info.modTime;
			}

		*symOutput << ' ' << itsLastUniqueID;
		*symOutput << ' ';
		}
}
