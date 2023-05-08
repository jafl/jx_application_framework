/******************************************************************************
 JXFileListTable.cpp

	Displays a sorted list of files.  Derived classes can listen to the object
	returned by GetFullNameDataList() and keep an array of extra data in
	sync with our information.

	BASE CLASS = JXTable

	Copyright (C) 1998-99 by John Lindal.

 ******************************************************************************/

#include "JXFileListTable.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXWindowPainter.h"
#include "JXImage.h"
#include "JXImageCache.h"
#include "JXColorManager.h"
#include "JXTEBase.h"
#include "JXTextMenu.h"
#include "JXSelectionManager.h"
#include "JXTextSelection.h"
#include "JXDNDManager.h"
#include "JXFileSelection.h"
#include "JXWebBrowser.h"
#include "jXUtil.h"
#include "jXGlobals.h"

#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_plain_file_small.xpm>

const JCoordinate kIconWidth    = 20;
const JCoordinate kTextPadding  = 5;
const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const JUtf8Byte kInactiveChar = ' ';	// file names starting with this are gray
const JUtf8Byte* kInactiveStr = " ";

static const JUtf8Byte* kSelectionDataID = "JXFileListTable";

// JBroadcaster messages

const JUtf8Byte* JXFileListTable::kProcessSelection = "ProcessSelection::JXFileListTable";

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
	itsRegex              = nullptr;
	itsAcceptFileDropFlag = false;
	itsBSRemoveSelFlag    = false;
	itsDragType           = kInvalidDrag;
	itsMaxStringWidth     = 0;
	itsEditMenuProvider   = nullptr;
	itsEditMenu           = nullptr;

	itsFileList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, 100);
	assert( itsFileList != nullptr );
	// UNIX file system is case-sensitive.
	itsFileList->SetCompareFunction(JCompareStringsCaseSensitive);
	ListenTo(itsFileList);

	itsVisibleList = jnew JArray<VisInfo>(100);
	assert( itsVisibleList != nullptr );
	itsVisibleList->SetSortOrder(JListT::kSortAscending);

	itsFileIcon = GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small);

	const JColorID blackColor = JColorManager::GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);

	AppendCols(1);
	SetDefaultRowHeight(JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()) +
						2*kVMarginWidth);

	SetSelectionBehavior(true, true);
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
}

/******************************************************************************
 IsInactive (private)

 ******************************************************************************/

inline bool
JXFileListTable::IsInactive
	(
	const JString& fullName
	)
	const
{
	return fullName.GetFirstCharacter() == kInactiveChar;
}

/******************************************************************************
 AddFile

	Returns true if the file was inserted.  Returns false if the file
	was already in the list.

	If a relative path is added, then GetFullName() will call the virtual
	function ResolveFullName().

	If fullNameIndex is not nullptr, it is set to the index into GetFullNameList(),
	both when the file is inserted and when the file is already there.

 ******************************************************************************/

bool
JXFileListTable::AddFile
	(
	const JString&	fullName,
	JIndex*			fullNameIndex	// can be nullptr
	)
{
	assert( !fullName.IsEmpty() );

	if (fullNameIndex != nullptr)
	{
		*fullNameIndex = 0;
	}

	ClearSelection();

	auto* s = jnew JString(fullName);
	assert( s != nullptr );

	bool found;
	const JIndex index = itsFileList->SearchSortedOTI(s, JListT::kAnyMatch, &found);
	if (fullNameIndex != nullptr)
	{
		*fullNameIndex = index;
	}

	if (!found)
	{
		itsFileList->InsertAtIndex(index, s);
		FilterFile(index);
		AdjustColWidths();
		return true;
	}
	else
	{
		jdelete s;
		return false;
	}
}

/******************************************************************************
 RemoveFile

 ******************************************************************************/

