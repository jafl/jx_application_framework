/******************************************************************************
 JXFSBindingTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JXFSBindingTable.h"
#include "JFSBindingList.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXInputField.h>
#include <JXColHeaderWidget.h>
#include <jXConstants.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JRegex.h>
#include <jASCIIConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kPatternColumn = 1,
	kCommandColumn,
	kTypeColumn,
	kSingleFileColumn
};

const JCoordinate kInitColWidth[] =
{
	80, 100, 50, 85
};

static const JCharacter* kColTitle[] =
{
	"File type",
	"Command",
	"Type",
	"One at a time"
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// Type menu

static const JCharacter* kTypeMenuStr =
	"  Plain %r"
	"| Shell %r"
	"| Window %r";

enum
{
	kPlainTypeCmd = 1,
	kShellTypeCmd,
	kWindowTypeCmd
};

static const JFSBinding::CommandType kMenuIndexToCmdType[] =
{
	JFSBinding::kRunPlain,
	JFSBinding::kRunInShell,
	JFSBinding::kRunInWindow
};

static const JIndex kCmdTypeToMenuIndex[] =
{
	kPlainTypeCmd,
	kShellTypeCmd,
	kWindowTypeCmd
};

// JBroadcaster messages

const JCharacter* JXFSBindingTable::kDataChanged = "JXFSBindingTable::kDataChanged";

// string ID's

static const JCharacter* kCantRemoveSystemBindingID = "CantRemoveSystemBinding::JXFSBindingTable";
static const JCharacter* kReplacedBySystemID        = "ReplacedBySystem::JXFSBindingTable";
static const JCharacter* kPatternUsedID             = "PatternUsed::JXFSBindingTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSBindingTable::JXFSBindingTable
	(
	JFSBindingList*		list,
	JXTextButton*		addButton,
	JXTextButton*		removeButton,
	JXTextButton*		duplicateButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsBindingList(list),
	itsAddButton(addButton),
	itsRemoveButton(removeButton),
	itsDuplicateButton(duplicateButton),
	itsTextInput(nullptr)
{
	// row height

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		GetFontManager()->GetDefaultFont().GetLineHeight(),
		GetFontManager()->GetDefaultMonospaceFont().GetLineHeight());
	SetDefaultRowHeight(rowHeight);

	// buttons

	ListenTo(itsAddButton);
	ListenTo(itsRemoveButton);
	ListenTo(itsDuplicateButton);

	// type menu

	itsTypeMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsTypeMenu != nullptr );
	itsTypeMenu->Hide();
	itsTypeMenu->SetToHiddenPopupMenu(kJTrue);
	itsTypeMenu->SetMenuItems(kTypeMenuStr);
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTypeMenu);

	// regex for testing

	itsTestRegex = jnew JRegex;
	assert( itsTestRegex != nullptr );

	// data

	for (JIndex i=1; i<=kColCount; i++)
		{
		AppendCols(1, kInitColWidth[i-1]);
		}
	UpdateColWidths();

	const JSize rowCount = itsBindingList->GetElementCount();
	AppendRows(rowCount);

	UpdateButtons();
	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSBindingTable::~JXFSBindingTable()
{
	jdelete itsTestRegex;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXFSBindingTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
		{
		return;
		}

	HilightIfSelected(p, cell, rect);

	const JFSBinding* b = itsBindingList->GetBinding(cell.y);
	JFSBinding::CommandType type;
	JBoolean singleFile;
	const JString& cmd = b->GetCommand(&type, &singleFile);

	if (cell.x == kPatternColumn)
		{
		p.SetFont(GetFontManager()->GetDefaultMonospaceFont());

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, b->GetPattern(), JPainter::kHAlignLeft, JPainter::kVAlignCenter);

		p.SetFont(GetFontManager()->GetDefaultFont());
		}
	else if (cell.x == kCommandColumn)
		{
		p.SetFont(GetFontManager()->GetDefaultMonospaceFont());

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, cmd, JPainter::kHAlignLeft, JPainter::kVAlignCenter);

		p.SetFont(GetFontManager()->GetDefaultFont());
		}
	else if (cell.x == kTypeColumn)
		{
		const JString& str = itsTypeMenu->GetItemText(kCmdTypeToMenuIndex[type]);
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kSingleFileColumn && singleFile)
		{
		JRect r;
		r.top    = rect.ycenter();
		r.left   = rect.xcenter();
		r.bottom = r.top+1;
		r.right  = r.left+1;
		r.Expand(3, 3);

		p.SetFilling(kJTrue);
		p.Ellipse(r);
		p.SetFilling(kJFalse);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXFSBindingTable::HandleMouseDown
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JSize clickCount,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
		{
		s.SelectCell(cell);
		TableScrollToCell(cell);

		if (cell.x == kPatternColumn || cell.x == kCommandColumn)
			{
			BeginEditing(cell);
			}
		else if (cell.x == kTypeColumn)
			{
			itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else if (cell.x == kSingleFileColumn)
			{
			itsBindingList->ToggleSingleFile(cell.y);
			TableRefreshRow(cell.y);
			Broadcast(DataChanged());
			}
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXFSBindingTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (GetEditedCell(&cell) && cell.x == kPatternColumn &&
		(key == kJTabKey ||
		 (modifiers.meta() && (key == kJRightArrow || key == '6'))))
		{
		if (EndEditing() &&
			(GetTableSelection()).GetSingleSelectedCell(&cell))
			{
			BeginEditing(JPoint(kCommandColumn, cell.y));
			}
		}

	else
		{
		JXEditTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

JBoolean
JXFSBindingTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JI2B(cell.x == kPatternColumn || cell.x == kCommandColumn);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXFSBindingTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != nullptr );

	const JFSBinding* b = itsBindingList->GetBinding(cell.y);
	if (cell.x == kPatternColumn)
		{
		itsTextInput->SetText(b->GetPattern());
		}
	else if (cell.x == kCommandColumn)
		{
		JFSBinding::CommandType type;
		JBoolean singleFile;
		const JString& cmd = b->GetCommand(&type, &singleFile);
		itsTextInput->SetText(cmd);
		}

	itsTextInput->SetFont(GetFontManager()->GetDefaultMonospaceFont());
	itsTextInput->SetIsRequired();
	ListenTo(itsTextInput);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
JXFSBindingTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	const JString& s = itsTextInput->GetText();
	JBoolean ok      = itsTextInput->InputValid();

	if (ok && cell.x == kPatternColumn)
		{
		const JFSBinding* b    = itsBindingList->GetBinding(cell.y);
		const JBoolean changed = JI2B(b->GetPattern() != s);

		if (JFSBinding::WillBeRegex(s))
			{
			const JError err = itsTestRegex->SetPattern(s);
			err.ReportIfError();
			ok = err.OK();
			}

		JIndex newIndex;
		if (ok && changed && itsBindingList->SetPattern(cell.y, s, &newIndex))
			{
			JTableSelection& s = GetTableSelection();
			s.ClearSelection();
			s.SelectCell(newIndex, kPatternColumn);
			Broadcast(DataChanged());
			}
		else if (ok && changed)
			{
			ok = kJFalse;

			const JCharacter* map[] =
				{
				"pattern", s.GetCString()
				};
			const JString msg = JGetString(kPatternUsedID, map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			}
		}

	else if (ok && cell.x == kCommandColumn &&
			 itsBindingList->SetCommand(cell.y, s))
		{
		Broadcast(DataChanged());
		}

	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JXFSBindingTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;

	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		const JFSBinding* b = itsBindingList->GetBinding(cell.y);
		JFSBinding::CommandType type;
		JBoolean singleFile;
		if ((b->GetPattern()).IsEmpty() ||
			(b->GetCommand(&type, &singleFile)).IsEmpty())
			{
			RemovePattern();
			}
		}

	UpdateButtons();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSBindingTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddButton && message.Is(JXButton::kPushed))
		{
		AddPattern();
		}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		RemovePattern();
		}
	else if (sender == itsDuplicateButton && message.Is(JXButton::kPushed))
		{
		DuplicatePattern();
		}

	else if (sender == itsTypeMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTypeMenu();
		}
	else if (sender == itsTypeMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleTypeMenu(selection->GetIndex());
		}

	else
		{
		if (sender == &(GetTableSelection()))
			{
			UpdateButtons();
			}
		else if (message.Is(JTextEditor::kTextChanged))
			{
			Broadcast(DataChanged());
			}

		JXEditTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddPattern (private)

 ******************************************************************************/

