/******************************************************************************
 JXDirTable.cpp

	Displays a directory listing stored in a JDirInfo object.  To filter
	the displayed files, create a derived class of JDirInfo.

	BASE CLASS = JXTable

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXDirTable.h>
#include <JDirInfo.h>

#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <jXUtil.h>

#include <JTableSelection.h>
#include <JString.h>
#include <JFontManager.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>
#include <jx_folder_small.xpm>
#include <jx_folder_read_only_small.xpm>
#include <jx_folder_locked_small.xpm>
#include <jx_executable_small.xpm>
#include <jx_unknown_file_small.xpm>

const JCoordinate kIconWidth    = 20;
const JCoordinate kTextPadding  = 5;
const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const Time kDirUpdatePeriod = 1000;		// milliseconds

const JCharacter* JXDirTable::kFileDblClicked = "FileDblClicked::JXDirTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDirTable::JXDirTable
	(
	JDirInfo*			data,
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
	assert( data != NULL );
	itsDirInfo = data;

	itsActiveCells = jnew JRunArray<JBoolean>;
	assert( itsActiveCells != NULL );

	itsDirUpdateTask = jnew JXTimerTask(kDirUpdatePeriod);
	assert( itsDirUpdateTask != NULL );
	itsDirUpdateTask->Start();
	ListenTo(itsDirUpdateTask);

	itsAllowSelectFilesFlag      = kJTrue;
	itsAllowSelectMultipleFlag   = kJFalse;
	itsAllowDblClickInactiveFlag = kJFalse;
	itsSelectWhenChangePathFlag  = kJTrue;
	itsMaxStringWidth            = 0;
	itsReselectFlag              = kJFalse;

	itsKeyBuffer = jnew JString;
	assert( itsKeyBuffer != NULL );

	itsReselectNameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsReselectNameList != NULL );

	JXDisplay* d = GetDisplay();

	itsFileIcon = jnew JXImage(d, jx_plain_file_small);
	assert( itsFileIcon != NULL );

	itsFolderIcon = jnew JXImage(d, jx_folder_small);
	assert( itsFolderIcon != NULL );

	itsReadOnlyFolderIcon = jnew JXImage(d, jx_folder_read_only_small);
	assert( itsReadOnlyFolderIcon != NULL );

	itsLockedFolderIcon = jnew JXImage(d, jx_folder_locked_small);
	assert( itsLockedFolderIcon != NULL );

	itsExecIcon = jnew JXImage(d, jx_executable_small);
	assert( itsExecIcon != NULL );

	itsUnknownIcon = jnew JXImage(d, jx_unknown_file_small);
	assert( itsUnknownIcon != NULL );

	itsIgnoreSelChangesFlag = kJFalse;
	SetSelectionBehavior(itsAllowSelectMultipleFlag, kJTrue);

	const JIndex blackColor = GetColormap()->GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);
	AppendCols(1);
	SetDefaultRowHeight(GetFontManager()->GetDefaultFont().GetLineHeight() +
						2*kVMarginWidth);
	AdjustTableContents();
	ListenTo(itsDirInfo);
	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDirTable::~JXDirTable()
{
	jdelete itsActiveCells;
	jdelete itsDirUpdateTask;
	jdelete itsKeyBuffer;
	jdelete itsReselectNameList;
	jdelete itsFileIcon;
	jdelete itsFolderIcon;
	jdelete itsReadOnlyFolderIcon;
	jdelete itsLockedFolderIcon;
	jdelete itsExecIcon;
	jdelete itsUnknownIcon;
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
JXDirTable::GetPath()
	const
{
	return itsDirInfo->GetDirectory();
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

JBoolean
JXDirTable::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 HasSingleSelection

 ******************************************************************************/

JBoolean
JXDirTable::HasSingleSelection()
	const
{
	const JTableSelection& s = GetTableSelection();
	JPoint cell1, cell2;
	return JI2B(
		s.GetFirstSelectedCell(&cell1) &&
		s.GetLastSelectedCell(&cell2) &&
		cell1.y == cell2.y);
}

/******************************************************************************
 GetFirstSelection

 ******************************************************************************/

JBoolean
JXDirTable::GetFirstSelection
	(
	const JDirEntry** entry
	)
	const
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		*entry = &(itsDirInfo->GetEntry(cell.y));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

JBoolean
JXDirTable::GetSelection
	(
	JPtrArray<JDirEntry>* entryList
	)
	const
{
	entryList->CleanOut();

	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	while (iter.Next(&cell))
		{
		entryList->Append(const_cast<JDirEntry*>(&(itsDirInfo->GetEntry(cell.y))));
		}

	entryList->SetCleanUpAction(JPtrArrayT::kForgetAll);
	return !entryList->IsEmpty();
}

