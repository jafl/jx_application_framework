/******************************************************************************
 CBTree.cpp

	This class encapsulates the inheritance tree of a set of CBClasses.

	Derived classes must implement:

		ViewSelectedSources
			Open the files containing the definitions of the selected classes.

		ViewSelectedHeaders
			Open the files containing the declarations of the selected classes.

	Derived classes can override:

		StreamOut
			Write out extra data after calling CBTree::StreamOut().

	BASE CLASS = JContainer

	Copyright (C) 1995-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBTree.h"
#include "CBClass.h"
#include "CBTreeDirector.h"
#include "CBDirList.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXFontManager.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JPainter.h>
#include <JLatentPG.h>
#include <JPtrArray-JString.h>
#include <JSimpleProcess.h>
#include <jGlobals.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JSize kBlockSize = 100;

// Code Mill info

const JFileVersion kCodeMillDataVersion                  = 0;
static const JCharacter* kCodeMillProgramName            = "code_mill";
static const JCharacter* kCodeMillDeleteInputFilesOption = "--delete";
static const JCharacter* kCodeMillOutputPathOption       = "--output_path";

// JBroadcaster message types

const JCharacter* CBTree::kChanged              = "Changed::CBTree";
const JCharacter* CBTree::kBoundsChanged        = "BoundsChanged::CBTree";
const JCharacter* CBTree::kNeedsRefresh         = "NeedsRefresh::CBTree";
const JCharacter* CBTree::kFontSizeChanged      = "FontSizeChanged::CBTree";

const JCharacter* CBTree::kPrepareForParse      = "PrepareForParse::CBTree";
const JCharacter* CBTree::kParseFinished        = "ParseFinished::CBTree";

const JCharacter* CBTree::kClassSelected        = "ClassSelected::CBTree";
const JCharacter* CBTree::kClassDeselected      = "ClassDeselected::CBTree";
const JCharacter* CBTree::kAllClassesDeselected = "AllClassesDeselected::CBTree";

// string ID's

static const JCharacter* kRequiresCodeMillID = "RequiresCodeMill::CBTree";

/******************************************************************************
 Constructor

	In the 2nd version, dirList can be NULL

 ******************************************************************************/

CBTree::CBTree
	(
	CBClassStreamInFn*		streamInFn,
	CBTreeDirector*			director,
	const CBTextFileType	fileType,
	const JSize				marginWidth
	)
	:
	JContainer(),
	itsFileType(fileType),
	itsMarginWidth(marginWidth)
{
	CBTreeX(director, streamInFn);
}

CBTree::CBTree
	(
	std::istream&				projInput,
	const JFileVersion		projVers,
	std::istream*				setInput,
	const JFileVersion		setVers,
	std::istream*				symStream,
	const JFileVersion		origSymVers,
	CBClassStreamInFn*		streamInFn,
	CBTreeDirector*			director,
	const CBTextFileType	fileType,
	const JSize				marginWidth,
	CBDirList*				dirList
	)
	:
	JContainer(),
	itsFileType(fileType),
	itsMarginWidth(marginWidth)
{
JIndex i;

	CBTreeX(director, streamInFn);

	std::istream* symInput          = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers = (projVers <= 41 ? projVers   : origSymVers);
	const JBoolean useSetProjData = JI2B( setInput == NULL || setVers < 71 );
	const JBoolean useSymProjData = JI2B( symInput == NULL || symVers < 71 );

/* symbol file */

	if (projVers < 71)
		{
		projInput >> itsWidth >> itsHeight;
		}
	if (!useSymProjData)	// overwrite
		{
		*symInput >> itsWidth >> itsHeight;
		}

/* settings file */

	if (projVers < 71)
		{
		if (projVers >= 12)
			{
			projInput >> itsShowLoneClassesFlag;
			if (projVers >= 13)
				{
				projInput >> itsShowLoneStructsFlag;
				}
			else
				{
				itsShowLoneStructsFlag = itsShowLoneClassesFlag;
				}
			}
		if (projVers >= 10)
			{
			projInput >> itsShowEnumsFlag;
			}

		if (projVers >= 19)
			{
			projInput >> itsNeedsMinimizeMILinksFlag;
			}
		else
			{
			itsNeedsMinimizeMILinksFlag = kJTrue;
			}
		}
	if (!useSetProjData)	// overwrite
		{
		*setInput >> itsShowLoneClassesFlag;
		*setInput >> itsShowLoneStructsFlag;
		*setInput >> itsShowEnumsFlag;
		*setInput >> itsNeedsMinimizeMILinksFlag;
		}

/* symbol file */

	if (symInput != NULL)
		{
		JBoolean classNamesSorted = kJTrue;
		if (symVers < 7)
			{
			*symInput >> classNamesSorted;
			}

		// get number of classes in tree

		JSize classCount;
		*symInput >> classCount;

		// read in each class

		itsClassesByFull->SetBlockSize(classCount+1);	// avoid repeated realloc
		itsVisibleByName->SetBlockSize(classCount+1);

		for (i=1; i<=classCount; i++)
			{
			CBClass* newClass = streamInFn(*symInput, symVers, this);
			itsClassesByFull->Append(newClass);
			if (newClass->IsVisible())
				{
				itsVisibleByName->InsertSorted(newClass);
				}
			}

		itsClassesByFull->SetBlockSize(kBlockSize);
		itsVisibleByName->SetBlockSize(kBlockSize);

		if (symVers >= 18)
			{
			const JSize geomCount = itsVisibleByName->GetElementCount();
			itsVisibleByGeom->SetBlockSize(geomCount+1);	// avoid repeated realloc
			for (i=1; i<=geomCount; i++)
				{
				JIndex j;
				*symInput >> j;
				itsVisibleByGeom->Append(itsVisibleByName->NthElement(j));
				}
			itsVisibleByGeom->SetBlockSize(kBlockSize);
			}

		for (i=1; i<=classCount; i++)
			{
			(itsClassesByFull->NthElement(i))->FindParentsAfterRead();
			}

		// We can only sort this after calling FindParentsAfterRead(),
		// because this function uses the order saved in the file.

		if (!classNamesSorted || symVers < 43)
			{
			itsClassesByFull->Sort();
			}
		}

/* project file */

	// We read this way down here because in version <= 41,
	// everything was in a single file.

	if (dirList != NULL)
		{
		dirList->ReadDirectories(projInput, projVers);
		}

	if (projVers < 40)
		{
		// toss file name and modification date

		JSize fileCount;
		projInput >> fileCount;

		JString name;
		time_t t;
		for (i=1; i<=fileCount; i++)
			{
			projInput >> name >> t;
			}
		}

/* symbol file */

	// check for change in file suffixes

	JBoolean needsReparse = JI2B(projVers < 49);
	if (projVers >= 9)
		{
		JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
		if (projVers < 71)
			{
			projInput >> suffixList;
			}
		if (!useSymProjData)	// overwrite
			{
			*symInput >> suffixList;
			}
		if (!JSameStrings(*itsSuffixList, suffixList, kJTrue))
			{
			needsReparse = kJTrue;
			}
		}

	if (needsReparse)
		{
		NextUpdateMustReparseAll();
		}
}

// private