void
JXFileListTable::RemoveFile
	(
	const JString& fullName
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

	for (auto* s : fileList)
	{
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

bool
JXFileListTable::GetFilterRegex
	(
	JString* regexStr
	)
	const
{
	if (itsRegex != nullptr)
	{
		*regexStr = itsRegex->GetPattern();
		return true;
	}
	else
	{
		regexStr->Clear();
		return false;
	}
}

/******************************************************************************
 SetFilterRegex

 ******************************************************************************/

JError
JXFileListTable::SetFilterRegex
	(
	const JString& regexStr
	)
{
	if (regexStr.IsEmpty())
	{
		ClearFilterRegex();
		return JNoError();
	}
	else if (itsRegex == nullptr)
	{
		itsRegex = jnew JRegex;
		assert( itsRegex != nullptr );
		const JError err = itsRegex->SetPattern(regexStr);
		itsRegex->SetCaseSensitive(false);
		if (err.OK())
		{
			RebuildTable();
		}
		else
		{
			jdelete itsRegex;
			itsRegex = nullptr;
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
	if (itsRegex != nullptr)
	{
		jdelete itsRegex;
		itsRegex = nullptr;

		RebuildTable();
	}
}

/******************************************************************************
 RebuildTable (private)

 ******************************************************************************/

void
JXFileListTable::RebuildTable
	(
	const bool maintainScroll
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
	JStringIterator iter(*fullName, kJIteratorStartAtEnd);

	VisInfo info;
	if (iter.Prev(ACE_DIRECTORY_SEPARATOR_STR))
	{
		info.nameIndex = iter.GetNextCharacterIndex() + 1;
	}
	else
	{
		info.nameIndex = 1;
	}

	iter.Invalidate();

	if (itsRegex == nullptr || itsRegex->Match(JString(fullName->GetBytes() + info.nameIndex-1, JString::kNoCopy)))
	{
		const JString fileName(*fullName, JCharacterRange(info.nameIndex, fullName->GetCharacterCount()));
		InstallCompareWrapper(fileName);

		info.fileIndex = 0;
		bool found;
		const JIndex rowIndex = itsVisibleList->SearchSortedOTI(info, JListT::kFirstMatch, &found);

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
			const JString drawStr(
				(itsFileList->GetElement(info.fileIndex))->GetBytes() + info.drawIndex-1,
				JString::kNoCopy);
			const JSize w = JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), drawStr);
			if (w > itsMaxStringWidth)
			{
				itsMaxStringWidth = w;
			}
		}

		itsVisibleList->ClearCompareFunction();
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
		const VisInfo info = itsVisibleList->GetElement(lastIndex);
		const JUtf8Byte* n =
			(itsFileList->GetElement(info.fileIndex))->GetBytes() + info.nameIndex-1;
		if (JString::Compare(n, fileName, JString::kIgnoreCase) != 0)
		{
			break;
		}
		lastIndex++;
	}
	lastIndex--;

	// calculate the length of the minimum prefix shared by the files

	const JString* firstFile = itsFileList->GetElement(RowIndexToFileIndex(firstIndex));
	JSize minLength = firstFile->GetCharacterCount();
	for (i=firstIndex; i<lastIndex; i++)
	{
		const JString* s1 = itsFileList->GetElement(RowIndexToFileIndex(i));
		const JString* s2 = itsFileList->GetElement(RowIndexToFileIndex(i+1));
		minLength         = JMin(minLength, JString::CalcCharacterMatchLength(*s1, *s2));
	}

	// back up so drawing starts at the beginning of a directory name

	JStringIterator iter(*firstFile, kJIteratorStartAfter, minLength);
	JUtf8Character c;
	while (iter.Prev(&c) && c != ACE_DIRECTORY_SEPARATOR_CHAR)
	{
		minLength--;
	}
	iter.Invalidate();

	if (minLength == 1 && c == ACE_DIRECTORY_SEPARATOR_CHAR)
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
	return JString(*fullName, JCharacterRange(info.nameIndex, fullName->GetCharacterCount()));
}

/******************************************************************************
 GetFullName

	Get the full name for the file in the specified row.  This is public
	because it is needed by anybody who catches the ProcessSelection message.

 ******************************************************************************/

bool
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
		return false;
	}
	else if (JIsAbsolutePath(*n) ||
			 const_cast<JXFileListTable*>(this)->
				MainResolveFullName(rowIndex, fileIndex, n))
	{
		*fullName = *n;
		return true;
	}
	else
	{
		n->Prepend(kInactiveStr);

		VisInfo info = itsVisibleList->GetElement(rowIndex);
		info.nameIndex++;
		info.drawIndex++;
		itsVisibleList->SetElement(rowIndex, info);

		itsFileList->Sort();	// after updating VisInfo

		fullName->Clear();
		return false;
	}
}