/******************************************************************************
 SelectSingleEntry

 ******************************************************************************/

JBoolean
JXDirTable::SelectSingleEntry
	(
	const JIndex	index,
	const JBoolean	scroll
	)
{
	if (ItemIsActive(index))
		{
		SelectSingleCell(JPoint(1, index), scroll);
		itsKeyBuffer->Clear();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SelectFirstEntry

 ******************************************************************************/

void
JXDirTable::SelectFirstEntry
	(
	const JBoolean scroll
	)
{
	JIndex index;
	if (GetNextSelectable(0, kJFalse, &index))
		{
		SelectSingleEntry(index, scroll);
		}
}

/******************************************************************************
 SelectLastEntry

 ******************************************************************************/

void
JXDirTable::SelectLastEntry
	(
	const JBoolean scroll
	)
{
	JIndex index;
	if (GetPrevSelectable(GetRowCount()+1, kJFalse, &index))
		{
		SelectSingleEntry(index, scroll);
		}
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JXDirTable::SelectAll()
{
	(GetTableSelection()).SelectAll();
}

/******************************************************************************
 CleanSelection (private)

	Only active files are allowed in a multiple selection.

 ******************************************************************************/

void
JXDirTable::CleanSelection()
{
	if (itsIgnoreSelChangesFlag)
		{
		return;
		}

	itsIgnoreSelChangesFlag = kJTrue;

	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);

	const JBoolean forMulti = !HasSingleSelection();

	JPoint cell;
	while (iter.Next(&cell))
		{
		if (!ItemIsSelectable(cell.y, forMulti))
			{
			s.SelectCell(cell, kJFalse);
			}
		}

	itsKeyBuffer->Clear();
	itsIgnoreSelChangesFlag = kJFalse;
}

/******************************************************************************
 IsSelectable (virtual)

 ******************************************************************************/

JBoolean
JXDirTable::IsSelectable
	(
	const JPoint&	cell,
	const JBoolean	forExtend
	)
	const
{
	return ItemIsSelectable(cell.y, forExtend);
}

/******************************************************************************
 ShowHidden

 ******************************************************************************/

void
JXDirTable::ShowHidden
	(
	const JBoolean showHidden
	)
{
	itsDirInfo->ShowHidden(showHidden);
}

/******************************************************************************
 AllowSelectFiles

 ******************************************************************************/

void
JXDirTable::AllowSelectFiles
	(
	const JBoolean allowSelectFiles,
	const JBoolean allowMultiple
	)
{
	itsAllowSelectMultipleFlag = allowMultiple;
	SetSelectionBehavior(itsAllowSelectMultipleFlag, kJTrue);

	if (allowSelectFiles != itsAllowSelectFilesFlag)
		{
		itsAllowSelectFilesFlag = allowSelectFiles;
		AdjustTableContents();
		}

	if (!itsAllowSelectMultipleFlag && !HasSingleSelection())
		{
		(GetTableSelection()).ClearSelection();
		}
}

/******************************************************************************
 GoToSelectedDirectory

	If there is a single item selected, and it is a directory, we switch to
	it and return kJTrue.

 ******************************************************************************/

JBoolean
JXDirTable::GoToSelectedDirectory()
{
	const JDirEntry* entry;
	if (HasSingleSelection() && GetFirstSelection(&entry))
		{
		JError err = JNoError();

		if (entry->GetName() == "..")
			{
			err = itsDirInfo->GoUp();
			}
		else if (entry->IsDirectory())
			{
			// need local copy because GoDown() deletes entry

			const JString dir = entry->GetName();
			err               = itsDirInfo->GoDown(dir);
			}
		else
			{
			return kJFalse;
			}

		err.ReportIfError();
		return err.OK();
		}

	return kJFalse;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXDirTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	const JDirEntry& entry = itsDirInfo->GetEntry(cell.y);
	HilightIfSelected(p, cell, rect);

	// draw icon

	JXImage* icon = NULL;
	if (entry.IsDirectory())
		{
		if (!entry.IsReadable())
			{
			icon = itsLockedFolderIcon;
			}
		else if (!entry.IsWritable())
			{
			icon = itsReadOnlyFolderIcon;
			}
		else
			{
			icon = itsFolderIcon;
			}
		}
	else if (entry.IsFile())
		{
		if (entry.IsExecutable())
			{
			icon = itsExecIcon;
			}
		else
			{
			icon = itsFileIcon;
			}
		}
	else if (entry.IsUnknown())
		{
		icon = itsUnknownIcon;
		}

	if (icon != NULL)
		{
		JRect r = rect;
		r.right = r.left + kIconWidth;
		p.Image(*icon, icon->GetBounds(), r);
		}

	// draw name

	const JBoolean italic = entry.IsLink();

	JColorIndex color = GetColormap()->GetBlackColor();
	if (!ItemIsActive(cell.y))
		{
		color = GetColormap()->GetGrayColor(40);
		}

	JFont font = GetWindow()->GetFontManager()->GetDefaultFont();
	font.SetStyle(JFontStyle(kJFalse, italic, 0, kJFalse, color));
	p.SetFont(font);

	JRect r = rect;
	r.left += kIconWidth + kHMarginWidth;
	p.String(r, entry.GetName(), JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXDirTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsKeyBuffer->Clear();
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		}
	else if (button == kJXLeftButton && clickCount == 2)
		{
		s.SetBoat(cell);
		s.SetAnchor(cell);
		HandleDoubleClick(cell.y);		// can jdelete us
		return;
		}
	else
		{
		BeginSelectionDrag(cell, button, modifiers);
		}

	// anything here also needs to be done before HandleDoubleClick()
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXDirTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ContinueSelectionDrag(pt, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXDirTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishSelectionDrag();
}

/******************************************************************************
 HandleDoubleClick (private)

 ******************************************************************************/

void
JXDirTable::HandleDoubleClick
	(
	const JIndex index
	)
{
	const JDirEntry& entry = itsDirInfo->GetEntry(index);
	if (ItemIsActive(index))
		{
		if (entry.IsDirectory())
			{
			GoToSelectedDirectory();
			}
		else
			{
			Broadcast(FileDblClicked(entry, kJTrue));
			}
		}
	else if (itsAllowDblClickInactiveFlag && entry.IsFile())
		{
		Broadcast(FileDblClicked(entry, kJFalse));
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXDirTable::HandleFocusEvent()
{
	JXTable::HandleFocusEvent();
	itsKeyBuffer->Clear();
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
JXDirTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s             = GetTableSelection();
	const JBoolean hadSelection    = s.GetFirstSelectedCell(&topSelCell);
	const JBoolean extendSelection = JI2B( modifiers.shift() && itsAllowSelectMultipleFlag );

	if (key == ' ')
		{
		itsKeyBuffer->Clear();
		s.ClearSelection();
		}

	else if (key == kJReturnKey)
		{
		if (hadSelection)
			{
			HandleDoubleClick(topSelCell.y);	// can jdelete us
			return;
			}
		}

	else if (key == kJUpArrow)
		{
		itsKeyBuffer->Clear();
		if (modifiers.meta())
			{
			const JError err = itsDirInfo->GoUp();
			err.ReportIfError();
			}
		else if (!hadSelection)
			{
			SelectLastEntry();		// last item might be inactive
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}

	else if (key == kJDownArrow)
		{
		itsKeyBuffer->Clear();
		if (modifiers.meta())
			{
			GoToSelectedDirectory();
			}
		else if (!hadSelection)
			{
			SelectFirstEntry();		// first item might be inactive
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		}

	else if (key == kJLeftArrow)
		{
		const JError err = itsDirInfo->GoUp();
		err.ReportIfError();
		}

	else if (key == kJRightArrow)
		{
		GoToSelectedDirectory();
		}

	else if (itsAllowSelectMultipleFlag &&
			 (key == 'a' || key == 'A') && modifiers.meta() && !modifiers.shift())
		{
		SelectAll();
		}

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer->AppendCharacter(key);

		JIndex index;
		if (ClosestMatch(*itsKeyBuffer, &index))
			{
			JString saveBuffer = *itsKeyBuffer;
			SelectSingleEntry(index);
			*itsKeyBuffer = saveBuffer;
			}
		}

	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}

	// anything here also needs to be done before HandleDoubleClick()
}

/******************************************************************************
 InstallShortcuts

 ******************************************************************************/

void
JXDirTable::InstallShortcuts()
{
	JXKeyModifiers modifiers(GetDisplay());
	modifiers.SetState(kJXMetaKeyIndex, kJTrue);

	JXWindow* window = GetWindow();
	window->InstallShortcut(this, kJUpArrow, modifiers);
	window->InstallShortcut(this, kJDownArrow, modifiers);
}

/******************************************************************************
 HandleShortcut

	Alt-up and Alt-down are handled as if we had focus.

 ******************************************************************************/

void
JXDirTable::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if ((key == kJUpArrow || key == kJDownArrow) && modifiers.meta())
		{
		if (Focus())
			{
			HandleKeyPress(key, modifiers);
			}
		}
	else
		{
		JXTable::HandleShortcut(key, modifiers);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list from anybody but ourselves.

 ******************************************************************************/

JBoolean
JXDirTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	// dropping on ourselves makes no sense

	if (this == const_cast<JXWidget*>(source))
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
JXDirTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						time, selMgr->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == selMgr->GetURLXAtom())
			{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);

			const JSize fileCount = fileNameList.GetElementCount();
			if (fileCount > 0)
				{
				const JString* entryName = fileNameList.FirstElement();
				JString path, name;
				if (JDirectoryExists(*entryName))
					{
					path = *entryName;
					}
				else if (JFileExists(*entryName))
					{
					JSplitPathAndName(*entryName, &path, &name);
					}

				const JError err = itsDirInfo->GoTo(path);
				err.ReportIfError();
				if (err.OK() && !name.IsEmpty())
					{
					JTableSelection& s = GetTableSelection();
					s.ClearSelection();
					for (JIndex i=1; i<=fileCount; i++)
						{
						entryName = fileNameList.NthElement(i);
						if (JFileExists(*entryName))
							{
							JSplitPathAndName(*entryName, &path, &name);
							JIndex index;
							if (itsDirInfo->FindEntry(name, &index) &&
								ItemIsActive(index))
								{
								GetWindow()->Raise();
								if (!s.HasSelection())
									{
									const JBoolean ok = SelectSingleEntry(index);
									assert( ok );
									}
								else
									{
									s.SelectRow(index);
									s.ClearBoat();
									s.ClearAnchor();
									}
								if (!itsAllowSelectMultipleFlag)
									{
									break;
									}
								}
							}
						}
					}
				}
			}

		selMgr->DeleteData(&data, delMethod);
		}
}

/******************************************************************************
 AdjustTableContents (private)

 ******************************************************************************/

void
JXDirTable::AdjustTableContents()
{
	itsKeyBuffer->Clear();

	// adjust the number of rows and the width of the text column

	const JFontManager* fontMgr = GetFontManager();

	RemoveAllRows();
	itsMaxStringWidth = 0;

	const JSize count = itsDirInfo->GetElementCount();
	AppendRows(count);

	const JFont& font = fontMgr->GetDefaultFont();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = itsDirInfo->GetEntry(i);
		const JSize w          = font.GetStringWidth(entry.GetName());
		if (w > itsMaxStringWidth)
			{
			itsMaxStringWidth = w;
			}
		}

	AdjustColWidths();

	// deactivate unselectable items

	itsActiveCells->RemoveAll();
	if (count > 0)
		{
		itsActiveCells->AppendElements(kJTrue, count);

		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry  = itsDirInfo->GetEntry(i);
			if (entry.IsBrokenLink() ||
				(entry.IsDirectory() && (!entry.IsReadable() || !entry.IsExecutable())) ||
				(entry.IsFile() && (!entry.IsReadable() || !itsAllowSelectFilesFlag)))
				{
				itsActiveCells->SetElement(i, kJFalse);
				}
			}
		}

	// select appropriate items

	JBoolean deselect  = kJFalse;
	JTableSelection& s = GetTableSelection();
	if (itsReselectFlag && !itsReselectNameList->IsEmpty())
		{
		// select the items that still exist

		const JSize count = itsReselectNameList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JIndex j;
			if (itsDirInfo->FindEntry(*(itsReselectNameList->NthElement(i)), &j) &&
				ItemIsActive(j))	// check for active in case of single, broken link
				{
				s.SelectRow(j);
				}
			}

		if (!HasSelection())
			{
			// select the closest item

			JIndex i;
			if (ClosestMatch(*(itsReselectNameList->FirstElement()), &i))
				{
				s.SelectRow(i);

				// If it is not an exact match, deselect it after scrolling to it.

				if ((itsDirInfo->GetEntry(i)).GetName() !=
					*(itsReselectNameList->FirstElement()))
					{
					deselect = kJTrue;
					}
				}
			}
		}

	if (HasSelection())
		{
		// keep display from jumping

		JPoint cell;
		const JBoolean hasSelection = s.GetFirstSelectedCell(&cell);
		assert( hasSelection );
		const JRect selRect   = GetCellRect(cell);
		const JPoint scrollPt = selRect.topLeft() + itsReselectScrollOffset;
		ScrollTo(scrollPt);
		}
	else if (!itsReselectFlag)
		{
		SelectFirstEntry();
		}

	if (deselect)
		{
		s.ClearSelection();
		}

	itsReselectFlag = kJFalse;
	itsReselectNameList->CleanOut();
}