void
CBTree::CBTreeX
	(
	CBTreeDirector*		director,
	CBClassStreamInFn*	streamInFn
	)
{
	itsDirector = director;
	itsFontSize = kJDefaultFontSize;

	itsClassesByFull = jnew JPtrArray<CBClass>(JPtrArrayT::kDeleteAll, kBlockSize);
	assert( itsClassesByFull != NULL );
	itsClassesByFull->SetCompareFunction(CompareClassFullNames);
	itsClassesByFull->SetSortOrder(JOrderedSetT::kSortAscending);

	itsVisibleByGeom = jnew JPtrArray<CBClass>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByGeom != NULL );

	itsVisibleByName = jnew JPtrArray<CBClass>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByName != NULL );
	itsVisibleByName->SetCompareFunction(CompareClassNames);
	itsVisibleByName->SetSortOrder(JOrderedSetT::kSortAscending);

	itsSuffixList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSuffixList != NULL );
	CBGetPrefsManager()->GetFileSuffixes(itsFileType, itsSuffixList);

	itsCollapsedList = NULL;

	InstallOrderedSet(itsClassesByFull);

	itsWidth = itsHeight = 0;
	itsBroadcastClassSelFlag = kJTrue;

	itsShowLoneClassesFlag  = kJTrue;
	itsShowLoneStructsFlag  = kJTrue;
	itsShowEnumsFlag        = kJTrue;
	itsDrawMILinksOnTopFlag = kJTrue;

	itsMinimizeMILinksFlag      = kJFalse;
	itsNeedsMinimizeMILinksFlag = kJFalse;

	itsReparseAllFlag           = kJFalse;
	itsChangedDuringParseFlag   = kJFalse;
	itsBeganEmptyFlag           = kJFalse;

	itsStreamInFn = streamInFn;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTree::~CBTree()
{
	jdelete itsClassesByFull;
	jdelete itsVisibleByGeom;
	jdelete itsVisibleByName;
	jdelete itsCollapsedList;
	jdelete itsSuffixList;
}

/******************************************************************************
 ReloadSetup (virtual)

	We cannot share this with the load ctor because that contains too much
	legacy code required for reading old project files.

 ******************************************************************************/

void
CBTree::ReloadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
JIndex i;

	itsReparseAllFlag = kJFalse;

	itsClassesByFull->DeleteAll();
	itsVisibleByGeom->RemoveAll();
	itsVisibleByName->RemoveAll();

	input >> itsWidth >> itsHeight;

	// get number of classes in tree

	JSize classCount;
	input >> classCount;

	// read in each class

	itsClassesByFull->SetBlockSize(classCount+1);	// avoid repeated realloc
	itsVisibleByName->SetBlockSize(classCount+1);

	for (i=1; i<=classCount; i++)
		{
		CBClass* newClass = itsStreamInFn(input, vers, this);
		newClass->CalcFrame();

		itsClassesByFull->Append(newClass);
		if (newClass->IsVisible())
			{
			itsVisibleByName->InsertSorted(newClass);
			}
		}

	itsClassesByFull->SetBlockSize(kBlockSize);
	itsVisibleByName->SetBlockSize(kBlockSize);

	// read in geometry ordering

	const JSize geomCount = itsVisibleByName->GetElementCount();

	itsVisibleByGeom->SetBlockSize(geomCount+1);	// avoid repeated realloc

	for (i=1; i<=geomCount; i++)
		{
		JIndex j;
		input >> j;
		itsVisibleByGeom->Append(itsVisibleByName->NthElement(j));
		}

	itsVisibleByGeom->SetBlockSize(kBlockSize);

	// link up each class with parents

	for (i=1; i<=classCount; i++)
		{
		(itsClassesByFull->NthElement(i))->FindParentsAfterRead();
		}

	// check for change in file suffixes

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	input >> suffixList;
	if (!JSameStrings(*itsSuffixList, suffixList, kJTrue))
		{
		NextUpdateMustReparseAll();
		}

	RebuildTree();	// calls X server, so cannot be done in child process

	Broadcast(Changed());
}

/******************************************************************************
 StreamOut (virtual)

	dirList can be NULL

 ******************************************************************************/

void
CBTree::StreamOut
	(
	std::ostream&			projOutput,
	std::ostream*			setOutput,
	std::ostream*			symOutput,
	const CBDirList*	dirList
	)
	const
{
JIndex i;

/* project file */

	if (dirList != NULL)
		{
		dirList->WriteDirectories(projOutput);
		}

/* symbol file */

	if (symOutput != NULL)
		{
		*symOutput << ' ' << itsWidth;
		*symOutput << ' ' << itsHeight;
		}

/* settings file */

	if (setOutput != NULL)
		{
		*setOutput << ' ' << itsShowLoneClassesFlag;
		*setOutput << ' ' << itsShowLoneStructsFlag;
		*setOutput << ' ' << itsShowEnumsFlag;
		*setOutput << ' ' << itsNeedsMinimizeMILinksFlag;
		}

/* symbol file */

	if (symOutput != NULL)
		{
		// write the number of classes in tree

		const JSize classCount = itsClassesByFull->GetElementCount();
		*symOutput << ' ' << classCount;

		// write each class

		for (i=1; i<=classCount; i++)
			{
			*symOutput << ' ';
			(itsClassesByFull->NthElement(i))->StreamOut(*symOutput);
			}

		// write geometry ordering

		const JSize geomCount = itsVisibleByGeom->GetElementCount();
		assert( geomCount == itsVisibleByName->GetElementCount() );
		for (i=1; i<=geomCount; i++)
			{
			JIndex j;
			const JBoolean found = itsVisibleByName->Find(itsVisibleByGeom->NthElement(i), &j);
			assert( found );
			*symOutput << ' ' << j;
			}

		// save file suffixes so we can check if they change

		*symOutput << ' ' << *itsSuffixList;

		*symOutput << ' ';
		}
}

/******************************************************************************
 FileTypesChanged

	We can't use Receive() because this must be called -before-
	CBProjectDocument updates everything.

 ******************************************************************************/

void
CBTree::FileTypesChanged
	(
	const CBPrefsManager::FileTypesChanged& info
	)
{
	if (info.Changed(itsFileType))
		{
		CBGetPrefsManager()->GetFileSuffixes(itsFileType, itsSuffixList);
		NextUpdateMustReparseAll();
		}
}

/******************************************************************************
 PrepareForUpdate (virtual)

	Get ready to parse files that have changed or been created and to
	throw out classes in files that no longer exist.

	deadFileIDList can be NULL.

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBTree::PrepareForUpdate
	(
	const JBoolean reparseAll
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	if (!CBInUpdateThread())
		{
		Broadcast(PrepareForParse());
		}

	// save collapsed classes

	itsCollapsedList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsCollapsedList != NULL );
	SaveCollapsedClasses(itsCollapsedList);

	if (reparseAll)
		{
		// toss everything

		itsClassesByFull->DeleteAll();
		itsVisibleByGeom->RemoveAll();
		itsVisibleByName->RemoveAll();
		}
	itsChangedDuringParseFlag = reparseAll;

	itsBeganEmptyFlag = itsClassesByFull->IsEmpty();
}

/******************************************************************************
 UpdateFinished (virtual)

	Cleans up after updating files.  Returns kJTrue if changes were found.

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBTree::UpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	// toss files that no longer exist

	const JSize fileCount = deadFileList.GetElementCount();
	for (JIndex i=1; i<=fileCount; i++)
		{
		RemoveFile(deadFileList.GetElement(i));
		}

	// restore collapsed classes

	const JBoolean forceRecalcVisible = RestoreCollapsedClasses(*itsCollapsedList);
	jdelete itsCollapsedList;
	itsCollapsedList = NULL;

	// rebuild tree

	if (itsChangedDuringParseFlag)
		{
		RebuildTree();
		}
	else if (forceRecalcVisible && !CBInUpdateThread())
		{
		RecalcVisible();
		}

	if (!CBInUpdateThread())
		{
		Broadcast(ParseFinished(itsChangedDuringParseFlag));
		if (itsChangedDuringParseFlag)
			{
			Broadcast(Changed());
			}

		itsReparseAllFlag = kJFalse;
		}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 FileChanged

	Returns kJTrue if the file should be parsed.

	Not private because it is called by CBFileListTable::ParseFile().

 ******************************************************************************/

