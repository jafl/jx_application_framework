/******************************************************************************
 GMMBoxColHeaderWidget.cc

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <GMMBoxColHeaderWidget.h>
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDragPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const JSize kFontSize                  = 10;
const JSize kCellFrameWidth            = kJXDefaultBorderWidth;
const JCoordinate kDragRegionHalfWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMMBoxColHeaderWidget::GMMBoxColHeaderWidget
	(
	JXTable*			table,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTable(h,1, NULL, enclosure, hSizing,vSizing, x,y, w,h)
{
	assert( table != NULL);

	itsTable = table;
	ListenTo(itsTable);

	itsTitles = NULL;

	SetDrawOrder(kDrawByRow);
	SetRowBorderInfo(0, (GetColormap())->GetBlackColor());

	// override JXTable

	WantInput(kJFalse);
	SetBackColor((GetColormap())->GetDefaultBackColor());

	AppendRow(GetApertureHeight());
	AdjustToTable();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMMBoxColHeaderWidget::~GMMBoxColHeaderWidget()
{
	if (itsTitles != NULL)
		{
		itsTitles->DeleteAll();
		delete itsTitles;
		}
}

/******************************************************************************
 GetColTitle

	Returns kJTrue if there is a title for the specified column.

 ******************************************************************************/

JBoolean
GMMBoxColHeaderWidget::GetColTitle
	(
	const JIndex	index,
	JString*		title
	)
	const
{
	if (itsTitles != NULL)
		{
		const JString* str = itsTitles->NthElement(index);
		if (str != NULL)
			{
			*title = *str;
			return kJTrue;
			}
		}

	title->Clear();
	return kJFalse;
}

/******************************************************************************
 SetColTitle

 ******************************************************************************/

void
GMMBoxColHeaderWidget::SetColTitle
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	if (itsTitles == NULL)
		{
		itsTitles = new JPtrArray<JString>;
		assert( itsTitles != NULL );

		const JSize colCount = GetColCount();
		for (JIndex i=1; i<=colCount; i++)
			{
			itsTitles->Append(NULL);
			}
		}

	JString* str = itsTitles->NthElement(index);
	if (str != NULL)
		{
		*str = title;
		}
	else
		{
		str = new JString(title);
		assert( str != NULL );
		itsTitles->SetElement(index, str);
		}
}

/******************************************************************************
 ClearColTitle

 ******************************************************************************/

void
GMMBoxColHeaderWidget::ClearColTitle
	(
	const JIndex index
	)
{
	if (itsTitles != NULL)
		{
		const JString* str = itsTitles->NthElement(index);
		if (str != NULL)
			{
			itsTitles->SetToNull(index);
			delete str;
			}
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
GMMBoxColHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kCellFrameWidth);

	JString str;
	JBoolean hasTitle = kJFalse;
	if (itsTitles != NULL)
		{
		const JString* title = itsTitles->NthElement(cell.x);
		if (title != NULL)
			{
			str      = *title;
			hasTitle = kJTrue;
			}
		}
	if (!hasTitle)
		{
		str = JString(cell.x, 0);
		}

	p.SetFont(JGetDefaultFontName(), kFontSize,
			  JFontStyle(kJTrue, kJFalse, 0, kJFalse, (p.GetColormap())->GetBlackColor()));
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMMBoxColHeaderWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	SetRowHeight(1, GetApertureHeight());
}

/******************************************************************************
 Receive (virtual protected)

	Listen for adjustments in table and vertical scrollbar.

 ******************************************************************************/

void
GMMBoxColHeaderWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTable && message.Is(JTable::kColWidthChanged))
		{
		const JTable::ColWidthChanged* info =
			dynamic_cast(const JTable::ColWidthChanged*, &message);
		assert( info != NULL );
		SetColWidth(info->GetIndex(), info->GetNewColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kAllColWidthsChanged))
		{
		const JTable::AllColWidthsChanged* info =
			dynamic_cast(const JTable::AllColWidthsChanged*, &message);
		assert( info != NULL );
		SetAllColWidths(info->GetNewColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kColInserted))
		{
		const JTable::ColInserted* info =
			dynamic_cast(const JTable::ColInserted*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			itsTitles->InsertAtIndex(info->GetIndex(), (JString*) NULL);
			}
		InsertCol(info->GetIndex(), info->GetColWidth());
		}

	else if (sender == itsTable && message.Is(JTable::kColRemoved))
		{
		const JTable::ColRemoved* info =
			dynamic_cast(const JTable::ColRemoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			itsTitles->DeleteElement(info->GetIndex());
			}
		RemoveCol(info->GetIndex());
		}

	else if (sender == itsTable && message.Is(JTable::kAllColsRemoved))
		{
		if (itsTitles != NULL)
			{
			itsTitles->DeleteAll();
			delete itsTitles;
			itsTitles = NULL;
			}
		RemoveAllCols();
		}

	else if (sender == itsTable && message.Is(JTable::kColMoved))
		{
		const JTable::ColMoved* info =
			dynamic_cast(const JTable::ColMoved*, &message);
		assert( info != NULL );
		if (itsTitles != NULL)
			{
			itsTitles->MoveElementToIndex(info->GetOrigIndex(), info->GetNewIndex());
			}
		MoveCol(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == itsTable && message.Is(JTable::kColBorderWidthChanged))
		{
		const JTable::ColBorderWidthChanged* info =
			dynamic_cast(const JTable::ColBorderWidthChanged*, &message);
		assert( info != NULL );
		SetColBorderInfo(info->GetNewBorderWidth(), (GetColormap())->GetDefaultBackColor());
		}

	// something else

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustToTable (private)

 ******************************************************************************/

void
GMMBoxColHeaderWidget::AdjustToTable()
{
	assert( itsTable != NULL && itsTitles == NULL );

	JCoordinate width;
	JColorIndex color;
	itsTable->GetColBorderInfo(&width, &color);
	SetColBorderInfo(width, (GetColormap())->GetDefaultBackColor());

	RemoveAllCols();

	const JSize colCount = itsTable->GetColCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		InsertCol(i, itsTable->GetColWidth(i));
		}
}
