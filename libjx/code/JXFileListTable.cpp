/******************************************************************************
 JXFileListTable.cpp

	Displays a sorted list of files.  Derived classes can listen to the object
	returned by GetFullNameDataList() and keep an array of extra data in
	sync with our information.

	BASE CLASS = JXTable

	Copyright (C) 1998-99 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXFileListTable.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <JXTEBase.h>
#include <JXTextMenu.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXDNDManager.h>
#include <JXFileSelection.h>
#include <JXWebBrowser.h>
#include <jXUtil.h>
#include <jXGlobals.h>

#include <JTableSelection.h>
#include <JFontManager.h>
#include <JRegex.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jMouseUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>

const JCoordinate kIconWidth    = 20;
const JCoordinate kTextPadding  = 5;
const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const JCharacter kInactiveChar = ' ';	// file names starting with this are gray

static const JCharacter* kSelectionDataID = "JXFileListTable";

// JBroadcaster messages

const JCharacter* JXFileListTable::kProcessSelection = "ProcessSelection::JXFileListTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileListTable::JXFileListTable
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
	JXTable(10, 10, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsRegex              = NULL;
	itsAcceptFileDropFlag = kJFalse;
	itsBSRemoveSelFlag    = kJFalse;
	itsDragType           = kInvalidDrag;
	itsMaxStringWidth     = 0;
	itsEditMenuProvider   = NULL;
	itsEditMenu           = NULL;

	itsFileList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, 100);
	assert( itsFileList != NULL );
	// UNIX file system is case-sensitive.
	itsFileList->SetCompareFunction(JCompareStringsCaseSensitive);
	ListenTo(itsFileList);

	itsVisibleList = jnew JArray<VisInfo>(100);
	assert( itsVisibleList != NULL );
	itsVisibleList->SetSortOrder(JListT::kSortAscending);

	itsFileIcon = jnew JXImage(GetDisplay(), jx_plain_file_small);
	assert( itsFileIcon != NULL );
	itsFileIcon->ConvertToRemoteStorage();

	const JIndex blackColor = GetColormap()->GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);

	AppendCols(1);
	SetDefaultRowHeight((GetFontManager()->GetDefaultFont()).GetLineHeight() +
						2*kVMarginWidth);

	SetSelectionBehavior(kJTrue, kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileListTable::~JXFileListTable()
{
	StopListening(itsFileList);		// avoid calling JXScrollbarSet in Receive()
	jdelete itsFileList;
	jdelete itsVisibleList;

	jdelete itsRegex;
	jdelete itsFileIcon;
}

/******************************************************************************
 IsInactive (private)

 ******************************************************************************/

inline JBoolean
JXFileListTable::IsInactive
	(
	const JCharacter* fullName
	)
	const
{
	return JI2B(fullName[0] == kInactiveChar);
}

/******************************************************************************
 AddFile

	Returns kJTrue if the file was inserted.  Returns kJFalse if the file
	was already in the list.

	If a relative path is added, then GetFullName() will call the virtual
	function ResolveFullName().

	If fullNameIndex is not NULL, it is set to the index into GetFullNameList(),
	both when the file is inserted and when the file is already there.

 ******************************************************************************/

JBoolean
JXFileListTable::AddFile
	(
	const JCharacter*	fullName,
	JIndex*				fullNameIndex	// can be NULL
	)
{
	assert( !JString::IsEmpty(fullName) );

	if (fullNameIndex != NULL)
		{
		*fullNameIndex = 0;
		}

	ClearSelection();

	JString* s = jnew JString(fullName);
	assert( s != NULL );

	JBoolean found;
	const JIndex index = itsFileList->SearchSorted1(s, JListT::kAnyMatch, &found);
	if (fullNameIndex != NULL)
		{
		*fullNameIndex = index;
		}

	if (!found)
		{
		itsFileList->InsertAtIndex(index, s);
		FilterFile(index);
		AdjustColWidths();
		return kJTrue;
		}
	else
		{
		jdelete s;
		return kJFalse;
		}
}