void
CBTree::FileChanged
	(
	const JCharacter*		fileName,
	const CBTextFileType	fileType,
	const JFAID_t			id
	)
{
	if (fileType == itsFileType)
		{
		if (!itsBeganEmptyFlag)
			{
			RemoveFile(id);
			}
		ParseFile(fileName, id);
		}
}

/******************************************************************************
 RemoveFile (private)

	Remove the specified file and all associated classes.

	*** Caller must call RebuildTree().

 ******************************************************************************/

void
CBTree::RemoveFile
	(
	const JFAID_t id
	)
{
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=classCount; i>=1; i--)
		{
		const CBClass* theClass = itsClassesByFull->NthElement(i);
		JFAID_t fileID;
		if (theClass->GetFileID(&fileID) && fileID == id)
			{
			itsClassesByFull->DeleteElement(i);
			itsVisibleByGeom->Remove(theClass);
			itsVisibleByName->Remove(theClass);

			itsChangedDuringParseFlag = kJTrue;
			}
		}
}

/******************************************************************************
 SaveCollapsedClasses (private)

	Save the names of the collapsed classes in the given string list.

 ******************************************************************************/

void
CBTree::SaveCollapsedClasses
	(
	JPtrArray<JString>* list
	)
	const
{
	const JSize count = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBClass* theClass = itsClassesByFull->NthElement(i);
		if (theClass->IsCollapsed())
			{
			list->Append(theClass->GetFullName());
			}
		}
}

/******************************************************************************
 RestoreCollapsedClasses (private)

	Collapse the classes in the given list.

	*** Caller must call RecalcVisible().

 ******************************************************************************/

JBoolean
CBTree::RestoreCollapsedClasses
	(
	const JPtrArray<JString>& list
	)
{
	JBoolean changed = kJFalse;

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fullName = list.NthElement(i);
		CBClass* theClass;
		if (FindClass(*fullName, &theClass) && !theClass->IsCollapsed())
			{
			theClass->SetCollapsed(kJTrue);
			changed = kJTrue;
			}
		}

	return changed;
}

/******************************************************************************
 AddClass (private)

	*** Caller must ensure that RebuildTree() gets called.

 ******************************************************************************/

void
CBTree::AddClass
	(
	CBClass* newClass
	)
{
	itsClassesByFull->InsertSorted(newClass);
	if (newClass->IsVisible())
		{
		itsVisibleByName->InsertSorted(newClass);
		itsVisibleByGeom->RemoveAll();
		}

	itsChangedDuringParseFlag = kJTrue;
}

/******************************************************************************
 RebuildTree (private)

	Recalculates all parents and then places all classes.  Returns the
	resulting width and height.

	Because one is not required to fully qualify every parent
	(e.g. JBroadcaster::Message), some classes can not find their parents
	immediately.  Thus, we keep looping until nobody can find any more
	of their parents, and then let them create ghosts.

	With this algorithm, progress is guaranteed at every step.
	In each pass, the following classes will find their parents:
		1)	All classes inheriting from classes at global scope or
			inheriting from fully qualified classes.
		2)	All classes inheriting from (1)
		3)	All classes inheriting from (2)
		4)  ...

 ******************************************************************************/

void
CBTree::RebuildTree()
{
JIndex i;

	// delete ghost classes and clear all parent-child connections

	JSize classCount = itsClassesByFull->GetElementCount();
	for (i=classCount; i>=1; i--)
		{
		CBClass* theClass = itsClassesByFull->NthElement(i);
		if (theClass->IsGhost())
			{
			itsClassesByFull->DeleteElement(i);
			itsVisibleByGeom->Remove(theClass);
			itsVisibleByName->Remove(theClass);
			classCount--;
			}
		else
			{
			theClass->ClearConnections();
			}
		}

	// Keep looking for parents as long as somebody finds another parent.
	// Don't allow ghosts since not all parents have been found.

	JBoolean progress;
	do
		{
		progress = kJFalse;
		for (i=1; i<=classCount; i++)
			{
			const JBoolean foundAnotherParent =
				(itsClassesByFull->NthElement(i))->FindParents(kJFalse);
			progress = JConvertToBoolean( progress || foundAnotherParent );
			}
		}
		while (progress);

	// Now that no more progress is being made, allow ghosts to be created.
	// (Inserting ghosts does not cause classes to be skipped.)

	for (i=1; i<=classCount; i++)
		{
		(itsClassesByFull->NthElement(i))->FindParents(kJTrue);
		classCount = GetElementCount();
		}

	// Now that all parents have been found, we can hide lone classes and
	// classes whose parents are hidden or collapsed.

	RecalcVisible(kJFalse, kJTrue);
}

/******************************************************************************
 RecalcVisible (private)

	Applies itsShow*Flags and hides classes whose parents are hidden or
	collapsed.

	If necessary or requested, it rebuilds the list of visible classes
	and adjusts their placement in the tree.

 ******************************************************************************/

void
CBTree::RecalcVisible
	(
	const JBoolean forceRebuildVisible,
	const JBoolean forcePlaceAll
	)
{
	const JSize classCount  = itsClassesByFull->GetElementCount();
	JBoolean rebuildVisible = forceRebuildVisible;

	// set visibility of lone classes and enums

	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass       = itsClassesByFull->NthElement(i);
		const JBoolean isStruct = theClass->IsStruct();
		const JBoolean isEnum   = theClass->IsEnum();

		// enums are controlled by one flag

		if (isEnum && theClass->IsVisible() != itsShowEnumsFlag)
			{
			rebuildVisible = kJTrue;
			theClass->SetVisible(itsShowEnumsFlag);
			}

		// classes and structs with no parents or children are "lone"

		else if (!isEnum && !theClass->HasParents() && !theClass->HasChildren() &&
				 ((!isStruct && theClass->IsVisible() != itsShowLoneClassesFlag) ||
				  ( isStruct && theClass->IsVisible() != itsShowLoneStructsFlag)))
			{
			rebuildVisible = kJTrue;
			if (isStruct)
				{
				theClass->SetVisible(itsShowLoneStructsFlag);
				}
			else
				{
				theClass->SetVisible(itsShowLoneClassesFlag);
				}
			}

		// the following loop of code will only work if the
		// root classes are visible

		else if (!isEnum && !theClass->HasParents() && theClass->HasChildren() &&
				 !theClass->IsVisible())
			{
			rebuildVisible = kJTrue;
			theClass->SetVisible(kJTrue);
			}
		}

	// hide classes whose parents are hidden or collapsed

	JBoolean changed;
	do
		{
		changed = kJFalse;
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* theClass = itsClassesByFull->NthElement(i);
			CBClass* parent;
			if (theClass->GetParent(1, &parent))
				{
				if (parent->IsCollapsed() && theClass->IsVisible())
					{
					rebuildVisible = changed = kJTrue;
					theClass->SetVisible(kJFalse);
					}
				else if (!parent->IsCollapsed() &&
						 parent->IsVisible() != theClass->IsVisible())
					{
					rebuildVisible = changed = kJTrue;
					theClass->SetVisible(parent->IsVisible());
					}
				}
			}
		}
		while (changed);

	// rebuild the list of visible classes

	if (rebuildVisible)
		{
		itsVisibleByName->RemoveAll();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* theClass = itsClassesByFull->NthElement(i);
			if (theClass->IsVisible())
				{
				itsVisibleByName->InsertSorted(theClass);
				}
			}
		}

	// Now that we know which classes are visible, we can optimize the arrangement
	// to minimize the lengths of multiple inheritance links.

	if (rebuildVisible || itsVisibleByGeom->IsEmpty())
		{
		itsVisibleByGeom->CopyPointers(*itsVisibleByName,
									   itsVisibleByGeom->GetCleanUpAction(), kJFalse);
		MinimizeMILinks();
		}

	// place classes in the shape of a tree

	if (rebuildVisible || forcePlaceAll)
		{
		PlaceAll(NULL);
		}
}

