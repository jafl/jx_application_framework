/******************************************************************************
 GMFilterActionTable.cc

	BASE CLASS = public JXEditTable

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterActionTable.h>
#include <GMFilter.h>
#include <GMFilterAction.h>

#include <gMailUtils.h>

#include <JXChooseSaveFile.h>
#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXWindow.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jXGlobals.h>

#include <jDirUtil.h>
#include <jAssert.h>

const JCoordinate kDefRowHeight		= 20;
const JCoordinate kDefColWidth		= 1000;

const JCoordinate kTypeColWidth		= 80;
const JCoordinate kCopyColWidth		= 80;
const JCoordinate kDestColWidth		= 200;

const JCoordinate kHMarginWidth	= 3;
const JCoordinate kRowHeightAdjust	= 2;

const JIndex kTypeColumn			= 1;
const JIndex kCopyColumn			= 2;
const JIndex kDestinationColumn		= 3;

/******************************************************************************
 Create (static)

 *****************************************************************************/

GMFilterActionTable*
GMFilterActionTable::Create
	(
	GMFilterDialog*	dialog,
	JXTextButton*		newButton,
	JXTextButton*		removeButton,
	JXTextButton*		chooseButton,
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
	GMFilterActionTable* widget	=
		new GMFilterActionTable(dialog, newButton, removeButton, chooseButton,
			scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h);
	assert(widget != NULL);
	widget->GMFilterActionTableX();
	return widget;
}

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterActionTable::GMFilterActionTable
	(
	GMFilterDialog*	dialog,
	JXTextButton*		newButton,
	JXTextButton*		removeButton,
	JXTextButton*		chooseButton,
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
	itsChooseButton(chooseButton),
	itsInput(NULL),
	itsFilter(NULL),
	itsActions(NULL)
{
}

/******************************************************************************
 GMFilterActionTableX (private)

 ******************************************************************************/

void
GMFilterActionTable::GMFilterActionTableX()
{
	const JFontManager* fm	= GetFontManager();
	itsRowHeight	=
		fm->GetLineHeight(JGetDefaultFontName(), kJXDefaultFontSize,
						JFontStyle()) + kRowHeightAdjust;
	AppendCols(1, kTypeColWidth);
	AppendCols(1, kCopyColWidth);
	AppendCols(1, kDestColWidth);
	AdjustColWidths();
	AdjustButtons();
	WantInput(kJFalse);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);
	ListenTo(itsChooseButton);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterActionTable::~GMFilterActionTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMFilterActionTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	assert(cell.y <= (JCoordinate)itsActions->GetElementCount());

	HilightIfSelected(p, JPoint(1, cell.y), rect);

	GMFilterAction* action	= itsActions->NthElement(cell.y);
	if (cell.x == (JCoordinate)kTypeColumn)
		{
		JString str;
		if (action->IsMailbox())
			{
			str	= "Transfer";
			}
		else
			{
			str	= "Forward";
			}
		p.JPainter::String(rect, str,
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == (JCoordinate)kCopyColumn)
		{
		JString str;
		if (action->IsCopying())
			{
			str	= "Yes";
			}
		else
			{
			str	= "No";
			}
		p.JPainter::String(rect, str,
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == (JCoordinate)kDestinationColumn)
		{
		JRect r = rect;
		r.left += kHMarginWidth;
		p.JPainter::String(r, action->GetDestination(),
						   JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}

}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GMFilterActionTable::HandleMouseDown
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
		BeginEditing(JPoint(kDestinationColumn, cell.y));
		}

	if (cell.x == (JCoordinate)kTypeColumn)
		{
		GMFilterAction* action	= itsActions->NthElement(cell.y);
		action->ShouldBeMailbox(!action->IsMailbox());
		}
	else if (cell.x == (JCoordinate)kCopyColumn &&
			 cell.y == (JCoordinate)GetRowCount())
		{
		GMFilterAction* action	= itsActions->NthElement(cell.y);
		action->ShouldCopy(!action->IsCopying());
		}

	TableRefresh();
	AdjustButtons();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMFilterActionTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		if (GetWindow()->OKToUnfocusCurrentWidget())
			{
			NewAction();
			}
		}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		RemoveSelectedActions();
		}
	else if (sender == itsChooseButton && message.Is(JXButton::kPushed))
		{
		ChooseMailbox();
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GMFilterActionTable::CreateXInputField
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
	GMFilterAction* action	= itsActions->NthElement(cell.y);
	itsInput->SetText(action->GetDestination());
	return itsInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GMFilterActionTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JBoolean ok = itsInput->InputValid();
	if (ok)
		{
		GMFilterAction* action	= itsActions->NthElement(cell.y);
		action->SetDestination(itsInput->GetText());
		}
	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GMFilterActionTable::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMFilterActionTable::ApertureResized
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
GMFilterActionTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();

	JCoordinate lineWidth;
	JColorIndex color;
	GetColBorderInfo(&lineWidth, &color);
	const JSize usedWidth =
		GetColWidth(kTypeColumn) + GetColWidth(kCopyColumn) + 2 * lineWidth;

	if (apWidth > usedWidth)
		{
		SetColWidth(kDestinationColumn, apWidth - usedWidth);
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
GMFilterActionTable::SetFilter
	(
	GMFilter* filter
	)
{
	itsFilter	= filter;
	RemoveAllRows();

	if (filter != NULL)
		{
		itsActions = filter->GetActions();

		const JSize count	= itsActions->GetElementCount();
		AppendRows(count, itsRowHeight);

		if (GetRowCount() == 0)
			{
			NewAction();
			}
		}
	else
		{
		itsActions = NULL;
		}
	AdjustButtons();
	TableRefresh();
}

/******************************************************************************
 AdjustButtons (private)

 ******************************************************************************/

void
GMFilterActionTable::AdjustButtons()
{
	if (itsFilter == NULL)
		{
		itsNewButton->Deactivate();
		}
	else
		{
		itsNewButton->Activate();
		}

	JTableSelection& s		= GetTableSelection();
	JBoolean hasSelection	= s.HasSelection();

	if (GetRowCount() <= 1 ||
		!hasSelection ||
		s.GetSelectedCellCount() == GetRowCount())
		{
		itsRemoveButton->Deactivate();
		}
	else
		{
		itsRemoveButton->Activate();
		}

	JPoint cell;
	JBoolean hasSingle	= s.GetSingleSelectedCell(&cell);
	if (hasSelection && hasSingle)
		{
		GMFilterAction* action	= itsActions->NthElement(cell.y);
		if (action->IsMailbox())
			{
			itsChooseButton->Activate();
			}
		else
			{
			itsChooseButton->Deactivate();
			}
		}
	else
		{
		itsChooseButton->Deactivate();
		}
}

/******************************************************************************
 OKToSwitch (public)

 ******************************************************************************/

JBoolean
GMFilterActionTable::OKToSwitch()
{
	if (IsEditing())
		{
		return EndEditing();
		}
	return kJTrue;
}

/******************************************************************************
 NewAction (private)

 ******************************************************************************/

void
GMFilterActionTable::NewAction()
{
	if (IsEditing() && !itsInput->InputValid())
		{
		return;
		}
	GMFilterAction* action = new GMFilterAction();
	assert(action != NULL);

	itsActions->Append(action);
	AppendRows(1, itsRowHeight);

	GetTableSelection().ClearSelection();
	GetTableSelection().SelectCell(GetRowCount(), 1);
	BeginEditing(JPoint(kDestinationColumn, GetRowCount()));

	const JSize count = itsActions->GetElementCount();
	for (JIndex i = 1; i < count; i++)
		{
		action = itsActions->NthElement(i);
		action->ShouldCopy(kJTrue);
		}

	TableRefresh();
	AdjustButtons();
}

/******************************************************************************
 RemoveSelectedActions (private)

 ******************************************************************************/

void
GMFilterActionTable::RemoveSelectedActions()
{
	JTableSelection& s	= GetTableSelection();
	assert(s.HasSelection());

	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell))
		{
		itsActions->DeleteElement(cell.y);
		RemoveRow(cell.y);
		}
	AdjustButtons();
}

/******************************************************************************
 ChooseMailbox (private)

 ******************************************************************************/

void
GMFilterActionTable::ChooseMailbox()
{
	JTableSelection& s		= GetTableSelection();
	JPoint cell;
	if (!s.GetSingleSelectedCell(&cell))
		{
		return;
		}

	JString temphome;
	JString home;
	JGetHomeDirectory(&temphome);
	JGetTrueName(temphome, &home);
	JAppendDirSeparator(&home);
	JString fullname = home;
	if (JXGetChooseSaveFile()->ChooseFile("","",&fullname))
		{
		JString name	= GGetRelativeToHome(fullname);
		GMFilterAction* action = itsActions->NthElement(cell.y);
		action->SetDestination(name);
		if (IsEditing())
			{
			itsInput->SetText(name);
			}
		}
}