/******************************************************************************
 RemoveFile

 ******************************************************************************/

void
JXFileListTable::RemoveFile
	(
	const JCharacter* fullName
	)
{
	ClearSelection();

	JString s = fullName;
	JIndex index;
	if (itsFileList->SearchSorted(&s, JListT::kAnyMatch, &index))
		{
		itsFileList->DeleteElement(index);
		}
}

/******************************************************************************
 RemoveFiles

 ******************************************************************************/

void
JXFileListTable::RemoveFiles
	(
	const JPtrArray<JString>& fileList
	)
{
	ClearSelection();

	const JSize count = fileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* s = const_cast<JString*>(fileList.GetElement(i));
		JIndex index;
		if (itsFileList->SearchSorted(s, JListT::kAnyMatch, &index))
			{
			itsFileList->DeleteElement(index);
			}
		}
}

/******************************************************************************
 RemoveSelectedFiles

 ******************************************************************************/

void
JXFileListTable::RemoveSelectedFiles()
{
	JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JTableSelectionIterator iter(&s);

		JPoint cell;
		while (iter.Next(&cell))
			{
			itsFileList->DeleteElement(RowIndexToFileIndex(cell.y));
			}
		}
}

/******************************************************************************
 RemoveAllFiles

 ******************************************************************************/

void
JXFileListTable::RemoveAllFiles()
{
	itsFileList->CleanOut();
	RebuildTable();
}

/******************************************************************************
 GetFilterRegex

 ******************************************************************************/