/******************************************************************************
 PlaceAll (protected)

	Calculates the placement of all classes, assuming all parents have
	been found.  Normally only called by RecalcVisible().

	rootGeom can be NULL.  It is only needed by MinimizeMILinks().
	It contains information about root classes that have children
	and is sorted by CBClass*.

 ******************************************************************************/

void
CBTree::PlaceAll
	(
	JArray<RootGeom>* rootGeom
	)
{
	if (CBInUpdateThread())
		{
		return;
		}

	itsWidth  = 0;
	itsHeight = itsMarginWidth;

	if (rootGeom != NULL)
		{
		rootGeom->RemoveAll();
		rootGeom->SetCompareFunction(CompareRGClassPtrs);
		}

	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsVisibleByGeom->NthElement(i);
		if (!theClass->HasParents())
			{
			const JCoordinate prevH = itsHeight;
			PlaceClass(theClass, itsMarginWidth, &itsHeight, &itsWidth);
			if (rootGeom != NULL && theClass->HasChildren())
				{
				rootGeom->InsertSorted(RootGeom(theClass, prevH, itsHeight - prevH));
				}
			}
		}

	itsWidth  += itsMarginWidth;
	itsHeight += itsMarginWidth;

	Broadcast(BoundsChanged());
}

/******************************************************************************
 PlaceClass (private, recursive)

	Build the class tree whose root is the given class.
	Updates y to include the height of the new subtree.

 ******************************************************************************/

void
CBTree::PlaceClass
	(
	CBClass*			theClass,
	const JCoordinate	x,
	JCoordinate*		y,
	JCoordinate*		maxWidth
	)
{
	const JCoordinate origY = *y;
	const JCoordinate w     = theClass->GetTotalWidth();

	JBoolean foundDescendants = kJFalse;
	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* aClass = itsVisibleByGeom->NthElement(i);
		CBClass* firstParent;
		if (aClass->GetParent(1, &firstParent) && theClass == firstParent)
			{
			foundDescendants = kJTrue;
			PlaceClass(aClass, x+w, y, maxWidth);
			}
		}

	const JCoordinate h = theClass->GetTotalHeight();
	if (foundDescendants)
		{
		// By quantizing the y-coordinate, we simplify pagination.

		JCoordinate newY = (*y+origY - h)/2;
		newY -= (newY - origY) % h;
		theClass->SetCoords(x, newY);
		}
	else
		{
		theClass->SetCoords(x, *y);
		*y += h;

		if (x+w > *maxWidth)
			{
			*maxWidth = x+w;
			}
		}
}

/******************************************************************************
 ForceMinimizeMILinks

 ******************************************************************************/

void
CBTree::ForceMinimizeMILinks()
{
	if (itsNeedsMinimizeMILinksFlag)
		{
		const JBoolean saveFlag = itsMinimizeMILinksFlag;
		itsMinimizeMILinksFlag  = kJTrue;
		RecalcVisible(kJTrue);
		itsMinimizeMILinksFlag  = saveFlag;
		}
}

/******************************************************************************
 MinimizeMILinks (private)

	Rearrange each connected subset of trees to minimize the lengths
	of the connecting MI links.

 ******************************************************************************/

void
CBTree::MinimizeMILinks()
{
JIndex i,j;

	if (CBInUpdateThread())
		{
		return;
		}

	const JSize classCount = itsVisibleByGeom->GetElementCount();
	if (classCount == 0)
		{
		itsNeedsMinimizeMILinksFlag = kJFalse;
		return;
		}
	else if (!itsMinimizeMILinksFlag)
		{
		itsNeedsMinimizeMILinksFlag = kJTrue;
		return;
		}

	itsNeedsMinimizeMILinksFlag = kJFalse;

	JArray<RootGeom> rootGeom;
	PlaceAll(&rootGeom);

	// optimize each disjoint subset of trees connected by MI

	JArray<JBoolean> marked(classCount);
	for (i=1; i<=classCount; i++)
		{
		marked.AppendElement(kJFalse);
		}

	JPtrArray<CBClass> newByGeom(*itsVisibleByGeom, JPtrArrayT::kForgetAll);

	JLatentPG pg(1000);
	pg.VariableLengthProcessBeginning("Minimizing multiple inheritance lengths...", kJTrue, kJFalse);
	pg.DisplayBusyCursor();

	for (i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsVisibleByGeom->NthElement(i);
		if (theClass->GetParentCount() > 1 && !marked.GetElement(i))
			{
			// find the roots that this class connects

			JArray<RootMIInfo> rootList;
			FindRoots(theClass, rootGeom, &rootList);
			marked.SetElement(i, kJTrue);

			// find all the other roots in this connected subset of trees
			// (must check GetElementCount() each time since list grows)

			for (j=1; j <= rootList.GetElementCount(); j++)
				{
				const RootMIInfo rootInfo = rootList.GetElement(j);
				FindMIClasses(rootInfo.root, &marked, rootGeom, &rootList);
				}

			// find the best ordering of the roots
			// (rootCount could be 1 if MI is within a single tree)

			const JSize rootCount = rootList.GetElementCount();
			if (rootCount > 1)
				{
//				std::cout << "# of roots: " << rootCount << std::endl;
				JArray<JIndex> rootOrder(rootCount);
				if (!ArrangeRoots(rootList, &rootOrder, pg))
					{
					for (j=1; j<=rootCount; j++)
						{
						jdelete (rootList.GetElement(j)).connList;
						}
					itsNeedsMinimizeMILinksFlag = kJTrue;
					break;
					}

				// rearrange the classes so PlaceAll() will place the trees
				// next to each other

				for (j=1; j<=rootCount; j++)
					{
					RootMIInfo info = rootList.GetElement(rootOrder.GetElement(j));
					newByGeom.Remove(info.root);
					newByGeom.InsertAtIndex(j, info.root);
					}
				}

			// clean up

			for (j=1; j<=rootCount; j++)
				{
				jdelete (rootList.GetElement(j)).connList;
				}
			}
		}

	pg.ProcessFinished();

	itsVisibleByGeom->CopyPointers(newByGeom, itsVisibleByGeom->GetCleanUpAction(), kJFalse);
}

/******************************************************************************
 ArrangeRoots (private)

	Find the ordering of the given root classes that minimizes the length of
	the MI connections.

	Returns kJFalse if the user cancels.

 ******************************************************************************/