/******************************************************************************
 MainResolveFullName (private)

 ******************************************************************************/

bool
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
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ResolveFullName (virtual protected)

	Derived classes should override this if they want to call AddFile()
	with relative paths and then find the files later on.

	On input, *name is a relative path.  If it can be converted to
	an absolute path, the function should return true.

 ******************************************************************************/

bool
JXFileListTable::ResolveFullName
	(
	JString* name
	)
{
	return false;
}

#if 0

/******************************************************************************
 GetFullName

	Get the full name for the specified file.  Returns true if successful.

 ******************************************************************************/

bool
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
		*fullName = *itsFileList->GetElement(index);
		return true;
	}
	else
	{
		fullName->Clear();
		return false;
	}
}

/******************************************************************************
 FileNameToFileIndex (private)

	Get the index into itsFileList for the specified file.

 ******************************************************************************/

bool
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
			return true;
		}
	}

	*index = 0;
	return false;
}

#endif

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
JXFileListTable::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

bool
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
	const bool	scroll
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
	const JUtf8Byte* s  = name->GetBytes() + info.drawIndex-1;

	if (IsInactive(*name))
	{
		p.SetFontStyle(JColorManager::GetGrayColor(50));
	}

	r = rect;
	r.left += kIconWidth + kHMarginWidth;
	p.String(r, JString(s, JString::kNoCopy), JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
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
	const bool noMod = !modifiers.shift() && !modifiers.control();

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
			SelectSingleEntry(cell.y, false);
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
		auto* data = jnew JXFileSelection(this, kSelectionDataID);
		assert( data != nullptr );

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
		const bool ok = GetCell(itsMouseDownPt, &cell);
		assert( ok );
		SelectSingleEntry(cell.y, false);
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const bool hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (c == ' ')
	{
		ClearSelection();
	}

	else if (c == kJReturnKey)
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

	else if (c == kJDeleteKey && itsBSRemoveSelFlag)
	{
		RemoveSelectedFiles();
	}

	else if (c == kJUpArrow || c == kJDownArrow)
	{
		ClearIncrementalSearchBuffer();
		if (!hadSelection && c == kJUpArrow && GetRowCount() > 0)
		{
			SelectSingleEntry(GetRowCount());
		}
		else
		{
			HandleSelectionKeyPress(c, modifiers);
		}
	}

	else if ((c == 'c' || c == 'C') && modifiers.meta() && !modifiers.shift())
	{
		CopySelectedFileNames();
	}
	else if ((c == 'a' || c == 'A') && modifiers.meta() && !modifiers.shift())
	{
		SelectAll();
	}

	else if (c.IsPrint() && !modifiers.control() && !modifiers.meta())
	{
		itsKeyBuffer.Append(c);

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
		JXTable::HandleKeyPress(c, keySym, modifiers);
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

bool
JXFileListTable::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	bool found;

	VisInfo target;
	target.fileIndex = 0;

	InstallCompareWrapper(prefixStr);
	*index = itsVisibleList->SearchSortedOTI(target, JListT::kFirstMatch, &found);
	if (*index > itsVisibleList->GetElementCount())		// insert beyond end of list
	{
		*index = itsVisibleList->GetElementCount();
	}
	itsVisibleList->ClearCompareFunction();
	return *index > 0;
}

/******************************************************************************
 Closest match (private)

 ******************************************************************************/

std::weak_ordering
JXFileListTable::CompareWrapper
	(
	const JPtrArray<JString>&	fileList,
	const JString&				prefix,
	const VisInfo&				i1,
	const VisInfo&				i2
	)
{
	assert( i1.fileIndex == 0 || i2.fileIndex == 0 );

	const JUtf8Byte* s1 =
		(i1.fileIndex == 0 ?
		 prefix.GetBytes() :
		 fileList.GetElement(i1.fileIndex)->GetBytes() + i1.nameIndex-1);

	const JUtf8Byte* s2 =
		(i2.fileIndex == 0 ?
		 prefix.GetBytes() :
		 fileList.GetElement(i2.fileIndex)->GetBytes() + i2.nameIndex-1);

	return JIntToWeakOrdering(JString::Compare(s1, s2, JString::kIgnoreCase));
}

void
JXFileListTable::InstallCompareWrapper
	(
	const JString& prefix
	)
	const
{
	itsVisibleList->SetCompareFunction([this, prefix](const VisInfo& i1, const VisInfo& i2)
	{
		return CompareWrapper(*itsFileList, prefix, i1, i2);
	});
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
	if (itsEditMenu != nullptr)
	{
		StopListening(itsEditMenu);
	}

	if (te != nullptr && te->GetEditMenu(&itsEditMenu))
	{
		itsEditMenuProvider = te;
		ListenTo(itsEditMenu);
	}
	else
	{
		itsEditMenuProvider = nullptr;
		itsEditMenu         = nullptr;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

#define ADJUST_INDEX(m) \
	const JSize count = itsVisibleList->GetElementCount(); \
	VisInfo* info     = const_cast<VisInfo*>(itsVisibleList->GetCArray()); \
	for (JUnsignedOffset i=0; i<count; i++) \
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
		const auto* m =
			dynamic_cast<const JListT::ElementsInserted*>(&message);
		assert( m != nullptr );
		ADJUST_INDEX(m)
	}
	else if (sender == itsFileList && message.Is(JListT::kElementsRemoved))
	{
		const auto* m =
			dynamic_cast<const JListT::ElementsRemoved*>(&message);
		assert( m != nullptr );

		const JSize count = itsVisibleList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
		{
			VisInfo info = itsVisibleList->GetElement(i);
			if (m->AdjustIndex(&info.fileIndex))
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
		const auto* m =
			dynamic_cast<const JListT::ElementMoved*>(&message);
		assert( m != nullptr );
		ADJUST_INDEX(m)
	}
	else if (sender == itsFileList && message.Is(JListT::kElementsSwapped))
	{
		const auto* m =
			dynamic_cast<const JListT::ElementsSwapped*>(&message);
		assert( m != nullptr );
		ADJUST_INDEX(m)
	}
	else if (sender == itsFileList && message.Is(JListT::kElementsChanged))
	{
		assert_msg( 0, "makes no sense since it's a JPtrArray" );
	}
	else if (sender == itsFileList && message.Is(JListT::kSorted))
	{
		assert_msg( 0, "not allowed" );
	}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateEditMenu();
	}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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
	JIndex index;
	if (HasSelection())
	{
		const bool ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index);
		assert( ok );
		itsEditMenu->EnableItem(index);
	}

	const bool ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index);
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

		auto* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != nullptr );

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
	const JString&		id
	)
{
	if (id == kSelectionDataID)
	{
		assert( HasSelection() );

		auto* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != nullptr );

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
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list from anybody but ourselves.

 ******************************************************************************/

bool
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
		return false;
	}

	// we accept drops of type text/uri-list

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	for (const auto type : typeList)
	{
		if (type == urlXAtom)
		{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return true;
		}
	}

	return false;
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
		for (auto* fileName : fileNameList)
		{
			AddFile(*fileName);
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