JBoolean
JXFileListTable::GetFilterRegex
	(
	JString* regexStr
	)
	const
{
	if (itsRegex != NULL)
		{
		*regexStr = itsRegex->GetPattern();
		return kJTrue;
		}
	else
		{
		regexStr->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetFilterRegex

 ******************************************************************************/

JError
JXFileListTable::SetFilterRegex
	(
	const JCharacter* regexStr
	)
{
	if (JString::IsEmpty(regexStr))
		{
		ClearFilterRegex();
		return JNoError();
		}
	else if (itsRegex == NULL)
		{
		itsRegex = jnew JRegex;
		assert( itsRegex != NULL );
		const JError err = itsRegex->SetPattern(regexStr);
		itsRegex->SetCaseSensitive(kJFalse);
		if (err.OK())
			{
			RebuildTable();
			}
		else
			{
			jdelete itsRegex;
			itsRegex = NULL;
			}
		return err;
		}
	else
		{
		JString origRegexStr = itsRegex->GetPattern();
		if (regexStr == origRegexStr)
			{
			return JNoError();
			}
		else
			{
			const JError err = itsRegex->SetPattern(regexStr);
			if (err.OK())
				{
				RebuildTable();
				}
			else
				{
				const JError err2 = itsRegex->SetPattern(origRegexStr);
				assert_ok( err2 );
				}
			return err;
			}
		}
}

/******************************************************************************
 ClearFilterRegex

 ******************************************************************************/

void
JXFileListTable::ClearFilterRegex()
{
	if (itsRegex != NULL)
		{
		jdelete itsRegex;
		itsRegex = NULL;

		RebuildTable();
		}
}

/******************************************************************************
 RebuildTable (private)

 ******************************************************************************/

void
JXFileListTable::RebuildTable
	(
	const JBoolean maintainScroll
	)
{
	const JPoint scrollPt = (GetAperture()).topLeft();

	itsVisibleList->RemoveAll();
	RemoveAllRows();
	ClearIncrementalSearchBuffer();
	itsMaxStringWidth = 0;

	const JSize count = itsFileList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FilterFile(i);
		}

	AdjustColWidths();

	if (maintainScroll)
		{
		ScrollTo(scrollPt);
		}
}

/******************************************************************************
 FilterFile (private)

	Client must call AdjustColWidths().

 ******************************************************************************/

void
JXFileListTable::FilterFile
	(
	const JIndex fileIndex
	)
{
	const JString* fullName = itsFileList->GetElement(fileIndex);

	VisInfo info;
	if (fullName->LocateLastSubstring(ACE_DIRECTORY_SEPARATOR_STR, &(info.nameIndex)))
		{
		info.nameIndex++;
		}
	else
		{
		info.nameIndex = 1;
		}

	if (itsRegex == NULL || itsRegex->Match(fullName->GetCString() + info.nameIndex-1))
		{
		const JString fileName(*fullName, JIndexRange(info.nameIndex, fullName->GetLength()));
		itsVisibleList->SetCompareObject(PrefixMatch(fileName, *itsFileList));

		info.fileIndex = 0;
		JBoolean found;
		const JIndex rowIndex = itsVisibleList->SearchSorted1(info, JListT::kFirstMatch, &found);

		info.fileIndex = fileIndex;
		info.drawIndex = info.nameIndex;
		itsVisibleList->InsertElementAtIndex(rowIndex, info);

		InsertRows(rowIndex, 1);

		JIndex endRowIndex = rowIndex;
		if (found)
			{
			endRowIndex = UpdateDrawIndex(rowIndex, fileName);
			}

		// check width of all rows that were affected

		for (JIndex i=rowIndex; i<=endRowIndex; i++)
			{
			info = itsVisibleList->GetElement(i);
			const JCharacter* drawStr =
				(itsFileList->GetElement(info.fileIndex))->GetCString() + info.drawIndex-1;
			const JSize w = (GetFontManager()->GetDefaultFont()).GetStringWidth(drawStr);
			if (w > itsMaxStringWidth)
				{
				itsMaxStringWidth = w;
				}
			}
		}
}

/******************************************************************************
 UpdateDrawIndex (private)

	Returns the last index that was updated.

 ******************************************************************************/

JIndex
JXFileListTable::UpdateDrawIndex
	(
	const JIndex	firstIndex,
	const JString&	fileName
	)
	const
{
JIndex i;

	const JSize count = itsVisibleList->GetElementCount();

	// find the last occurrence of fileName

	JIndex lastIndex = firstIndex+2;	// first one that could be different
	while (lastIndex <= count)
		{
		const VisInfo info  = itsVisibleList->GetElement(lastIndex);
		const JCharacter* n =
			(itsFileList->GetElement(info.fileIndex))->GetCString() + info.nameIndex-1;
		if (JString::Compare(n, fileName, kJFalse) != 0)
			{
			break;
			}
		lastIndex++;
		}
	lastIndex--;

	// calculate the length of the minimum prefix shared by the files

	const JString* firstFile = itsFileList->GetElement(RowIndexToFileIndex(firstIndex));
	JSize minLength = firstFile->GetLength();
	for (i=firstIndex; i<lastIndex; i++)
		{
		const JString* s1 = itsFileList->GetElement(RowIndexToFileIndex(i));
		const JString* s2 = itsFileList->GetElement(RowIndexToFileIndex(i+1));
		minLength         = JMin(minLength, JCalcMatchLength(*s1, *s2));
		}

	// back up so drawing starts at the beginning of a directory name

	while (minLength > 0 && firstFile->GetCharacter(minLength) != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		minLength--;
		}
	if (minLength == 1 && firstFile->GetCharacter(minLength) == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		minLength = 0;		// might as well show full path
		}

	// update drawIndex for each file

	for (i=firstIndex; i<=lastIndex; i++)
		{
		VisInfo info   = itsVisibleList->GetElement(i);
		info.drawIndex = minLength+1;
		itsVisibleList->SetElement(i, info);
		}

	return lastIndex;
}

/******************************************************************************
 GetFileName (protected)

 ******************************************************************************/

JString
JXFileListTable::GetFileName
	(
	const JIndex rowIndex
	)
	const
{
	const VisInfo info      = itsVisibleList->GetElement(rowIndex);
	const JString* fullName = itsFileList->GetElement(info.fileIndex);
	return JString(*fullName, JIndexRange(info.nameIndex, fullName->GetLength()));
}

/******************************************************************************
 GetFullName

	Get the full name for the file in the specified row.  This is public
	because it is needed by anybody who catches the ProcessSelection message.

 ******************************************************************************/

JBoolean
JXFileListTable::GetFullName
	(
	const JIndex	rowIndex,
	JString*		fullName
	)
	const
{
	const JIndex fileIndex = RowIndexToFileIndex(rowIndex);
	JString* n             = itsFileList->GetElement(fileIndex);
	if (IsInactive(*n))
		{
		fullName->Clear();
		return kJFalse;
		}
	else if (JIsAbsolutePath(*n) ||
			 const_cast<JXFileListTable*>(this)->
				MainResolveFullName(rowIndex, fileIndex, n))
		{
		*fullName = *n;
		return kJTrue;
		}
	else
		{
		n->PrependCharacter(kInactiveChar);

		VisInfo info = itsVisibleList->GetElement(rowIndex);
		info.nameIndex++;
		info.drawIndex++;
		itsVisibleList->SetElement(rowIndex, info);

		itsFileList->Sort();	// after updating VisInfo

		fullName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 MainResolveFullName (private)

 ******************************************************************************/

JBoolean
JXFileListTable::MainResolveFullName
	(
	const JIndex	rowIndex,
	const JIndex	fileIndex,
	JString*		name		// pointer from itsFileList
	)
{
	JString n = *name;
	if (ResolveFullName(&n) && JIsAbsolutePath(n) && JFileExists(n))
		{
		*name = n;
		itsFileList->Sort();
		RebuildTable();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ResolveFullName (virtual protected)

	Derived classes should override this if they want to call AddFile()
	with relative paths and then find the files later on.

	On input, *name is a relative path.  If it can be converted to
	an absolute path, the function should return kJTrue.

 ******************************************************************************/

JBoolean
JXFileListTable::ResolveFullName
	(
	JString* name
	)
{
	return kJFalse;
}

#if 0

/******************************************************************************
 GetFullName

	Get the full name for the specified file.  Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXFileListTable::GetFullName
	(
	const JString&	fileName,
	JString*		fullName
	)
	const
{
	JIndex index;
	if (FileNameToFileIndex(fileName, &index))
		{
		*fullName = *(itsFileList->GetElement(index));
		return kJTrue;
		}
	else
		{
		fullName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 FileNameToFileIndex (private)

	Get the index into itsFileList for the specified file.

 ******************************************************************************/

JBoolean
JXFileListTable::FileNameToFileIndex
	(
	const JString&	name,
	JIndex*			index
	)
	const
{
	const JSize nameLen = name.GetLength();

	const JSize count = itsFileList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* f = itsFileList->GetElement(i);
		const JSize len  = f->GetLength();
		if (f->EndsWith(name) &&
			(len == nameLen ||
			 f->GetCharacter(len - nameLen) == ACE_DIRECTORY_SEPARATOR_CHAR))
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

#endif

/******************************************************************************
 HasSelection

 ******************************************************************************/

JBoolean
JXFileListTable::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

JBoolean
JXFileListTable::GetSelection
	(
	JPtrArray<JString>* fileList
	)
	const
{
	fileList->CleanOut();

	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	JString fullName;
	while (iter.Next(&cell))
		{
		if (GetFullName(cell.y, &fullName))
			{
			fileList->Append(fullName);
			}
		}

	return !fileList->IsEmpty();
}

/******************************************************************************
 SelectSingleEntry

 ******************************************************************************/

void
JXFileListTable::SelectSingleEntry
	(
	const JIndex	index,
	const JBoolean	scroll
	)
{
	SelectSingleCell(JPoint(1, index), scroll);
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JXFileListTable::SelectAll()
{
	(GetTableSelection()).SelectAll();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
JXFileListTable::ClearSelection()
{
	(GetTableSelection()).ClearSelection();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ShowSelectedFileLocations

 ******************************************************************************/

void
JXFileListTable::ShowSelectedFileLocations()
	const
{
	JTableSelectionIterator iter(&(GetTableSelection()));

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JPoint cell;
	JString fullName;
	while (iter.Next(&cell))
		{
		if (GetFullName(cell.y, &fullName))
			{
			list.Append(fullName);
			}
		}

	(JXGetWebBrowser())->ShowFileLocations(list);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXFileListTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	// draw icon

	JRect r = rect;
	r.right = r.left + kIconWidth;
	p.Image(*itsFileIcon, itsFileIcon->GetBounds(), r);

	// draw name

	const VisInfo info  = itsVisibleList->GetElement(cell.y);
	const JString* name = itsFileList->GetElement(info.fileIndex);
	const JCharacter* s = name->GetCString() + info.drawIndex-1;

	if (IsInactive(*name))
		{
		p.SetFontStyle((p.GetColormap())->GetGrayColor(50));
		}

	r = rect;
	r.left += kIconWidth + kHMarginWidth;
	p.String(r, s, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ClearIncrementalSearchBuffer();
	JTableSelection& s   = GetTableSelection();
	const JBoolean noMod = JNegate( modifiers.shift() || modifiers.control() );

	JPoint cell;

	itsDragType = kInvalidDrag;
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if (button == kJXLeftButton && clickCount == 1 && noMod)
		{
		itsDragType    = kWaitForDND;
		itsMouseDownPt = pt;
		if (!s.IsSelected(cell))
			{
			SelectSingleEntry(cell.y, kJFalse);
			}
		}
	else if (button == kJXLeftButton && clickCount == 2 && noMod)
		{
		s.SetBoat(cell);
		s.SetAnchor(cell);
		Broadcast(ProcessSelection());
		}
	else
		{
		BeginSelectionDrag(cell, button, modifiers);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kWaitForDND && JMouseMoved(itsMouseDownPt, pt))
		{
		JXFileSelection* data = jnew JXFileSelection(this, kSelectionDataID);
		assert( data != NULL );

		BeginDND(pt, buttonStates, modifiers, data);
		itsDragType = kInvalidDrag;
		}
	else
		{
		ContinueSelectionDrag(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kWaitForDND)		// DND never started
		{
		JPoint cell;
		const JBoolean ok = GetCell(itsMouseDownPt, &cell);
		assert( ok );
		SelectSingleEntry(cell.y, kJFalse);
		}
	else
		{
		FinishSelectionDrag();
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleFocusEvent()
{
	JXTable::HandleFocusEvent();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const JBoolean hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (key == ' ')
		{
		ClearSelection();
		}

	else if (key == kJReturnKey)
		{
		ClearIncrementalSearchBuffer();
		if (hadSelection && modifiers.meta())
			{
			ShowSelectedFileLocations();
			}
		else if (hadSelection)
			{
			Broadcast(ProcessSelection());
			}
		}

	else if (key == kJDeleteKey && itsBSRemoveSelFlag)
		{
		RemoveSelectedFiles();
		}

	else if (key == kJUpArrow || key == kJDownArrow)
		{
		ClearIncrementalSearchBuffer();
		if (!hadSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleEntry(GetRowCount());
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}

	else if ((key == 'c' || key == 'C') && modifiers.meta() && !modifiers.shift())
		{
		CopySelectedFileNames();
		}
	else if ((key == 'a' || key == 'A') && modifiers.meta() && !modifiers.shift())
		{
		SelectAll();
		}

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.AppendCharacter(key);

		JIndex index;
		if (ClosestMatch(itsKeyBuffer, &index))
			{
			JString saveBuffer = itsKeyBuffer;
			SelectSingleEntry(index);
			itsKeyBuffer = saveBuffer;
			}
		}

	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 ClearIncrementalSearchBuffer

 ******************************************************************************/

void
JXFileListTable::ClearIncrementalSearchBuffer()
{
	itsKeyBuffer.Clear();
}

/******************************************************************************
 ClosestMatch (private)

	Returns the index of the closest match for the given name prefix.

 ******************************************************************************/

JBoolean
JXFileListTable::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	JBoolean found;

	VisInfo target;
	target.fileIndex = 0;

	itsVisibleList->SetCompareObject(PrefixMatch(prefixStr, *itsFileList));
	*index = itsVisibleList->SearchSorted1(target, JListT::kFirstMatch, &found);
	if (*index > itsVisibleList->GetElementCount())		// insert beyond end of list
		{
		*index = itsVisibleList->GetElementCount();
		}
	return JConvertToBoolean( *index > 0 );
}

/******************************************************************************
 ClosestMatch class (private)

 ******************************************************************************/

JXFileListTable::PrefixMatch::PrefixMatch
	(
	const JString&				prefix,
	const JPtrArray<JString>&	fileList
	)
	:
	itsPrefix(prefix),
	itsFileList(fileList)
{
}

JXFileListTable::PrefixMatch::~PrefixMatch()
{
}

JListT::CompareResult
JXFileListTable::PrefixMatch::Compare
	(
	const VisInfo& i1,
	const VisInfo& i2
	)
	const
{
	assert( i1.fileIndex == 0 || i2.fileIndex == 0 );

	const JCharacter* s1 =
		(i1.fileIndex == 0 ?
		 itsPrefix.GetCString() :
		 (itsFileList.GetElement(i1.fileIndex))->GetCString() + i1.nameIndex-1);

	const JCharacter* s2 =
		(i2.fileIndex == 0 ?
		 itsPrefix.GetCString() :
		 (itsFileList.GetElement(i2.fileIndex))->GetCString() + i2.nameIndex-1);

	const int r = JString::Compare(s1, s2, kJFalse);
	if (r > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

JElementComparison<JXFileListTable::VisInfo>*
JXFileListTable::PrefixMatch::Copy()
	const
{
	PrefixMatch* copy = jnew PrefixMatch(itsPrefix, itsFileList);
	assert( copy != NULL );
	return copy;
}

/******************************************************************************
 SetEditMenuProvider

 ******************************************************************************/

void
JXFileListTable::SetEditMenuProvider
	(
	JXTEBase* te
	)
{
	if (itsEditMenu != NULL)
		{
		StopListening(itsEditMenu);
		}

	if (te != NULL && te->GetEditMenu(&itsEditMenu))
		{
		itsEditMenuProvider = te;
		ListenTo(itsEditMenu);
		}
	else
		{
		itsEditMenuProvider = NULL;
		itsEditMenu         = NULL;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

#define ADJUST_INDEX(m) \
	const JSize count = itsVisibleList->GetElementCount(); \
	VisInfo* info     = const_cast<VisInfo*>(itsVisibleList->GetCArray()); \
	for (JIndex i=0; i<count; i++) \
		{ \
		m->AdjustIndex(&(info[i].fileIndex)); \
		}

void
JXFileListTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileList && message.Is(JListT::kElementsInserted))
		{
		const JListT::ElementsInserted* m =
			dynamic_cast<const JListT::ElementsInserted*>(&message);
		assert( m != NULL );
		ADJUST_INDEX(m);
		}
	else if (sender == itsFileList && message.Is(JListT::kElementsRemoved))
		{
		const JListT::ElementsRemoved* m =
			dynamic_cast<const JListT::ElementsRemoved*>(&message);
		assert( m != NULL );

		const JSize count = itsVisibleList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			VisInfo info = itsVisibleList->GetElement(i);
			if (m->AdjustIndex(&(info.fileIndex)))
				{
				itsVisibleList->SetElement(i, info);
				}
			else
				{
				itsVisibleList->RemoveElement(i);
				RemoveRow(i);
				}
			}
		}
	else if (sender == itsFileList && message.Is(JListT::kElementMoved))
		{
		const JListT::ElementMoved* m =
			dynamic_cast<const JListT::ElementMoved*>(&message);
		assert( m != NULL );
		ADJUST_INDEX(m);
		}
	else if (sender == itsFileList && message.Is(JListT::kElementsSwapped))
		{
		const JListT::ElementsSwapped* m =
			dynamic_cast<const JListT::ElementsSwapped*>(&message);
		assert( m != NULL );
		ADJUST_INDEX(m);
		}
	else if (sender == itsFileList && message.Is(JListT::kElementChanged))
		{
		assert( 0 /* makes no sense since it's a JPtrArray */ );
		}
	else if (sender == itsFileList && message.Is(JListT::kSorted))
		{
		assert( 0 /* not allowed */ );
		}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditMenu();
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleEditMenu(selection->GetIndex());
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

#undef ADJUST_INDEX

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
JXFileListTable::UpdateEditMenu()
{
JBoolean ok;
JIndex index;

	if (HasSelection())
		{
		ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index);
		assert( ok );
		itsEditMenu->EnableItem(index);
		}

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index);
	assert( ok );
	itsEditMenu->EnableItem(index);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
JXFileListTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JTextEditor::CmdIndex cmd;
	if (itsEditMenuProvider->EditMenuIndexToCmd(index, &cmd))
		{
		if (cmd == JTextEditor::kCopyCmd)
			{
			CopySelectedFileNames();
			}
		else if (cmd == JTextEditor::kSelectAllCmd)
			{
			SelectAll();
			}
		}
}

/******************************************************************************
 CopySelectedFileNames (private)

 ******************************************************************************/

void
JXFileListTable::CopySelectedFileNames()
	const
{
	if (HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

		JTableSelectionIterator iter(&(GetTableSelection()));
		JPoint cell;
		while (iter.Next(&cell))
			{
			list.Append(GetFileName(cell.y));
			}

		JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != NULL );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
JXFileListTable::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kSelectionDataID) == 0)
		{
		assert( HasSelection() );

		JXFileSelection* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != NULL );

		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

		JTableSelectionIterator iter(&(GetTableSelection()));
		JPoint cell;
		JString fullName;
		while (iter.Next(&cell))
			{
			if (GetFullName(cell.y, &fullName))
				{
				list.Append(fullName);
				}
			}

		fileData->SetData(list);
		}
	else
		{
		JXTable::GetSelectionData(data, id);
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
JXFileListTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
JXFileListTable::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list from anybody but ourselves.

 ******************************************************************************/

JBoolean
JXFileListTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	// dropping on ourselves makes no sense

	if (!itsAcceptFileDropFlag || this == const_cast<JXWidget*>(source))
		{
		return kJFalse;
		}

	// we accept drops of type text/uri-list

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == urlXAtom)
			{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
JXFileListTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
					   urlList(JPtrArrayT::kDeleteAll);
	if (JXFileSelection::GetFileList(GetDNDManager()->GetDNDSelectionName(), time,
									 GetDisplay(), &fileNameList, &urlList))
		{
		const JSize fileCount = fileNameList.GetElementCount();
		for (JIndex i=1; i<=fileCount; i++)
			{
			AddFile(*(fileNameList.GetElement(i)));
			}
		JXReportUnreachableHosts(urlList);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXFileListTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
JXFileListTable::AdjustColWidths()
{
	const JCoordinate minWidth = kIconWidth + itsMaxStringWidth + kTextPadding;
	SetColWidth(1, JMax(minWidth, GetApertureWidth()));
}