JBoolean
CBTree::ArrangeRoots
	(
	const JArray<RootMIInfo>&	rootList,
	JArray<JIndex>*				rootOrder,
	JProgressDisplay&			pg
	)
	const
{
	const JSize rootCount = rootList.GetElementCount();

	JArray<RootSubset> l1(100), l2(100);
	JArray<RootSubset> *list1 = &l1, *list2 = &l2;

	l1.SetCompareFunction(CompareRSContent);
	l2.SetCompareFunction(CompareRSContent);

	JArray<JBoolean>* content = jnew JArray<JBoolean>(rootCount);
	assert( content != NULL );
	for (JIndex i=1; i<=rootCount; i++)
		{
		content->AppendElement(kJFalse);
		}

	JArray<JIndex>* order = jnew JArray<JIndex>(rootCount);
	assert( order != NULL );

	list1->AppendElement(RootSubset(content, order, 0));

	do
		{
		const JSize subsetCount = list1->GetElementCount();
		for (JIndex subsetIndex=1; subsetIndex<=subsetCount; subsetIndex++)
			{
			const RootSubset* subset = list1->GetCArray() + subsetIndex-1;
			const JSize subsetSize   = (subset->order)->GetElementCount();
			for (JIndex newRootIndex=1; newRootIndex<=rootCount; newRootIndex++)
				{
				if (!(subset->content)->GetElement(newRootIndex))
					{
					// try adding this root to the end of the subset

					JIndex i;
					JSize newLinkLength = subset->linkLength;

					// add length of links from new root

					const RootMIInfo* rootPtr   = rootList.GetCArray();
					const RootMIInfo* rootInfo  = rootPtr + newRootIndex-1;
					const JCoordinate newHeight = rootInfo->h;
					const JSize linkCount       = (rootInfo->connList)->GetElementCount();
					const RootConn* connInfo    = (rootInfo->connList)->GetCArray();
					for (i=0; i<linkCount; i++)
						{
						if ((subset->content)->GetElement(connInfo[i].otherRoot))
							{
							newLinkLength += connInfo[i].dy;
							}
						else
							{
							newLinkLength += newHeight - connInfo[i].dy;
							}
						}

					// add length of links continuing past new root

					for (i=1; i<=subsetSize; i++)
						{
						const JIndex j = (subset->order)->GetElement(i);

						const RootMIInfo* rootInfo = rootPtr + j-1;
						const JSize linkCount      = (rootInfo->connList)->GetElementCount();
						const RootConn* connInfo   = (rootInfo->connList)->GetCArray();
						for (JIndex k=0; k<linkCount; k++)
							{
							const JIndex otherRoot = connInfo[k].otherRoot;
							if (otherRoot != newRootIndex &&
								!(subset->content)->GetElement(otherRoot))
								{
								newLinkLength += newHeight;
								}
							}
						}

					// if this hasn't been tried or it beats the previous record, save it

					(subset->content)->SetElement(newRootIndex, kJTrue);

					JBoolean found;
					i = list2->SearchSorted1(*subset, JOrderedSetT::kAnyMatch, &found);
					if (found && newLinkLength < (list2->GetCArray())[i-1].linkLength)
						{
						RootSubset foundSubset = list2->GetElement(i);
						*(foundSubset.content) = *(subset->content);
						*(foundSubset.order)   = *(subset->order);
						(foundSubset.order)->AppendElement(newRootIndex);
						foundSubset.linkLength = newLinkLength;
						list2->SetElement(i, foundSubset);
						}
					else if (!found)
						{
						JArray<JBoolean>* newContent = jnew JArray<JBoolean>(*(subset->content));
						assert( newContent != NULL );

						JArray<JIndex>* newOrder = jnew JArray<JIndex>(*(subset->order));
						assert( newOrder != NULL );
						newOrder->AppendElement(newRootIndex);

						list2->InsertElementAtIndex(i, RootSubset(newContent, newOrder, newLinkLength));
						}

					(subset->content)->SetElement(newRootIndex, kJFalse);
					}
				}

			if (!pg.IncrementProgress())
				{
				CleanList(list1);
				CleanList(list2);
				return kJFalse;
				}
			}

		// swap and repeat

		CleanList(list1);

		JArray<RootSubset>* tempList = list1;
		list1 = list2;
		list2 = tempList;
		}
		while (list1->GetElementCount() > 1);

	// return the result

	RootSubset completeSet = list1->GetFirstElement();
	*rootOrder = *(completeSet.order);
	jdelete completeSet.content;
	jdelete completeSet.order;

	return kJTrue;
}

/******************************************************************************
 CleanList (private)

	Deletes all objects in the list and then empties it.

 ******************************************************************************/

void
CBTree::CleanList
	(
	JArray<RootSubset>* list
	)
	const
{
	const JSize count = list->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		RootSubset subset = list->GetElement(i);
		jdelete subset.content;
		jdelete subset.order;
		}

	list->RemoveAll();
}

/******************************************************************************
 FindMIClasses (private)

	For each primary child of the given class, check if it connects
	trees via MI and then recurse on it.

 ******************************************************************************/

void
CBTree::FindMIClasses
	(
	CBClass*				theClass,
	JArray<JBoolean>*		marked,
	const JArray<RootGeom>&	rootGeom,
	JArray<RootMIInfo>*		rootList
	)
	const
{
	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c              = itsVisibleByGeom->NthElement(i);
		const JSize parentCount = c->GetParentCount();
		for (JIndex parentIndex=1; parentIndex<=parentCount; parentIndex++)
			{
			CBClass* p;
			const JBoolean ok = c->GetParent(parentIndex, &p);
			assert( ok );

			if (p == theClass && !marked->GetElement(i))
				{
				FindRoots(c, rootGeom, rootList);
				marked->SetElement(i, kJTrue);
				}
			if (parentIndex == 1 && p == theClass && c->HasChildren())
				{
				FindMIClasses(c, marked, rootGeom, rootList);
				}
			}
		}
}

/******************************************************************************
 FindRoots (private)

	If the given class connects two or more trees via MI, store information
	about each connection.

 ******************************************************************************/

void
CBTree::FindRoots
	(
	CBClass*				theClass,
	const JArray<RootGeom>&	rootGeom,
	JArray<RootMIInfo>*		rootList
	)
	const
{
	const JSize parentCount = theClass->GetParentCount();
	if (parentCount < 2)
		{
		return;
		}

	JIndex primaryRootIndex   = 0;
	JCoordinate primaryRootDy = 0;
	for (JIndex parentIndex=1; parentIndex<=parentCount; parentIndex++)
		{
		// find primary root for this parent

		CBClass* parent;
		JBoolean ok = theClass->GetParent(parentIndex, &parent);
		assert( ok );

		CBClass* root = parent;
		while (root->HasParents())
			{
			ok = root->GetParent(1, &root);
			assert( ok );
			};

		// find the geometry information

		JIndex geomIndex;
		ok = rootGeom.SearchSorted(RootGeom(root,0,0), JOrderedSetT::kAnyMatch, &geomIndex);
		assert( ok );
		const RootGeom geom = rootGeom.GetElement(geomIndex);

		// if root is not already in master list, append it

		JIndex rootIndex;
		if (!FindRoot(root, *rootList, &rootIndex))
			{
			const RootMIInfo info(root, geom.h, jnew JArray<RootConn>);
			assert( info.connList != NULL );
			rootList->AppendElement(info);
			rootIndex = rootList->GetElementCount();
			}

		if (parentIndex == 1)
			{
			primaryRootIndex = rootIndex;

			JCoordinate x,y;
			theClass->GetCoords(&x, &y);
			primaryRootDy = y - geom.top;
			}
		else
			{
			// add information about connection between trees

			RootMIInfo info = rootList->GetElement(primaryRootIndex);
			(info.connList)->AppendElement(RootConn(primaryRootDy, rootIndex));

			JCoordinate x,y;
			parent->GetCoords(&x, &y);
			info = rootList->GetElement(rootIndex);
			(info.connList)->AppendElement(RootConn(y - geom.top, primaryRootIndex));
			}
		}
}

/******************************************************************************
 FindRoot (private)

	Returns kJTrue if rootList contains the given root.  We have to do
	a linear search because roots are appended to the list as they are
	encountered to allow the list to grow as the overall search for roots
	proceeds. (see MinimizeMILinks())  This is also required because the
	indicies stored in RootConn index into rootList.

 ******************************************************************************/

