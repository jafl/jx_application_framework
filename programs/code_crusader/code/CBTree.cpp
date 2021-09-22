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

	Copyright © 1995-99 John Lindal.

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
#include <JStringIterator.h>
#include <JSimpleProcess.h>
#include <jGlobals.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JSize kBlockSize = 1024;

// Code Mill info

static const JString kCodeMillProgramName("code_mill", JString::kNoCopy);
static const JString kCodeMillOptions("--delete --output_path", JString::kNoCopy);
#ifndef CODE_CRUSADER_UNIT_TEST
const JFileVersion kCodeMillDataVersion = 0;
#endif

// JBroadcaster message types

const JUtf8Byte* CBTree::kChanged              = "Changed::CBTree";
const JUtf8Byte* CBTree::kBoundsChanged        = "BoundsChanged::CBTree";
const JUtf8Byte* CBTree::kNeedsRefresh         = "NeedsRefresh::CBTree";
const JUtf8Byte* CBTree::kFontSizeChanged      = "FontSizeChanged::CBTree";

const JUtf8Byte* CBTree::kPrepareForParse      = "PrepareForParse::CBTree";
const JUtf8Byte* CBTree::kParseFinished        = "ParseFinished::CBTree";

const JUtf8Byte* CBTree::kClassSelected        = "ClassSelected::CBTree";
const JUtf8Byte* CBTree::kClassDeselected      = "ClassDeselected::CBTree";
const JUtf8Byte* CBTree::kAllClassesDeselected = "AllClassesDeselected::CBTree";

/******************************************************************************
 Constructor

	In the 2nd version, dirList can be nullptr

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

#ifndef CODE_CRUSADER_UNIT_TEST

CBTree::CBTree
	(
	std::istream&			projInput,
	const JFileVersion		projVers,
	std::istream*			setInput,
	const JFileVersion		setVers,
	std::istream*			symStream,
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

	std::istream* symInput        = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers    = (projVers <= 41 ? projVers   : origSymVers);
	const bool useSetProjData = setInput == nullptr || setVers < 71;
	const bool useSymProjData = symInput == nullptr || symVers < 71;

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
		bool showLoneClasses, showLoneStructs, showEnums;
		if (projVers >= 12)
			{
			projInput >> JBoolFromString(showLoneClasses);
			if (projVers >= 13)
				{
				projInput >> JBoolFromString(showLoneStructs);
				}
			}
		if (projVers >= 10)
			{
			projInput >> JBoolFromString(showEnums);
			}

		if (projVers >= 19)
			{
			projInput >> JBoolFromString(itsNeedsMinimizeMILinksFlag);
			}
		else
			{
			itsNeedsMinimizeMILinksFlag = true;
			}
		}
	if (!useSetProjData)	// overwrite
		{
		if (setVers < 87)
			{
			bool showLoneClasses, showLoneStructs, showEnums;
			*setInput >> JBoolFromString(showLoneClasses)
					  >> JBoolFromString(showLoneStructs)
					  >> JBoolFromString(showEnums);
			}
		*setInput >> JBoolFromString(itsNeedsMinimizeMILinksFlag);
		}

/* symbol file */

	if (symInput != nullptr)
		{
		bool classNamesSorted = true;
		if (symVers < 7)
			{
			*symInput >> JBoolFromString(classNamesSorted);
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
				itsVisibleByGeom->Append(itsVisibleByName->GetElement(j));
				}
			itsVisibleByGeom->SetBlockSize(kBlockSize);
			}

		for (i=1; i<=classCount; i++)
			{
			(itsClassesByFull->GetElement(i))->FindParentsAfterRead();
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

	if (dirList != nullptr)
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

	bool needsReparse = projVers < 49;
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
		if (!JSameStrings(*itsSuffixList, suffixList, JString::kCompareCase))
			{
			needsReparse = true;
			}
		}

	if (needsReparse)
		{
		NextUpdateMustReparseAll();
		}
}

#endif

// private

void
CBTree::CBTreeX
	(
	CBTreeDirector*		director,
	CBClassStreamInFn*	streamInFn
	)
{
	itsDirector = director;
	itsFontSize = JFontManager::GetDefaultFontSize();

	itsClassesByFull = jnew JPtrArray<CBClass>(JPtrArrayT::kDeleteAll, kBlockSize);
	assert( itsClassesByFull != nullptr );
	itsClassesByFull->SetCompareFunction(CompareClassFullNames);
	itsClassesByFull->SetSortOrder(JListT::kSortAscending);

	itsVisibleByGeom = jnew JPtrArray<CBClass>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByGeom != nullptr );

	itsVisibleByName = jnew JPtrArray<CBClass>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByName != nullptr );
	itsVisibleByName->SetCompareFunction(CompareClassNames);
	itsVisibleByName->SetSortOrder(JListT::kSortAscending);