/******************************************************************************
 GetNextSelectable (protected)

	startIndex can be zero.

 ******************************************************************************/

JBoolean
JXDirTable::GetNextSelectable
	(
	const JIndex	startIndex,
	const JBoolean	forMulti,
	JIndex*			nextIndex
	)
	const
{
	const JSize rowCount = GetRowCount();
	if (startIndex >= rowCount)
		{
		return kJFalse;
		}

	JIndex i = startIndex + 1;
	while (i <= rowCount && !ItemIsSelectable(i, forMulti))
		{
		i++;
		}

	*nextIndex = i;
	return JI2B( i <= rowCount );
}

/******************************************************************************
 GetPrevSelectable (protected)

	startIndex can be count+1.

 ******************************************************************************/

JBoolean
JXDirTable::GetPrevSelectable
	(
	const JIndex	startIndex,
	const JBoolean	forMulti,
	JIndex*			nextIndex
	)
	const
{
	if (startIndex <= 1)
		{
		return kJFalse;
		}

	JIndex i = startIndex - 1;
	while (i >= 1 && !ItemIsSelectable(i, forMulti))
		{
		i--;
		}

	*nextIndex = i;
	return JI2B( i >= 1 );
}

/******************************************************************************
 ItemIsFile (protected)

 ******************************************************************************/