JBoolean
CBTree::FindRoot
	(
	CBClass*					root,
	const JArray<RootMIInfo>&	rootList,
	JIndex*						index
	)
	const
{
	const JSize count = rootList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (root == (rootList.GetElement(i)).root)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 FindClass

	Returns kJTrue if a class with the given full name exists.

 ******************************************************************************/

JBoolean
CBTree::FindClass
	(
	const JCharacter*	fullName,
	const CBClass**		theClass
	)
	const
{
	return FindClass(fullName, const_cast<CBClass**>(theClass));
}

JBoolean
CBTree::FindClass
	(
	const JCharacter*	fullName,
	CBClass**			theClass
	)
	const
{
	JBoolean found = kJFalse;
	JIndex index   = 0;
	if (!IsEmpty())
		{
		CBClass target(fullName);
		found = itsClassesByFull->SearchSorted(&target, JOrderedSetT::kFirstMatch, &index);
		}

	if (found)
		{
		*theClass = itsClassesByFull->NthElement(index);
		return kJTrue;
		}
	else
		{
		*theClass = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 IsUniqueClassName

	Returns kJTrue if there exists a single class with the given name,
	not full name.

 ******************************************************************************/

JBoolean
CBTree::IsUniqueClassName
	(
	const JCharacter*	name,
	const CBClass**		theClass
	)
	const
{
	*theClass = NULL;

	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsClassesByFull->NthElement(i);
		if (c->GetName() == name)
			{
			if (*theClass == NULL)
				{
				*theClass = c;
				}
			else
				{
				*theClass = NULL;
				return kJFalse;
				}
			}
		}

	return JI2B( *theClass != NULL );
}

/******************************************************************************
 ClosestVisibleMatch

	Returns the closest visible match for the given name prefix.

 ******************************************************************************/

JBoolean
CBTree::ClosestVisibleMatch
	(
	const JCharacter*	prefixStr,
	CBClass**			theClass
	)
	const
{
	if (IsEmpty())
		{
		*theClass = NULL;
		return kJFalse;
		}

	CBClass target(prefixStr);
	JBoolean found;
	JIndex index =
		itsVisibleByName->SearchSorted1(&target, JOrderedSetT::kFirstMatch, &found);
	if (index > itsVisibleByName->GetElementCount())		// insert beyond end of list
		{
		index = itsVisibleByName->GetElementCount();
		}
	*theClass = itsVisibleByName->NthElement(index);
	return kJTrue;
}

/******************************************************************************
 FindParent

	Does "container" or any of its parents contain a class called "parentName"?

	Returns kJTrue if a class "parentName" exists and the namespace is a class
	that is an ancestor of the class "container".

	"parent" returns the class "parentName".
	"nameSpace" returns the namespace of "parentName".

	Called by CBClass::FindParent().

 ******************************************************************************/

JBoolean
CBTree::FindParent
	(
	const JCharacter*	parentName,
	const CBClass*		container,
	CBClass**			parent,
	JString*			nameSpace
	)
	const
{
	const JCharacter* namespaceOp = container->GetNamespaceOperator();

	JString parentSuffix = namespaceOp;
	parentSuffix += parentName;

	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c           = itsClassesByFull->NthElement(i);
		const JString& cName = c->GetFullName();
		if (cName.EndsWith(parentSuffix) &&
			parentSuffix.GetLength() < cName.GetLength())
			{
			JString prefixClassName =
				cName.GetSubstring(1, cName.GetLength() - parentSuffix.GetLength());
			const CBClass* prefixClass = NULL;
			if (FindClass(prefixClassName, &prefixClass) &&
				(prefixClass == container ||
				 prefixClass->IsAncestor(container)))
				{
				*parent    = c;
				*nameSpace = prefixClassName + namespaceOp;
				return kJTrue;
				}
			}
		}

	*parent = NULL;
	nameSpace->Clear();
	return kJFalse;
}

/******************************************************************************
 IndexToClassAfterRead (private)

	Called by CBClass when we call CBClass::FindParentsAfterRead().

	Not inline so it is next to ClassToIndexForWrite().  Both functions
	must use the same list of classes.

 ******************************************************************************/

CBClass*
CBTree::IndexToClassAfterRead
	(
	const JIndex index
	)
	const
{
	return itsClassesByFull->NthElement(index);
}

/******************************************************************************
 ClassToIndexForWrite (private)

	Called by CBClass when we call CBClass::StreamOut().

 ******************************************************************************/

JIndex
CBTree::ClassToIndexForWrite
	(
	const CBClass* theClass
	)
	const
{
	JIndex index;
	const JBoolean found = itsClassesByFull->Find(theClass, &index);
	assert( found );
	return index;
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

JBoolean
CBTree::HasSelection()
	const
{
	const JSize classCount = itsVisibleByName->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		if ((itsVisibleByName->NthElement(i))->IsSelected())
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetSelectedClasses

	Do *not* call DeleteAll() on the classList that is returned!

 ******************************************************************************/

JBoolean
CBTree::GetSelectedClasses
	(
	JPtrArray<CBClass>* classList
	)
	const
{
	classList->RemoveAll();
	classList->SetCleanUpAction(JPtrArrayT::kForgetAll);

	const JSize classCount = itsVisibleByName->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsVisibleByName->NthElement(i);
		if (c->IsSelected())
			{
			classList->Append(c);
			}
		}

	return !classList->IsEmpty();
}

/******************************************************************************
 DeselectAll

 ******************************************************************************/

void
CBTree::DeselectAll()
{
	itsBroadcastClassSelFlag = kJFalse;

	const JSize count = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsClassesByFull->NthElement(i))->SetSelected(kJFalse);
		}

	itsBroadcastClassSelFlag = kJTrue;

	Broadcast(AllClassesDeselected());
}

/******************************************************************************
 GetSelectionCoverage

	Returns kJTrue if there are any selected classes, and then sets *coverage
	to the coverage of these classes, and *selCount to the number of classes.

 ******************************************************************************/

JBoolean
CBTree::GetSelectionCoverage
	(
	JRect*	coverage,
	JSize*	selCount
	)
	const
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		*coverage = (classList.FirstElement())->GetFrame();

		const JSize count = classList.GetElementCount();
		for (JIndex i=2; i<=count; i++)
			{
			*coverage = JCovering(*coverage, (classList.NthElement(i))->GetFrame());
			}

		*selCount = classList.GetElementCount();
		return kJTrue;
		}
	else
		{
		coverage->Set(0,0,0,0);
		*selCount = 0;
		return kJFalse;
		}
}

/******************************************************************************
 SelectClasses

	Selects all the classes with the given name, not full name.

 ******************************************************************************/

void
CBTree::SelectClasses
	(
	const JCharacter*	name,
	const JBoolean		deselectAll
	)
{
	if (deselectAll)
		{
		DeselectAll();
		}

	JBoolean changed       = kJFalse;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsClassesByFull->NthElement(i);
		if (theClass->GetName() == name)
			{
			theClass->SetSelected(kJTrue);
			if (!theClass->IsVisible())
				{
				changed = kJTrue;
				ForceVisible(theClass);
				}
			}
		}

	if (changed)
		{
		RecalcVisible(kJTrue);		// ForceVisible() can uncollapse -or- simply show
		Broadcast(Changed());
		}
}

/******************************************************************************
 SelectImplementors

	Selects all the classes that implement the given function.

 ******************************************************************************/

void
CBTree::SelectImplementors
	(
	const JCharacter*	fnName,
	const JBoolean		caseSensitive,
	const JBoolean		deselectAll
	)
{
	if (deselectAll)
		{
		DeselectAll();
		}

	JBoolean changed       = kJFalse;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsClassesByFull->NthElement(i);
		if (theClass->Implements(fnName, caseSensitive))
			{
			theClass->SetSelected(kJTrue);
			if (!theClass->IsVisible())
				{
				changed = kJTrue;
				ForceVisible(theClass);
				}
			}
		}

	if (changed)
		{
		RecalcVisible(kJTrue);		// ForceVisible() can uncollapse -or- simply show
		Broadcast(Changed());
		}
}