#ifndef CODE_CRUSADER_UNIT_TEST
	itsSuffixList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSuffixList != nullptr );
	CBGetPrefsManager()->GetFileSuffixes(itsFileType, itsSuffixList);
#else
	itsSuffixList = nullptr;
#endif

	itsCollapsedList = nullptr;

	InstallCollection(itsClassesByFull);

	itsWidth = itsHeight = 0;
	itsBroadcastClassSelFlag = true;

	itsDrawMILinksOnTopFlag = true;

	itsMinimizeMILinksFlag      = false;
	itsNeedsMinimizeMILinksFlag = false;

	itsReparseAllFlag           = false;
	itsChangedDuringParseFlag   = false;
	itsBeganEmptyFlag           = false;

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
	std::istream&		input,
	const JFileVersion	vers
	)
{
JIndex i;

	itsReparseAllFlag = false;

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
		itsVisibleByGeom->Append(itsVisibleByName->GetElement(j));
		}

	itsVisibleByGeom->SetBlockSize(kBlockSize);

	// link up each class with parents

	for (i=1; i<=classCount; i++)
		{
		(itsClassesByFull->GetElement(i))->FindParentsAfterRead();
		}

	// check for change in file suffixes

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	input >> suffixList;
	if (!JSameStrings(*itsSuffixList, suffixList, JString::kCompareCase))
		{
		NextUpdateMustReparseAll();
		}

	RebuildTree();	// calls X server, so cannot be done in child process

	Broadcast(Changed());
}

/******************************************************************************
 StreamOut (virtual)

	dirList can be nullptr

 ******************************************************************************/

void
CBTree::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const CBDirList*	dirList
	)
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

JIndex i;

/* project file */

	if (dirList != nullptr)
		{
		dirList->WriteDirectories(projOutput);
		}

/* symbol file */

	if (symOutput != nullptr)
		{
		*symOutput << ' ' << itsWidth;
		*symOutput << ' ' << itsHeight;
		}

/* settings file */

	if (setOutput != nullptr)
		{
		*setOutput << ' ' << JBoolToString(itsNeedsMinimizeMILinksFlag);
		}

/* symbol file */

	if (symOutput != nullptr)
		{
		// write the number of classes in tree

		const JSize classCount = itsClassesByFull->GetElementCount();
		*symOutput << ' ' << classCount;

		// write each class

		for (i=1; i<=classCount; i++)
			{
			*symOutput << ' ';
			(itsClassesByFull->GetElement(i))->StreamOut(*symOutput);
			}

		// write geometry ordering

		const JSize geomCount = itsVisibleByGeom->GetElementCount();
		assert( geomCount == itsVisibleByName->GetElementCount() );
		for (i=1; i<=geomCount; i++)
			{
			JIndex j;
			const bool found = itsVisibleByName->Find(itsVisibleByGeom->GetElement(i), &j);
			assert( found );
			*symOutput << ' ' << j;
			}

		// save file suffixes so we can check if they change

		*symOutput << ' ' << *itsSuffixList;

		*symOutput << ' ';
		}

#endif
}

/******************************************************************************
 FileTypesChanged

	We can't use Receive() because this must be called -before-
	CBProjectDocument updates everything.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

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

#endif

/******************************************************************************
 PrepareForUpdate (virtual)

	Get ready to parse files that have changed or been created and to
	throw out classes in files that no longer exist.

	*** This often runs in the update thread.

 ******************************************************************************/

void
CBTree::PrepareForUpdate
	(
	const bool reparseAll
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	if (!CBInUpdateThread())
		{
		Broadcast(PrepareForParse());
		}

	// save collapsed classes

	itsCollapsedList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsCollapsedList != nullptr );
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

	Cleans up after updating files.  Returns true if changes were found.

	*** This often runs in the update thread.

 ******************************************************************************/

bool
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

	const bool forceRecalcVisible = RestoreCollapsedClasses(*itsCollapsedList);
	jdelete itsCollapsedList;
	itsCollapsedList = nullptr;

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

		itsReparseAllFlag = false;
		}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 FileChanged

	Returns true if the file should be parsed.

	Not private because it is called by CBFileListTable::ParseFile().

 ******************************************************************************/