void
JXFSBindingTable::AddPattern()
{
	if (EndEditing())
		{
		const JFSBinding* b = itsBindingList->GetDefaultCommand();
		JFSBinding::CommandType type;
		JBoolean singleFile;
		const JString& cmd = b->GetCommand(&type, &singleFile);

		const JIndex rowIndex =
			itsBindingList->AddBinding("", cmd, type, singleFile);
		InsertRows(rowIndex, 1);
		BeginEditing(JPoint(kPatternColumn, rowIndex));
		Broadcast(DataChanged());
		}
}

/******************************************************************************
 RemovePattern (private)

 ******************************************************************************/

void
JXFSBindingTable::RemovePattern()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		if ((itsBindingList->GetBinding(cell.y))->IsSystemBinding())
			{
			JGetUserNotification()->ReportError(JGetString(kCantRemoveSystemBindingID));
			}
		else
			{
			CancelEditing();
			if (itsBindingList->DeleteBinding(cell.y))
				{
				RemoveRow(cell.y);
				}
			else
				{
				TableRefreshRow(cell.y);
				GetWindow()->Update();
				(JGetUserNotification())->DisplayMessage(JGetString(kReplacedBySystemID));
				}
			UpdateButtons();
			Broadcast(DataChanged());
			}
		}
}

