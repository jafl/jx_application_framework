/******************************************************************************
 GMAccountList.cc

	BASE CLASS = public JXEditTable

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMAccountList.h>
#include <GMAccount.h>
#include <GMAccountDialog.h>
#include <JXColormap.h>
#include <JXInputField.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jGlobals.h>
#include <jASCIIConstants.h>

#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;
const JCoordinate kDefRowHeight	= 20;

const JCharacter* GMAccountList::kNameSelected    = "GMAccountList::kNameSelected";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMAccountList*
GMAccountList::Create
	(
	GMAccountDialog*		dialog,
	JPtrArray<GMAccount>*	accounts,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
{
	GMAccountList* widget	=
		new GMAccountList(dialog, accounts, scrollbarSet, enclosure,
			hSizing,vSizing, x,y, w,h);
	assert(widget != NULL);
	widget->GMAccountListX();
	return widget;
}

GMAccountList::GMAccountList
	(
	GMAccountDialog*		dialog,
	JPtrArray<GMAccount>*	accounts,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
   JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
   itsInput(NULL),
   itsDialog(dialog),
   itsAccountInfo(accounts)
{
	itsMinColWidth = 1;

	JXColormap* colormap         = GetColormap();
	const JColorIndex blackColor = colormap->GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);
	SetBackColor(JGetCurrColormap()->GetWhiteColor());

	const JSize fontHeight =
		(GetFontManager())->GetLineHeight(JGetDefaultFontName(),
		kJDefaultFontSize, JFontStyle());
	const JCoordinate rowHeight = fontHeight + 2*kVMarginWidth;
	SetDefaultRowHeight(rowHeight);
	SetAllRowHeights(rowHeight);

	itsLineHeight =
		GetFontManager()->GetLineHeight(JGetDefaultFontName(),
		kJDefaultFontSize, JFontStyle()) + 2 * kVMarginWidth;

	ListenTo(itsAccountInfo);
}

void
GMAccountList::GMAccountListX()
{
	AppendCols(1, kDefColWidth);
	const JSize count	= itsAccountInfo->GetElementCount();
	AppendRows(count, itsLineHeight);
	GetTableSelection().SelectRow(1);
	WantInput(kJFalse);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMAccountList::~GMAccountList()
{
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
GMAccountList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && clickCount == 1 &&
		GetCell(pt, &cell))
		{
		JTableSelection& s = GetTableSelection();
		if (!s.IsSelected(cell) && itsDialog->OKToSwitchAccounts())
			{
			s.ClearSelection();
			s.SelectRow(cell.y);
			Broadcast(NameSelected(cell.y));
			BeginEditing(cell);
			}
		else if (s.IsSelected(cell) && !IsEditing())
			{
			BeginEditing(cell);
			}
		}
	else if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMAccountList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	JFontStyle fontstyle;
	if (itsDialog->GetDefaultIndex() == (JIndex)cell.y)
		{
		fontstyle.bold	= kJTrue;
		}

	p.SetFont(JGetDefaultFontName(), kJDefaultFontSize, fontstyle);

	JString str = itsAccountInfo->NthElement(cell.y)->GetNickname();

	// check that column is wide enough

	const JCoordinate w = p.GetStringWidth(str) + 2*kHMarginWidth;
	if (w > GetColWidth(1))
		{
		itsMinColWidth = w;
		}

	// draw string

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
	if (cell.y == JCoordinate(itsAccountInfo->GetElementCount()))
		{
		AdjustColWidth();
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMAccountList::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes in our list.

 ******************************************************************************/

void
GMAccountList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAccountInfo &&
		message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast(const JOrderedSetT::ElementsInserted*, &message);
		assert( info != NULL );
		const JIndex firstIndex = info->GetFirstIndex();
		const JSize count       = info->GetCount();
		InsertRows(firstIndex, count);
		}

	else if (sender == itsAccountInfo &&
			 message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast(const JOrderedSetT::ElementsRemoved*, &message);
		assert( info != NULL );
		const JIndex firstIndex = info->GetFirstIndex();
		const JSize count       = info->GetCount();
		itsMinColWidth = 1;
		RemoveNextRows(firstIndex, count);
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
GMAccountList::AdjustColWidth()
{
	JCoordinate w = GetApertureWidth();
	if (w < itsMinColWidth)
		{
		w = itsMinColWidth;
		}
	SetColWidth(1,w);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GMAccountList::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert(itsInput == NULL);
	itsInput =
		new JXInputField(this,
				JXWidget::kHElastic, JXWidget::kFixedBottom,
				x, y, w, h);
	assert(itsInput != NULL);
	assert(cell.y <= (JCoordinate)(itsAccountInfo->GetElementCount()));
	// need to check for duplicates here
	itsInput->SetText(itsAccountInfo->NthElement(cell.y)->GetNickname());
	itsInput->SetIsRequired();
	if (itsDialog->GetDefaultIndex() == (JIndex)cell.y)
		{
		itsInput->SetFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse));
		}
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GMAccountList::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GMAccountList::ExtractInputData
	(
	const JPoint& cell
	)
{
	if (!itsInput->InputValid())
		{
		return kJFalse;
		}
	JString name		= itsInput->GetText();
	const JSize count	= itsAccountInfo->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		if (name == itsAccountInfo->NthElement(i)->GetNickname() &&
			(JCoordinate)i != cell.y)
			{
			JGetUserNotification()->ReportError("This nickname is already in use.");
			return kJFalse;
			}
		}
	assert(cell.y <= (JCoordinate)(itsAccountInfo->GetElementCount()));
	itsAccountInfo->NthElement(cell.y)->SetNickname(name);
	return kJTrue;
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GMAccountList::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJEscapeKey)
		{
		if (IsEditing())
			{
			CancelEditing();
			}
		}
	else if (key == kJReturnKey)
		{
		if (IsEditing())
			{
			EndEditing();
			}
		}
	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 GetCurrentIndex (public)

 ******************************************************************************/

JIndex
GMAccountList::GetCurrentIndex()
{
	JPoint cell;
	if (GetEditedCell(&cell))
		{
		return cell.y;
		}
	JBoolean ok = GetTableSelection().GetFirstSelectedCell(&cell);
	assert(ok);
	return cell.y;
}

/******************************************************************************
 SetCurrentAccount (public)

 ******************************************************************************/

void
GMAccountList::SetCurrentAccount
	(
	const JIndex index
	)
{
	JTableSelection& s	= GetTableSelection();
	s.ClearSelection();
	JPoint cell(1, index);
	s.SelectRow(index);
	BeginEditing(cell);
}

/******************************************************************************
 SetCurrentAsDefault (public)

 ******************************************************************************/

void
GMAccountList::SetCurrentAsDefault()
{
	if (IsEditing())
		{
		itsInput->SetFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse));
		}
	TableRefresh();
}