void
CBTree::FileChanged
	(
	const JString&			fileName,
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
		const CBClass* theClass = itsClassesByFull->GetElement(i);
		JFAID_t fileID;
		if (theClass->GetFileID(&fileID) && fileID == id)
			{
			itsClassesByFull->DeleteElement(i);
			itsVisibleByGeom->Remove(theClass);
			itsVisibleByName->Remove(theClass);

			itsChangedDuringParseFlag = true;
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
		const CBClass* theClass = itsClassesByFull->GetElement(i);
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

bool
CBTree::RestoreCollapsedClasses
	(
	const JPtrArray<JString>& list
	)
{
	bool changed = false;

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fullName = list.GetElement(i);
		CBClass* theClass;
		if (FindClass(*fullName, &theClass) && !theClass->IsCollapsed())
			{
			theClass->SetCollapsed(true);
			changed = true;
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

	itsChangedDuringParseFlag = true;
}

/******************************************************************************
 RebuildTree (private)

	Recalculates all parents and then places all classes.

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
		CBClass* theClass = itsClassesByFull->GetElement(i);
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

	bool progress;
	do
		{
		progress = false;
		for (i=1; i<=classCount; i++)
			{
			const bool foundAnotherParent =
				itsClassesByFull->GetElement(i)->FindParents(false);
			progress = progress || foundAnotherParent;
			}
		}
		while (progress);

	// Now that no more progress is being made, allow ghosts to be created.
	// (Inserting ghosts does not cause classes to be skipped.)

	for (i=1; i<=classCount; i++)
		{
		itsClassesByFull->GetElement(i)->FindParents(true);
		classCount = GetElementCount();
		}

	// Now that all parents have been found, we can hide lone classes and
	// classes whose parents are hidden or collapsed.

	RecalcVisible(false, true);
}

/******************************************************************************
 RecalcVisible (private)

	Hides classes whose parents are hidden or collapsed.

	If necessary or requested, it rebuilds the list of visible classes
	and adjusts their placement in the tree.

 ******************************************************************************/

void
CBTree::RecalcVisible
	(
	const bool forceRebuildVisible,
	const bool forcePlaceAll
	)
{
	const JSize classCount  = itsClassesByFull->GetElementCount();
	bool rebuildVisible = forceRebuildVisible;

	// hide classes whose parents are hidden or collapsed

	bool changed;
	do
		{
		changed = false;
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* theClass = itsClassesByFull->GetElement(i);
			CBClass* parent;
			if (theClass->GetParent(1, &parent))
				{
				if (parent->IsCollapsed() && theClass->IsVisible())
					{
					rebuildVisible = changed = true;
					theClass->SetVisible(false);
					}
				else if (!parent->IsCollapsed() &&
						 parent->IsVisible() != theClass->IsVisible())
					{
					rebuildVisible = changed = true;
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
			CBClass* theClass = itsClassesByFull->GetElement(i);
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
									   itsVisibleByGeom->GetCleanUpAction(), false);

		// move class trees to top, to simplify visual searching

		const JSize classCount = itsVisibleByGeom->GetElementCount();

		JIndex min = 1;
		for (JIndex i = classCount; i >= min; i--)
			{
			CBClass* theClass = itsVisibleByGeom->GetElement(i);
			if (!theClass->HasParents() && theClass->HasChildren())
				{
				itsVisibleByGeom->MoveElementToIndex(i, 1);
				i++;		// recheck new element at this index
				min++;		// ignore newly moved element
				}
			}

		// truly minimize, if required

		MinimizeMILinks();
		}

	// place classes in the shape of a tree

	if (rebuildVisible || forcePlaceAll)
		{
		PlaceAll();
		}
}

/******************************************************************************
 PlaceAll (protected)

	Calculates the placement of all classes, assuming all parents have
	been found.  Normally only called by RecalcVisible().

	rootGeom can be nullptr.  It is only needed by MinimizeMILinks().
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

	if (rootGeom != nullptr)
		{
		rootGeom->RemoveAll();
		rootGeom->SetCompareFunction(CompareRGClassPtrs);
		}

	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsVisibleByGeom->GetElement(i);
		if (!theClass->HasParents())
			{
			const JCoordinate prevH = itsHeight;
			PlaceClass(theClass, itsMarginWidth, &itsHeight, &itsWidth);
			if (rootGeom != nullptr && theClass->HasChildren())
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

	bool foundDescendants = false;
	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* aClass = itsVisibleByGeom->GetElement(i);
		CBClass* firstParent;
		if (aClass->GetParent(1, &firstParent) && theClass == firstParent)
			{
			foundDescendants = true;
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
		const bool saveFlag = itsMinimizeMILinksFlag;
		itsMinimizeMILinksFlag  = true;
		RecalcVisible(true);
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
		itsNeedsMinimizeMILinksFlag = false;
		return;
		}
	else if (!itsMinimizeMILinksFlag)
		{
		itsNeedsMinimizeMILinksFlag = true;
		return;
		}

	itsNeedsMinimizeMILinksFlag = false;

	JArray<RootGeom> rootGeom;
	PlaceAll(&rootGeom);

	// optimize each disjoint subset of trees connected by MI

	JArray<bool> marked(classCount);
	for (i=1; i<=classCount; i++)
		{
		marked.AppendElement(false);
		}

	JPtrArray<CBClass> newByGeom(*itsVisibleByGeom, JPtrArrayT::kForgetAll);

	JLatentPG pg(1000);
	pg.VariableLengthProcessBeginning(JGetString("MinMILengthsProgress::CBTree"), true, false);
	pg.DisplayBusyCursor();

	for (i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsVisibleByGeom->GetElement(i);
		if (theClass->GetParentCount() > 1 && !marked.GetElement(i))
			{
			// find the roots that this class connects

			JArray<RootMIInfo> rootList;
			FindRoots(theClass, rootGeom, &rootList);
			marked.SetElement(i, true);

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
					itsNeedsMinimizeMILinksFlag = true;
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

	itsVisibleByGeom->CopyPointers(newByGeom, itsVisibleByGeom->GetCleanUpAction(), false);
}

/******************************************************************************
 ArrangeRoots (private)

	Find the ordering of the given root classes that minimizes the length
	of the MI connections.  The algorithm is based on the concept of
	dynamic programming to avoid factorial runtime -- but it can still be
	really slow!

	Returns false if the user cancels.

 ******************************************************************************/

bool
CBTree::ArrangeRoots
	(
	const JArray<RootMIInfo>&	rootList,
	JArray<JIndex>*				rootOrder,
	JProgressDisplay&			pg
	)
	const
{
	const JSize rootCount = rootList.GetElementCount();
	// std::cout << "# roots: " << rootCount << std::endl;

	JArray<RootSubset> l1(100), l2(100);
	JArray<RootSubset> *list1 = &l1, *list2 = &l2;

	l1.SetCompareFunction(CompareRSContent);
	l2.SetCompareFunction(CompareRSContent);

	auto* content = jnew JArray<bool>(rootCount);
	assert( content != nullptr );
	for (JIndex i=1; i<=rootCount; i++)
		{
		content->AppendElement(false);
		}

	auto* order = jnew JArray<JIndex>(rootCount);
	assert( order != nullptr );

	list1->AppendElement(RootSubset(content, order, 0));

	do
		{
		const JSize subsetCount = list1->GetElementCount();
		//std::cout << "do loop: " << subsetCount << std::endl;

		for (JIndex subsetIndex=1; subsetIndex<=subsetCount; subsetIndex++)
			{
			const RootSubset* subset = list1->GetCArray() + subsetIndex-1;
			const JSize subsetSize   = (subset->order)->GetElementCount();
			for (JIndex newRootIndex=1; newRootIndex<=rootCount; newRootIndex++)
				{
				if ((subset->content)->GetElement(newRootIndex))
					{
					continue;
					}

				// try adding this root to the end of the subset

				JSize newLinkLength = subset->linkLength;

				// add length of links from new root

				const RootMIInfo* rootPtr   = rootList.GetCArray();
				const RootMIInfo* rootInfo  = rootPtr + newRootIndex-1;
				const JCoordinate newHeight = rootInfo->h;
				const JSize linkCount       = (rootInfo->connList)->GetElementCount();
				const RootConn* connInfo    = (rootInfo->connList)->GetCArray();
				for (JUnsignedOffset i=0; i<linkCount; i++)
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

				for (JIndex i=1; i<=subsetSize; i++)
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

				subset->content->SetElement(newRootIndex, true);

				bool found;
				const JIndex i = list2->SearchSorted1(*subset, JListT::kAnyMatch, &found);
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
					auto* newContent = jnew JArray<bool>(*(subset->content));
					assert( newContent != nullptr );

					auto* newOrder = jnew JArray<JIndex>(*(subset->order));
					assert( newOrder != nullptr );
					newOrder->AppendElement(newRootIndex);

					list2->InsertElementAtIndex(i, RootSubset(newContent, newOrder, newLinkLength));
					}

				(subset->content)->SetElement(newRootIndex, false);
				}

			if (!pg.IncrementProgress())
				{
				CleanList(list1);
				CleanList(list2);
				return false;
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

	return true;
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
	JArray<bool>*		marked,
	const JArray<RootGeom>&	rootGeom,
	JArray<RootMIInfo>*		rootList
	)
	const
{
	const JSize classCount = itsVisibleByGeom->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c              = itsVisibleByGeom->GetElement(i);
		const JSize parentCount = c->GetParentCount();
		for (JIndex parentIndex=1; parentIndex<=parentCount; parentIndex++)
			{
			CBClass* p;
			const bool ok = c->GetParent(parentIndex, &p);
			assert( ok );

			if (p == theClass && !marked->GetElement(i))
				{
				FindRoots(c, rootGeom, rootList);
				marked->SetElement(i, true);
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
		bool ok = theClass->GetParent(parentIndex, &parent);
		assert( ok );

		CBClass* root = parent;
		while (root->HasParents())
			{
			ok = root->GetParent(1, &root);
			assert( ok );
			};

		// find the geometry information

		JIndex geomIndex;
		ok = rootGeom.SearchSorted(RootGeom(root,0,0), JListT::kAnyMatch, &geomIndex);
		assert( ok );
		const RootGeom geom = rootGeom.GetElement(geomIndex);

		// if root is not already in master list, append it

		JIndex rootIndex;
		if (!FindRoot(root, *rootList, &rootIndex))
			{
			const RootMIInfo info(root, geom.h, jnew JArray<RootConn>);
			assert( info.connList != nullptr );
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

	Returns true if rootList contains the given root.  We have to do
	a linear search because roots are appended to the list as they are
	encountered to allow the list to grow as the overall search for roots
	proceeds. (see MinimizeMILinks())  This is also required because the
	indicies stored in RootConn index into rootList.

 ******************************************************************************/

bool
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
			return true;
			}
		}

	*index = 0;
	return false;
}

/******************************************************************************
 FindClass

	Returns true if a class with the given full name exists.

 ******************************************************************************/

bool
CBTree::FindClass
	(
	const JString&	fullName,
	const CBClass**	theClass
	)
	const
{
	return FindClass(fullName, const_cast<CBClass**>(theClass));
}

bool
CBTree::FindClass
	(
	const JString&	fullName,
	CBClass**		theClass
	)
	const
{
	bool found = false;
	JIndex index   = 0;
	if (!IsEmpty())
		{
		CBClass target(fullName);
		found = itsClassesByFull->SearchSorted(&target, JListT::kFirstMatch, &index);
		}

	if (found)
		{
		*theClass = itsClassesByFull->GetElement(index);
		return true;
		}
	else
		{
		*theClass = nullptr;
		return false;
		}
}

/******************************************************************************
 IsUniqueClassName

	Returns true if there exists a single class with the given name,
	not full name.

 ******************************************************************************/

bool
CBTree::IsUniqueClassName
	(
	const JString&	name,
	const CBClass**	theClass
	)
	const
{
	*theClass = nullptr;

	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsClassesByFull->GetElement(i);
		if (c->GetName() == name)
			{
			if (*theClass == nullptr)
				{
				*theClass = c;
				}
			else
				{
				*theClass = nullptr;
				return false;
				}
			}
		}

	return *theClass != nullptr;
}

/******************************************************************************
 ClosestVisibleMatch

	Returns the closest visible match for the given name prefix.

 ******************************************************************************/

bool
CBTree::ClosestVisibleMatch
	(
	const JString&	prefixStr,
	CBClass**		theClass
	)
	const
{
	if (IsEmpty())
		{
		*theClass = nullptr;
		return false;
		}

	CBClass target(prefixStr);
	bool found;
	JIndex index =
		itsVisibleByName->SearchSorted1(&target, JListT::kFirstMatch, &found);
	if (index > itsVisibleByName->GetElementCount())		// insert beyond end of list
		{
		index = itsVisibleByName->GetElementCount();
		}
	*theClass = itsVisibleByName->GetElement(index);
	return true;
}

/******************************************************************************
 FindParent

	Does "container" or any of its parents contain a class called "parentName"?

	Returns true if a class "parentName" exists and the namespace is a class
	that is an ancestor of the class "container".

	"parent" returns the class "parentName".
	"nameSpace" returns the namespace of "parentName".

	Called by CBClass::FindParent().

 ******************************************************************************/

bool
CBTree::FindParent
	(
	const JString&	parentName,
	const CBClass*	container,
	CBClass**		parent,
	JString*		nameSpace
	)
	const
{
	const JUtf8Byte* namespaceOp = container->GetNamespaceOperator();

	JString parentSuffix(namespaceOp);
	parentSuffix += parentName;

	for (auto* c : *itsClassesByFull)
		{
		const JString& cName = c->GetFullName();
		if (cName.EndsWith(parentSuffix) &&
			parentSuffix.GetCharacterCount() < cName.GetCharacterCount())
			{
			JString prefixClassName = cName;
			JStringIterator iter(&prefixClassName, kJIteratorStartAtEnd);
			iter.Prev(parentSuffix);
			iter.RemoveAllNext();
			iter.Invalidate();

			const CBClass* prefixClass = nullptr;
			if (FindClass(prefixClassName, &prefixClass) &&
				(prefixClass == container ||
				 prefixClass->IsAncestor(container)))
				{
				*parent    = c;
				*nameSpace = prefixClassName + namespaceOp;
				return true;
				}
			}
		}

	*parent = nullptr;
	nameSpace->Clear();
	return false;
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
	return itsClassesByFull->GetElement(index);
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
	const bool found = itsClassesByFull->Find(theClass, &index);
	assert( found );
	return index;
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
CBTree::HasSelection()
	const
{
	const JSize classCount = itsVisibleByName->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		if ((itsVisibleByName->GetElement(i))->IsSelected())
			{
			return true;
			}
		}

	return false;
}

/******************************************************************************
 GetSelectedClasses

	Do *not* call DeleteAll() on the classList that is returned!

 ******************************************************************************/

bool
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
		CBClass* c = itsVisibleByName->GetElement(i);
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
	itsBroadcastClassSelFlag = false;

	const JSize count = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsClassesByFull->GetElement(i))->SetSelected(false);
		}

	itsBroadcastClassSelFlag = true;

	Broadcast(AllClassesDeselected());
}

/******************************************************************************
 GetSelectionCoverage

	Returns true if there are any selected classes, and then sets *coverage
	to the coverage of these classes, and *selCount to the number of classes.

 ******************************************************************************/

bool
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
		*coverage = (classList.GetFirstElement())->GetFrame();

		const JSize count = classList.GetElementCount();
		for (JIndex i=2; i<=count; i++)
			{
			*coverage = JCovering(*coverage, (classList.GetElement(i))->GetFrame());
			}

		*selCount = classList.GetElementCount();
		return true;
		}
	else
		{
		coverage->Set(0,0,0,0);
		*selCount = 0;
		return false;
		}
}

/******************************************************************************
 SelectClasses

	Selects all the classes with the given name, not full name.

 ******************************************************************************/

void
CBTree::SelectClasses
	(
	const JString&	name,
	const bool	deselectAll
	)
{
	if (deselectAll)
		{
		DeselectAll();
		}

	bool changed       = false;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsClassesByFull->GetElement(i);
		if (theClass->GetName() == name)
			{
			theClass->SetSelected(true);
			if (!theClass->IsVisible())
				{
				changed = true;
				theClass->ForceVisible();
				}
			}
		}

	if (changed)
		{
		RecalcVisible(true);		// ForceVisible() can uncollapse
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
	const JString&	fnName,
	const bool	caseSensitive,
	const bool	deselectAll
	)
{
/*
	if (deselectAll)
		{
		DeselectAll();
		}

	bool changed       = false;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* theClass = itsClassesByFull->GetElement(i);
		if (theClass->Implements(fnName, caseSensitive))
			{
			theClass->SetSelected(true);
			if (!theClass->IsVisible())
				{
				changed = true;
				theClass->ForceVisible();
				}
			}
		}

	if (changed)
		{
		RecalcVisible(true);		// ForceVisible() can uncollapse
		Broadcast(Changed());
		}
*/
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
		bool changed       = false;
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.GetElement(i);
			const JSize parentCount = c->GetParentCount();
			for (JIndex j=1; j<=parentCount; j++)
				{
				CBClass* p;
				const bool ok = c->GetParent(j, &p);
				assert( ok );
				p->SetSelected(true);
				if (!p->IsVisible())
					{
					changed = true;
					p->ForceVisible();
					}
				}
			}

		if (changed)
			{
			RecalcVisible(true);		// ForceVisible() can uncollapse
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
		bool changed       = false;
		const JSize classCount = classList.GetElementCount();
		const JSize totalCount = itsClassesByFull->GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.GetElement(i);
			for (JIndex j=1; j<=totalCount; j++)
				{
				CBClass* c1 = itsClassesByFull->GetElement(j);
				if (c->IsAncestor(c1))
					{
					c1->SetSelected(true);
					if (!c1->IsVisible())
						{
						changed = true;
						c1->ForceVisible();
						}
					}
				}
			}

		if (changed)
			{
			RecalcVisible(true);		// ForceVisible() can uncollapse
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
			(classList.GetElement(i))->ViewSource();
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
			(classList.GetElement(i))->ViewHeader();
			}
		}
}