/******************************************************************************
 SelectParents

	Selects the parents of the selected classes.

 ******************************************************************************/

void
CBTree::SelectParents()
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		JBoolean changed       = kJFalse;
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.NthElement(i);
			const JSize parentCount = c->GetParentCount();
			for (JIndex j=1; j<=parentCount; j++)
				{
				CBClass* p;
				const JBoolean ok = c->GetParent(j, &p);
				assert( ok );
				p->SetSelected(kJTrue);
				if (!p->IsVisible())
					{
					changed = kJTrue;
					ForceVisible(p);
					}
				}
			}

		if (changed)
			{
			RecalcVisible(kJTrue);		// ForceVisible() can uncollapse -or- simply show
			Broadcast(Changed());
			}
		}
}

/******************************************************************************
 SelectDescendants

	Selects all descendants of the selected classes.

 ******************************************************************************/

void
CBTree::SelectDescendants()
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		JBoolean changed       = kJFalse;
		const JSize classCount = classList.GetElementCount();
		const JSize totalCount = itsClassesByFull->GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.NthElement(i);
			for (JIndex j=1; j<=totalCount; j++)
				{
				CBClass* c1 = itsClassesByFull->NthElement(j);
				if (c->IsAncestor(c1))
					{
					c1->SetSelected(kJTrue);
					if (!c1->IsVisible())
						{
						changed = kJTrue;
						ForceVisible(c1);
						}
					}
				}
			}

		if (changed)
			{
			RecalcVisible(kJTrue);		// ForceVisible() can uncollapse -or- simply show
			Broadcast(Changed());
			}
		}
}

/******************************************************************************
 ViewSelectedSources

	Opens the source file for each selected class.

 ******************************************************************************/

void
CBTree::ViewSelectedSources()
	const
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			(classList.NthElement(i))->ViewSource();
			}
		}
}

/******************************************************************************
 ViewSelectedHeaders

	Opens the header file for each selected class.

 ******************************************************************************/

void
CBTree::ViewSelectedHeaders()
	const
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			(classList.NthElement(i))->ViewHeader();
			}
		}
}

/******************************************************************************
 ViewSelectedFunctionLists

	Opens a function list window for each selected class.

 ******************************************************************************/

void
CBTree::ViewSelectedFunctionLists()
	const
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			itsDirector->ViewFunctionList(classList.NthElement(i));
			}
		}
}

/******************************************************************************
 CopySelectedClassNames

	Copies the names of the selected classes to the text clipboard.

 ******************************************************************************/

void
CBTree::CopySelectedClassNames()
	const
{
	JPtrArray<JString> nameList(JPtrArrayT::kForgetAll);

	const JPtrArray<CBClass>& classList = GetVisibleClasses();
	const JSize classCount              = classList.GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		const CBClass* theClass = classList.NthElement(i);
		if (theClass->IsSelected())
			{
			const JString& name = theClass->GetFullName();
			nameList.Append(const_cast<JString*>(&name));
			}
		}

	if (!nameList.IsEmpty())
		{
		JXTextSelection* data = jnew JXTextSelection(itsDirector->GetDisplay(), nameList);
		assert( data != NULL );

		((itsDirector->GetDisplay())->GetSelectionManager())->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 DeriveFromSelected

 ******************************************************************************/

void
CBTree::DeriveFromSelected()
	const
{
	JPtrArray<CBClass> parentList(JPtrArrayT::kForgetAll);
	if (!GetSelectedClasses(&parentList))
		{
		return;
		}

	if (!JProgramAvailable(kCodeMillProgramName))
		{
		(JGetUserNotification())->DisplayMessage(JGetString(kRequiresCodeMillID));
		return;
		}

	// build data files describing ancestor classes

	JString outputPath;
	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JBoolean success = kJTrue;

	JPtrArray<CBClass> ancestorList(JPtrArrayT::kForgetAll);

	const JSize parentCount = parentList.GetElementCount();
	JString dataFileName, srcFileName;
	for (JIndex i=1; i<=parentCount; i++)
		{
		CBClass* parent = parentList.NthElement(i);

		ancestorList.RemoveAll();
		CollectAncestors(parent, &ancestorList);
		ancestorList.Prepend(parent);		// first element must be base class

		const JError err = JCreateTempFile(&dataFileName);
		if (!err.OK())
			{
			err.ReportIfError();
			success = kJFalse;
			break;
			}
		argList.Append(dataFileName);

		std::ofstream output(dataFileName);
		output << kCodeMillDataVersion << std::endl;

		const JSize ancestorCount = ancestorList.GetElementCount();
		output << ancestorCount << std::endl;

		for (JIndex j=1; j<=ancestorCount; j++)
			{
			CBClass* ancestor = ancestorList.NthElement(j);
			if (ancestor->GetFileName(&srcFileName))
				{
				output << ancestor->GetFullName() << '\t' << srcFileName << std::endl;

				if (outputPath.IsEmpty())
					{
					JString name;
					JSplitPathAndName(srcFileName, &outputPath, &name);
					}
				}
			}

		if (output.fail())
			{
			success = kJFalse;
			break;
			}
		}

	// invoke Code Mill

	const JSize fileCount = argList.GetElementCount();
	if (success && fileCount > 0)
		{
		argList.Prepend(JPrepArgForExec(outputPath));
		argList.Prepend(kCodeMillOutputPathOption);
		argList.Prepend(kCodeMillDeleteInputFilesOption);
		argList.Prepend(kCodeMillProgramName);
		JSimpleProcess::Create(argList, kJTrue);
		}
	else if (fileCount > 0)
		{
		for (JIndex i=1; i<=fileCount; i++)
			{
			remove(*(argList.NthElement(i)));
			}
		}
}

/******************************************************************************
 CollectAncestors (private)

 ******************************************************************************/

void
CBTree::CollectAncestors
	(
	CBClass* 			cbClass,
	JPtrArray<CBClass>*	list
	)
	const
{
	const JSize parentCount = cbClass->GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
		{
		CBClass* parent;
		if (cbClass->GetParent(i, &parent))
			{
			JBoolean found = kJFalse;

			const JSize count = list->GetElementCount();
			for (JIndex j=1; j<=count; j++)
				{
				if (CompareClassFullNames(parent, list->NthElement(j)) ==
					JOrderedSetT::kFirstEqualSecond)
					{
					found = kJTrue;
					}
				}

			if (!found)
				{
				list->Append(parent);
				CollectAncestors(parent, list);
				}
			}
		}
}

/******************************************************************************
 ForceVisible (private)

 ******************************************************************************/

void
CBTree::ForceVisible
	(
	CBClass* theClass
	)
{
	const CBClass::ForceVisibleAction action = theClass->ForceVisible();
	if (action == CBClass::kShowLoneClasses)
		{
		itsShowLoneClassesFlag = kJTrue;
		Broadcast(Changed());
		}
	else if (action == CBClass::kShowLoneStructs)
		{
		itsShowLoneStructsFlag = kJTrue;
		Broadcast(Changed());
		}
	else if (action == CBClass::kShowEnums)
		{
		itsShowEnumsFlag = kJTrue;
		Broadcast(Changed());
		}
	else
		{
		assert( action == CBClass::kDoNothing );
		}
}

/******************************************************************************
 ShowLoneClasses

	Set whether or not lone classes should be visible.

 ******************************************************************************/

void
CBTree::ShowLoneClasses
	(
	const JBoolean visible
	)
{
	if (visible != itsShowLoneClassesFlag)
		{
		itsShowLoneClassesFlag = visible;
		RecalcVisible();
		Broadcast(Changed());
		}
}

/******************************************************************************
 ShowLoneStructs

	Set whether or not lone classes should be visible.

 ******************************************************************************/

void
CBTree::ShowLoneStructs
	(
	const JBoolean visible
	)
{
	if (visible != itsShowLoneStructsFlag)
		{
		itsShowLoneStructsFlag = visible;
		RecalcVisible();
		Broadcast(Changed());
		}
}

/******************************************************************************
 ShowEnums

	Set whether or not enums should be visible.

 ******************************************************************************/

void
CBTree::ShowEnums
	(
	const JBoolean visible
	)
{
	if (visible != itsShowEnumsFlag)
		{
		itsShowEnumsFlag = visible;
		RecalcVisible();
		Broadcast(Changed());
		}
}

/******************************************************************************
 GetMenuInfo

 ******************************************************************************/

void
CBTree::GetMenuInfo
	(
	JBoolean* hasSelection,
	JBoolean* canCollapse,
	JBoolean* canExpand
	)
	const
{
	*hasSelection = kJFalse;
	*canCollapse  = kJFalse;
	*canExpand    = kJFalse;

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		*hasSelection = kJTrue;

		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.NthElement(i);
			if (c->IsCollapsed())
				{
				*canExpand = kJTrue;
				}
			else if (c->HasPrimaryChildren())
				{
				*canCollapse = kJTrue;
				}
			}
		}
}

