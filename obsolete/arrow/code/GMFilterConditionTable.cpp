/******************************************************************************
 GMFilterConditionTable.cc

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GMFilterConditionTable.h>
#include <GMFilter.h>
#include <GMFilterCondition.h>

#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXWindow.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jXGlobals.h>

#include <jAssert.h>

const JCoordinate kDefRowHeight		= 20;
const JCoordinate kDefColWidth		= 1000;

const JCoordinate kTypeColWidth		= 80;
const JCoordinate kRelationColWidth	= 80;
const JCoordinate kPatternColWidth	= 200;

const JCoordinate kHMarginWidth	= 3;
const JCoordinate kRowHeightAdjust	= 2;

const JIndex kTypeColumn			= 1;
const JIndex kRelationColumn		= 2;
const JIndex kPatternColumn			= 3;

const JSize kGMTypeStringCount		= 8;
const JCharacter* kGMTypeStrings[]	=
	{"From", "To", "Any To", "Subject", "CC", "ReplyTo", "Any Header", "Body"};

const JSize kGMRelationStringCount	= 4;
const JCharacter* kGMRelationStrings[]	=
	{"contains", "is", "begins with", "ends with"};


/******************************************************************************
 Create (static)

 *****************************************************************************/

GMFilterConditionTable*
GMFilterConditionTable::Create
	(
	GMFilterDialog*	dialog,
	JXTextButton*		newButton,
	JXTextButton*		removeButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	GMFilterConditionTable* widget	=
		new GMFilterConditionTable(dialog, newButton, removeButton,
			scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h);
	assert(widget != NULL);
	widget->GMFilterConditionTableX();
	return widget;
}

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterConditionTable::GMFilterConditionTable
	(
	GMFilterDialog*	dialog,
	JXTextButton*		newButton,
	JXTextButton*		removeButton,
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
	JXEditTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsDialog(dialog),
	itsNewButton(newButton),
	itsRemoveButton(removeButton),
	itsInput(NULL),
	itsFilter(NULL),
	itsConditions(NULL)
{
}

/******************************************************************************
 GMFilterConditionTableX (private)

 ******************************************************************************/

void
GMFilterConditionTable::GMFilterConditionTableX()
{
	const JFontManager* fm	= GetFontManager();
	itsRowHeight	=
		fm->GetLineHeight(JGetDefaultFontName(), kJDefaultFontSize,
						JFontStyle()) + kRowHeightAdjust;

	itsTypeMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0,10,10);
	assert(itsTypeMenu != NULL);
	itsTypeMenu->Hide();
	itsTypeMenu->SetToHiddenPopupMenu(kJTrue);
	itsTypeMenu->CompressHeight();
	for (JIndex i = 0; i < kGMTypeStringCount; i++)
		{
		itsTypeMenu->AppendItem(kGMTypeStrings[i]);
		}
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTypeMenu);

	itsRelationMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0,10,10);
	assert(itsRelationMenu != NULL);
	itsRelationMenu->Hide();
	itsRelationMenu->SetToHiddenPopupMenu(kJTrue);
	itsRelationMenu->CompressHeight();
	for (JIndex i = 0; i < kGMRelationStringCount; i++)
		{
		itsRelationMenu->AppendItem(kGMRelationStrings[i]);
		}
	itsRelationMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsRelationMenu);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	AppendCols(1, kTypeColWidth);
	AppendCols(1, kRelationColWidth);
	AppendCols(1, kPatternColWidth);
	AdjustColWidths();
	AdjustButtons();
	WantInput(kJFalse);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterConditionTable::~GMFilterConditionTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMFilterConditionTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	assert(cell.y <= (JCoordinate)itsConditions->GetElementCount());

	HilightIfSelected(p, JPoint(1, cell.y), rect);

	GMFilterCondition* condition	= itsConditions->GetElement(cell.y);
	if (cell.x == (JCoordinate)kTypeColumn)
		{
		p.JPainter::String(rect, kGMTypeStrings[condition->GetType() - 1],
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == (JCoordinate)kRelationColumn)
		{
		p.JPainter::String(rect, kGMRelationStrings[condition->GetRelation() - 1],
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == (JCoordinate)kPatternColumn)
		{
		JRect r = rect;
		r.left += kHMarginWidth;
		p.JPainter::String(r, condition->GetPattern(),
						   JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}

}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GMFilterConditionTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		return;
		}

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}

	JTableSelection& s		= GetTableSelection();

	JPoint oldCell;
	JBoolean hasSelection	= s.GetFirstSelectedCell(&oldCell);
	if (!hasSelection ||
		!IsEditing() ||
		(cell.y != oldCell.y && OKToSwitch()))
		{
		s.ClearSelection();
		s.SelectCell(cell.y, 1);
		BeginEditing(JPoint(kPatternColumn, cell.y));
		}

	if (cell.x == (JCoordinate)kTypeColumn)
		{
		itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (cell.x == (JCoordinate)kRelationColumn)
		{
		itsRelationMenu->PopUp(this, pt, buttonStates, modifiers);
		}

	TableRefresh();
	AdjustButtons();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMFilterConditionTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		if (GetWindow()->OKToUnfocusCurrentWidget())
			{
			NewCondition();
			}
		}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		RemoveSelectedConditions();
		}
	else if (sender == itsTypeMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleTypeMenu(selection->GetIndex());
		}
	else if (sender == itsRelationMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleRelationMenu(selection->GetIndex());
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GMFilterConditionTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsInput	=
		new JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);
	itsInput->SetIsRequired(kJTrue);
	GMFilterCondition* condition	= itsConditions->GetElement(cell.y);
	itsInput->SetText(condition->GetPattern());
	return itsInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GMFilterConditionTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JBoolean ok = itsInput->InputValid();
	if (ok)
		{
		GMFilterCondition* condition	= itsConditions->GetElement(cell.y);
		condition->SetPattern(itsInput->GetText());
		}
	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GMFilterConditionTable::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMFilterConditionTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
GMFilterConditionTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorIndex color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth =
		GetColWidth(kTypeColumn) + GetColWidth(kRelationColumn) + 2 * lineWidth;

	if (apWidth > usedWidth)
		{
		SetColWidth(kPatternColumn, apWidth - usedWidth);
		}
	else
		{
		// I'll really need to adjust this relative to the size of the patterns.
		}
}

/******************************************************************************
 SetFilter (public)

 ******************************************************************************/

void
GMFilterConditionTable::SetFilter
	(
	GMFilter* filter
	)
{
	itsFilter	= filter;
	RemoveAllRows();

	if (filter != NULL)
		{
		itsConditions = filter->GetConditions();

		const JSize count	= itsConditions->GetElementCount();
		AppendRows(count, itsRowHeight);
		}
	else
		{
		itsConditions = NULL;
		}
	AdjustButtons();
	TableRefresh();
}

/******************************************************************************
 AdjustButtons (private)

 ******************************************************************************/

void
GMFilterConditionTable::AdjustButtons()
{
	if (itsFilter == NULL)
		{
		itsNewButton->Deactivate();
		}
	else
		{
		itsNewButton->Activate();
		}

	if (GetRowCount() == 0 ||
		!GetTableSelection().HasSelection() ||
		GetTableSelection().GetSelectedCellCount() == GetRowCount())
		{
		itsRemoveButton->Deactivate();
		}
	else
		{
		itsRemoveButton->Activate();
		}
}

/******************************************************************************
 OKToSwitch (public)

 ******************************************************************************/

JBoolean
GMFilterConditionTable::OKToSwitch()
{
	if (IsEditing())
		{
		return EndEditing();
		}
	return kJTrue;
}

/******************************************************************************
 NewCondition (private)

 ******************************************************************************/

void
GMFilterConditionTable::NewCondition()
{
	GMFilterCondition* condition = new GMFilterCondition();
	assert(condition != NULL);

	itsConditions->Append(condition);
	AppendRows(1, itsRowHeight);

	GetTableSelection().ClearSelection();
	GetTableSelection().SelectCell(GetRowCount(), 1);
	BeginEditing(JPoint(kPatternColumn, GetRowCount()));
	TableRefresh();

	AdjustButtons();
}

/******************************************************************************
 RemoveSelectedConditions (private)

 ******************************************************************************/

void
GMFilterConditionTable::RemoveSelectedConditions()
{
	JTableSelection& s	= GetTableSelection();
	assert(s.HasSelection());

	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell))
		{
		itsConditions->DeleteElement(cell.y);
		RemoveRow(cell.y);
		}
	AdjustButtons();
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
GMFilterConditionTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	if (!GetTableSelection().GetFirstSelectedCell(&cell))
		{
		return;
		}
	GMFilterCondition* condition = itsConditions->GetElement(cell.y);
	condition->SetType((GMFilterCondition::ConditionType)index);
	TableRefresh();
}

/******************************************************************************
 HandleRelationMenu (private)

 ******************************************************************************/

void
GMFilterConditionTable::HandleRelationMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	if (!GetTableSelection().GetFirstSelectedCell(&cell))
		{
		return;
		}
	GMFilterCondition* condition = itsConditions->GetElement(cell.y);
	condition->SetRelation((GMFilterCondition::ConditionRelation)index);
	TableRefresh();
}