/******************************************************************************
 ViewSelectedFunctionLists

	Opens a function list window for each selected class.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

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
			itsDirector->ViewFunctionList(classList.GetElement(i));
			}
		}
}

#endif

/******************************************************************************
 CopySelectedClassNames

	Copies the names of the selected classes to the text clipboard.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
CBTree::CopySelectedClassNames()
	const
{
	JPtrArray<JString> nameList(JPtrArrayT::kForgetAll);

	const JPtrArray<CBClass>& classList = GetVisibleClasses();
	const JSize classCount              = classList.GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		const CBClass* theClass = classList.GetElement(i);
		if (theClass->IsSelected())
			{
			const JString& name = theClass->GetFullName();
			nameList.Append(const_cast<JString*>(&name));
			}
		}

	if (!nameList.IsEmpty())
		{
		auto* data = jnew JXTextSelection(itsDirector->GetDisplay(), nameList);
		assert( data != nullptr );

		((itsDirector->GetDisplay())->GetSelectionManager())->SetData(kJXClipboardName, data);
		}
}

#endif

/******************************************************************************
 DeriveFromSelected

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

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
		JGetUserNotification()->DisplayMessage(JGetString("RequiresCodeMill::CBTree"));
		return;
		}

	// build data files describing ancestor classes

	JString outputPath;
	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	bool success = true;

	JPtrArray<CBClass> ancestorList(JPtrArrayT::kForgetAll);

	const JSize parentCount = parentList.GetElementCount();
	JString dataFileName, srcFileName;
	for (JIndex i=1; i<=parentCount; i++)
		{
		CBClass* parent = parentList.GetElement(i);

		ancestorList.RemoveAll();
		CollectAncestors(parent, &ancestorList);
		ancestorList.Prepend(parent);		// first element must be base class

		const JError err = JCreateTempFile(&dataFileName);
		if (!err.OK())
			{
			err.ReportIfError();
			success = false;
			break;
			}
		argList.Append(dataFileName);

		std::ofstream output(dataFileName.GetBytes());
		output << kCodeMillDataVersion << std::endl;

		const JSize ancestorCount = ancestorList.GetElementCount();
		output << ancestorCount << std::endl;

		for (JIndex j=1; j<=ancestorCount; j++)
			{
			CBClass* ancestor = ancestorList.GetElement(j);
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
			success = false;
			break;
			}
		}

	// invoke Code Mill

	const JSize fileCount = argList.GetElementCount();
	if (success && fileCount > 0)
		{
		argList.Prepend(JPrepArgForExec(outputPath));
		argList.Prepend(kCodeMillOptions);
		argList.Prepend(kCodeMillProgramName);
		JSimpleProcess::Create(argList, true);
		}
	else if (fileCount > 0)
		{
		for (JIndex i=1; i<=fileCount; i++)
			{
			remove(argList.GetElement(i)->GetBytes());
			}
		}
}

#endif

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
			bool found = false;

			const JSize count = list->GetElementCount();
			for (JIndex j=1; j<=count; j++)
				{
				if (CompareClassFullNames(parent, list->GetElement(j)) ==
					JListT::kFirstEqualSecond)
					{
					found = true;
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
 GetMenuInfo

 ******************************************************************************/