/******************************************************************************
 CollapseExpandSelectedClasses

 ******************************************************************************/

void
CBTree::CollapseExpandSelectedClasses
	(
	const JBoolean collapse
	)
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		JBoolean changed       = kJFalse;
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.NthElement(i);
			if ((!collapse || c->HasPrimaryChildren()) &&
				c->IsCollapsed() != collapse)
				{
				changed = kJTrue;
				c->SetCollapsed(collapse);
				}
			}

		if (changed)
			{
			RecalcVisible();
			Broadcast(Changed());
			}
		}
}

/******************************************************************************
 ExpandAllClasses

 ******************************************************************************/

void
CBTree::ExpandAllClasses()
{
	JBoolean changed       = kJFalse;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsClassesByFull->NthElement(i);
		if (c->IsCollapsed())
			{
			changed = kJTrue;
			c->SetCollapsed(kJFalse);
			}
		}

	if (changed)
		{
		RecalcVisible();
		Broadcast(Changed());
		}
}

/******************************************************************************
 GetClass

	Return the class that contains the given point.

 ******************************************************************************/

JBoolean
CBTree::GetClass
	(
	const JPoint&	pt,
	CBClass**		theClass
	)
	const
{
	const JSize classCount = itsVisibleByName->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsVisibleByName->NthElement(i);
		if (c->Contains(pt))
			{
			*theClass = c;
			return kJTrue;
			}
		}

	*theClass = NULL;
	return kJFalse;
}

/******************************************************************************
 HitSameClass

	Return the class that contains both points.

 ******************************************************************************/

JBoolean
CBTree::HitSameClass
	(
	const JPoint&	pt1,
	const JPoint&	pt2,
	CBClass**		theClass
	)
	const
{
	const JSize classCount = itsVisibleByName->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c        = itsVisibleByName->NthElement(i);
		const JBoolean c1 = c->Contains(pt1);
		const JBoolean c2 = c->Contains(pt2);
		if (c1 && c2)
			{
			*theClass = c;
			return kJTrue;
			}
		else if (c1 || c2)	// quit now if it contains only one pt
			{
			*theClass = NULL;
			return kJFalse;
			}
		}

	*theClass = NULL;
	return kJFalse;
}

/******************************************************************************
 BroadcastSelectionChange (private)

	Called by CBClass.

 ******************************************************************************/

void
CBTree::BroadcastSelectionChange
	(
	CBClass*		theClass,
	const JBoolean	isSelected
	)
{
	if (itsBroadcastClassSelFlag && isSelected)
		{
		Broadcast(ClassSelected(theClass));
		}
	else if (itsBroadcastClassSelFlag)
		{
		Broadcast(ClassDeselected(theClass));
		}
}

/******************************************************************************
 Draw

 ******************************************************************************/

void
CBTree::Draw
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
JIndex i;

	p.SetFontSize(itsFontSize);
	const JSize classCount = itsVisibleByName->GetElementCount();

	if (!itsDrawMILinksOnTopFlag)
		{
		for (i=1; i<=classCount; i++)
			{
			(itsVisibleByName->NthElement(i))->DrawMILinks(p, rect);
			}
		}

	for (i=1; i<=classCount; i++)
		{
		(itsVisibleByName->NthElement(i))->Draw(p, rect);
		}

	if (itsDrawMILinksOnTopFlag)
		{
		for (i=1; i<=classCount; i++)
			{
			(itsVisibleByName->NthElement(i))->DrawMILinks(p, rect);
			}
		}

	for (i=1; i<=classCount; i++)
		{
		(itsVisibleByName->NthElement(i))->DrawText(p, rect);
		}
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

CBProjectDocument*
CBTree::GetProjectDoc()
	const
{
	return itsDirector->GetProjectDoc();
}

/******************************************************************************
 GetFontManager

	We don't assert in the constructor because CBTextTree doesn't provide one.

 ******************************************************************************/

const JFontManager*
CBTree::GetFontManager()
	const
{
	return (itsDirector->GetDisplay())->GetFontManager();
}

/******************************************************************************
 SetFontSize

 ******************************************************************************/

void
CBTree::SetFontSize
	(
	const JSize size
	)
{
	if (size != itsFontSize)
		{
		const JSize origSize = itsFontSize;
		const JFloat h1      = CBClass::GetTotalHeight(this, GetFontManager());

		itsFontSize = size;

		const JSize count = itsClassesByFull->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsClassesByFull->NthElement(i))->CalcFrame();
			}

		const JFloat h2 = CBClass::GetTotalHeight(this, GetFontManager());
		Broadcast(FontSizeChanged(origSize, size, h2/h1));	// scale scrolltabs first
		PlaceAll();
		}
}

/******************************************************************************
 GetLineHeight

	Get the quantization step size for the vertical placement of classes.

 ******************************************************************************/

JCoordinate
CBTree::GetLineHeight()
	const
{
	return (itsClassesByFull->FirstElement())->GetTotalHeight();
}

/******************************************************************************
 CompareClassFullNames (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
CBTree::CompareClassFullNames
	(
	CBClass* const & c1,
	CBClass* const & c2
	)
{
	return JCompareStringsCaseSensitive(
			const_cast<JString*>(&(c1->GetFullName())),
			const_cast<JString*>(&(c2->GetFullName())));
}

/******************************************************************************
 CompareClassNames (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
CBTree::CompareClassNames
	(
	CBClass* const & c1,
	CBClass* const & c2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&(c1->GetName())),
			const_cast<JString*>(&(c2->GetName())));
}

/******************************************************************************
 CompareRGClassPtrs (static private)

	We sort the actual pointers because that's all we need to look up.

 ******************************************************************************/

JOrderedSetT::CompareResult
CBTree::CompareRGClassPtrs
	(
	const RootGeom& i1,
	const RootGeom& i2
	)
{
	if (i1.root < i2.root)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (i1.root == i2.root)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 CompareRSContent (static private)

	We compare the boolean content arrays.

 ******************************************************************************/

JOrderedSetT::CompareResult
CBTree::CompareRSContent
	(
	const RootSubset& s1,
	const RootSubset& s2
	)
{
	const JBoolean* b1 = (s1.content)->GetCArray();
	const JBoolean* b2 = (s2.content)->GetCArray();

	const JSize count = (s1.content)->GetElementCount();
	for (JIndex i=1; i<=count; i++, b1++, b2++)
		{
		if (!(*b1) && *b2)
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		else if (*b1 && !(*b2))
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		}

	return JOrderedSetT::kFirstEqualSecond;
}