/******************************************************************************
 DuplicatePattern (private)

 ******************************************************************************/

void
JXFSBindingTable::DuplicatePattern()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell) && EndEditing())
		{
		const JFSBinding* b = itsBindingList->GetBinding(cell.y);

		JFSBinding::CommandType type;
		JBoolean singleFile;
		const JString& cmd = b->GetCommand(&type, &singleFile);

		const JIndex rowIndex =
			itsBindingList->AddBinding("", cmd, type, singleFile);
		InsertRows(rowIndex, 1);
		BeginEditing(JPoint(kPatternColumn, rowIndex));
		Broadcast(DataChanged());
		}
}

/******************************************************************************
 UpdateTypeMenu (private)

 ******************************************************************************/

void
JXFSBindingTable::UpdateTypeMenu()
{
	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const JFSBinding* b = itsBindingList->GetBinding(cell.y);
	itsTypeMenu->CheckItem(kCmdTypeToMenuIndex[ b->GetCommandType() ]);
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
JXFSBindingTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const JFSBinding::CommandType newType = kMenuIndexToCmdType [ index-1 ];
	if (itsBindingList->SetCommandType(cell.y, newType))
		{
		TableRefreshRow(cell.y);
		Broadcast(DataChanged());
		}
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
JXFSBindingTable::UpdateButtons()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		itsRemoveButton->SetActive(
			!(itsBindingList->GetBinding(cell.y))->IsSystemBinding());
		itsDuplicateButton->Activate();
		}
	else
		{
		itsRemoveButton->Deactivate();
		itsDuplicateButton->Deactivate();
		}
}

/******************************************************************************
 SyncWithBindingList

 ******************************************************************************/

void
JXFSBindingTable::SyncWithBindingList()
{
	CancelEditing();

	const JSize dataRowCount = itsBindingList->GetElementCount();
	if (GetRowCount() < dataRowCount)
		{
		AppendRows(dataRowCount - GetRowCount());
		}
	else if (GetRowCount() > dataRowCount)
		{
		RemoveNextRows(dataRowCount+1, GetRowCount() - dataRowCount);
		}

	UpdateButtons();
	Refresh();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXFSBindingTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw, dh);
	UpdateColWidths();
}

/******************************************************************************
 UpdateColWidths (private)

 ******************************************************************************/

void
JXFSBindingTable::UpdateColWidths()
{
	JCoordinate borderWidth;
	JColorID color;
	GetColBorderInfo(&borderWidth, &color);

	JCoordinate w = GetApertureWidth();
	for (JIndex i=1; i<=kColCount; i++)
		{
		if (i != kCommandColumn)
			{
			w -= GetColWidth(i) + borderWidth;
			}
		}

	w = JMax(w, kInitColWidth[ kCommandColumn-1 ]);
	SetColWidth(kCommandColumn, w);
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
JXFSBindingTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		widget->SetColTitle(i, kColTitle[i-1]);
		}
}