void
CBTree::GetMenuInfo
	(
	bool* hasSelection,
	bool* canCollapse,
	bool* canExpand
	)
	const
{
	*hasSelection = false;
	*canCollapse  = false;
	*canExpand    = false;

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		*hasSelection = true;

		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.GetElement(i);
			if (c->IsCollapsed())
				{
				*canExpand = true;
				}
			else if (c->HasPrimaryChildren())
				{
				*canCollapse = true;
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
	const bool collapse
	)
{
	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
		{
		bool changed       = false;
		const JSize classCount = classList.GetElementCount();
		for (JIndex i=1; i<=classCount; i++)
			{
			CBClass* c = classList.GetElement(i);
			if ((!collapse || c->HasPrimaryChildren()) &&
				c->IsCollapsed() != collapse)
				{
				changed = true;
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
	bool changed       = false;
	const JSize classCount = itsClassesByFull->GetElementCount();
	for (JIndex i=1; i<=classCount; i++)
		{
		CBClass* c = itsClassesByFull->GetElement(i);
		if (c->IsCollapsed())
			{
			changed = true;
			c->SetCollapsed(false);
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

bool
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
		CBClass* c = itsVisibleByName->GetElement(i);
		if (c->Contains(pt))
			{
			*theClass = c;
			return true;
			}
		}

	*theClass = nullptr;
	return false;
}

/******************************************************************************
 HitSameClass

	Return the class that contains both points.

 ******************************************************************************/

bool
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
		CBClass* c        = itsVisibleByName->GetElement(i);
		const bool c1 = c->Contains(pt1);
		const bool c2 = c->Contains(pt2);
		if (c1 && c2)
			{
			*theClass = c;
			return true;
			}
		else if (c1 || c2)	// quit now if it contains only one pt
			{
			*theClass = nullptr;
			return false;
			}
		}

	*theClass = nullptr;
	return false;
}

/******************************************************************************
 BroadcastSelectionChange (private)

	Called by CBClass.

 ******************************************************************************/

void
CBTree::BroadcastSelectionChange
	(
	CBClass*		theClass,
	const bool	isSelected
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
	p.SetFontSize(itsFontSize);

	if (!itsDrawMILinksOnTopFlag)
		{
		for (auto* c : *itsVisibleByName)
			{
			c->DrawMILinks(p, rect);
			}
		}

	for (auto* c : *itsVisibleByName)
		{
		c->Draw(p, rect);
		}

	if (itsDrawMILinksOnTopFlag)
		{
		for (auto* c : *itsVisibleByName)
			{
			c->DrawMILinks(p, rect);
			}
		}

	for (auto* c : *itsVisibleByName)
		{
		c->DrawText(p, rect);
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

#ifndef CODE_CRUSADER_UNIT_TEST

JFontManager*
CBTree::GetFontManager()
	const
{
	return itsDirector->GetDisplay()->GetFontManager();
}

#endif

/******************************************************************************
 SetFontSize

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

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

		for (auto* c : *itsClassesByFull)
			{
			c->CalcFrame();
			}

		const JFloat h2 = CBClass::GetTotalHeight(this, GetFontManager());
		Broadcast(FontSizeChanged(origSize, size, h2/h1));	// scale scrolltabs first
		PlaceAll();
		}
}

#endif

/******************************************************************************
 GetLineHeight

	Get the quantization step size for the vertical placement of classes.

 ******************************************************************************/

JCoordinate
CBTree::GetLineHeight()
	const
{
	return itsClassesByFull->GetFirstElement()->GetTotalHeight();
}

/******************************************************************************
 CompareClassFullNames (static private)

 ******************************************************************************/

JListT::CompareResult
CBTree::CompareClassFullNames
	(
	CBClass* const & c1,
	CBClass* const & c2
	)
{
	return JCompareStringsCaseSensitive(
			const_cast<JString*>(&c1->GetFullName()),
			const_cast<JString*>(&c2->GetFullName()));
}

/******************************************************************************
 CompareClassNames (static private)

 ******************************************************************************/

JListT::CompareResult
CBTree::CompareClassNames
	(
	CBClass* const & c1,
	CBClass* const & c2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&c1->GetName()),
			const_cast<JString*>(&c2->GetName()));
}

/******************************************************************************
 CompareRGClassPtrs (static private)

	We sort the actual pointers because that's all we need to look up.

 ******************************************************************************/

JListT::CompareResult
CBTree::CompareRGClassPtrs
	(
	const RootGeom& i1,
	const RootGeom& i2
	)
{
	if (i1.root < i2.root)
		{
		return JListT::kFirstLessSecond;
		}
	else if (i1.root == i2.root)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 CompareRSContent (static private)

	We compare the boolean content arrays.

 ******************************************************************************/

JListT::CompareResult
CBTree::CompareRSContent
	(
	const RootSubset& s1,
	const RootSubset& s2
	)
{
	const bool* b1 = (s1.content)->GetCArray();
	const bool* b2 = (s2.content)->GetCArray();

	const JSize count = (s1.content)->GetElementCount();
	for (JIndex i=1; i<=count; i++, b1++, b2++)
		{
		if (!(*b1) && *b2)
			{
			return JListT::kFirstLessSecond;
			}
		else if (*b1 && !(*b2))
			{
			return JListT::kFirstGreaterSecond;
			}
		}

	return JListT::kFirstEqualSecond;
}