JBoolean
JXDirTable::ItemIsFile
	(
	const JIndex index
	)
	const
{
	return (itsDirInfo->GetEntry(index)).IsFile();
}

/******************************************************************************
 ClosestMatch

	Returns kJFalse if nothing can be selected.

 ******************************************************************************/

JBoolean
JXDirTable::ClosestMatch
	(
	const JCharacter*	prefixStr,
	JIndex*				index
	)
	const
{
	JBoolean found = itsDirInfo->ClosestMatch(prefixStr, index);

	if (found && !ItemIsActive(*index))
		{
		found = GetNextSelectable(*index, kJFalse, index);
		if (!found)
			{
			found = GetPrevSelectable(GetRowCount()+1, kJFalse, index);
			}
		}

	return found;
}

/******************************************************************************
 UpdateDisplay

	Called by our idle task.

 ******************************************************************************/

void
JXDirTable::UpdateDisplay()
{
	itsDirInfo->Update();
}

/******************************************************************************
 Receive (virtual protected)

	This provides a single entry point to update the display, regardless
	of what triggered the update.

 ******************************************************************************/

void
JXDirTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDirInfo && message.Is(JDirInfo::kContentsWillBeUpdated))
		{
		RememberSelections();
		}
	else if (sender == itsDirInfo && message.Is(JDirInfo::kPathChanged))
		{
		// This comes after ContentsChanged, and cancels the effect of
		// ContentsWillBeUpdated.

		itsReselectFlag = kJFalse;
		itsReselectNameList->CleanOut();
		if (itsSelectWhenChangePathFlag)
			{
			SelectFirstEntry();
			}
		else
			{
			(GetTableSelection()).ClearSelection();
			}
		}

	else if (sender == itsDirInfo && message.Is(JDirInfo::kContentsChanged))
		{
		AdjustTableContents();
		}

	else if (sender == itsDirUpdateTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		UpdateDisplay();
		}

	else
		{
		if (sender == &(GetTableSelection()) && !itsIgnoreSelChangesFlag)
			{
			CleanSelection();
			}

		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 RememberSelections (private)

 ******************************************************************************/

void
JXDirTable::RememberSelections()
{
	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	if (!itsReselectFlag && GetSelection(&entryList))
		{
		itsReselectFlag = kJTrue;

		const JSize count = entryList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsReselectNameList->Append((entryList.NthElement(i))->GetName());
			}

		JPoint cell;
		const JBoolean hasSelection = (GetTableSelection()).GetFirstSelectedCell(&cell);
		assert( hasSelection );
		const JRect selRect     = GetCellRect(cell);
		itsReselectScrollOffset = (GetAperture()).topLeft() - selRect.topLeft();
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXDirTable::ApertureResized
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
JXDirTable::AdjustColWidths()
{
	const JCoordinate minWidth = kIconWidth + itsMaxStringWidth + kTextPadding;
	SetColWidth(1, JMax(minWidth, GetApertureWidth()));
}
