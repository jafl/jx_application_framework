/******************************************************************************
 JStyleTableData.cpp

	Stores font and style information for a table.  The font and size
	are restricted to be table-wide, while the style can be changed for
	each cell.

	BASE CLASS = JAuxTableData<JFontStyle>

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JStyleTableData.h>
#include <JTable.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kVMarginWidth = 1;

// JBroadcaster messages

const JCharacter* JStyleTableData::kFontChanged = "FontChanged::JStyleTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JStyleTableData::JStyleTableData
	(
	JTable*				table,
	const JFontManager*	fontManager,
	JColormap*			colormap
	)
	:
	JAuxTableData<JFontStyle>(table, JFontStyle()),
	itsFontManager(fontManager),
	itsColormap(colormap)
{
	SetFont(JGetDefaultFontName(), kJDefaultFontSize);

	itsProcessMessagesFlag = kJTrue;
	ListenTo(GetTable());
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStyleTableData::~JStyleTableData()
{
	UpdateAllColors(kJFalse);
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JStyleTableData::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	itsFontName = name;
	itsFontSize = size;

	const JSize rowHeight = 2*kVMarginWidth +
		itsFontManager->GetLineHeight(itsFontName, itsFontSize, JFontStyle());
	(GetTable())->SetDefaultRowHeight(rowHeight);
	(GetTable())->SetAllRowHeights(rowHeight);

	Broadcast(FontChanged());
}

/******************************************************************************
 SetCellStyle

 ******************************************************************************/

void
JStyleTableData::SetCellStyle
	(
	const JPoint&		cell,
	const JFontStyle&	style
	)
{
	const JFontStyle origStyle = GetCellStyle(cell);
	if (style != origStyle)
		{
		if (style.color != origStyle.color)
			{
			itsColormap->DeallocateColor(origStyle.color);
			itsColormap->UsingColor(style.color);
			}

		SetElement(cell, style);
		}
}

/******************************************************************************
 SetAllCellStyles

 ******************************************************************************/

void
JStyleTableData::SetAllCellStyles
	(
	const JFontStyle& style
	)
{
	UpdateAllColors(kJFalse);
	SetAllElements(style);
	UpdateAllColors(kJTrue);
}

/******************************************************************************
 Receive (virtual protected)

	We ignore RectChanged because we handle it in SetCellStyle().

 ******************************************************************************/

void
JStyleTableData::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// row created

	if (itsProcessMessagesFlag &&
		sender == this && message.Is(JTableData::kRowsInserted))
		{
		const JTableData::RowsInserted* info =
			dynamic_cast(const JTableData::RowsInserted*, &message);
		assert( info != NULL );
		UpdateRectColors(JRect(info->GetFirstIndex(),  1,
							   info->GetLastIndex()+1, GetColCount()+1), kJTrue);
		}

	else if (itsProcessMessagesFlag &&
			 sender == this && message.Is(JTableData::kRowDuplicated))
		{
		const JTableData::RowDuplicated* info =
			dynamic_cast(const JTableData::RowDuplicated*, &message);
		assert( info != NULL );
		UpdateRectColors(JRect(info->GetNewIndex(),   1,
							   info->GetNewIndex()+1, GetColCount()+1), kJTrue);
		}

	// column created

	else if (itsProcessMessagesFlag &&
			 sender == this && message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast(const JTableData::ColsInserted*, &message);
		assert( info != NULL );
		UpdateRectColors(JRect(1,               info->GetFirstIndex(),
							   GetRowCount()+1, info->GetLastIndex()+1), kJTrue);
		}

	else if (itsProcessMessagesFlag &&
			 sender == this && message.Is(JTableData::kColDuplicated))
		{
		const JTableData::ColDuplicated* info =
			dynamic_cast(const JTableData::ColDuplicated*, &message);
		assert( info != NULL );
		UpdateRectColors(JRect(1,               info->GetNewIndex(),
							   GetRowCount()+1, info->GetNewIndex()+1), kJTrue);
		}

	// something about to be removed

	else if (itsProcessMessagesFlag &&
			 sender == GetTable() && message.Is(JTable::kPrepareForTableDataMessage))
		{
		const JTable::PrepareForTableDataMessage* info =
			dynamic_cast(const JTable::PrepareForTableDataMessage*, &message);
		assert( info != NULL );
		const Message& nextMessage = info->GetMessage();

		// rows

		if (nextMessage.Is(JTableData::kRowsRemoved))
			{
			const JTableData::RowsRemoved* nextInfo =
				dynamic_cast(const JTableData::RowsRemoved*, &nextMessage);
			assert( nextInfo != NULL );
			if (nextInfo->GetFirstIndex() == 1 && nextInfo->GetCount() == GetRowCount())
				{
				UpdateAllColors(kJFalse);
				}
			else
				{
				UpdateRectColors(JRect(nextInfo->GetFirstIndex(),  1,
									   nextInfo->GetLastIndex()+1, GetColCount()+1), kJFalse);
				}
			}

		// columns

		else if (nextMessage.Is(JTableData::kColsRemoved))
			{
			const JTableData::ColsRemoved* nextInfo =
				dynamic_cast(const JTableData::ColsRemoved*, &nextMessage);
			assert( nextInfo != NULL );
			if (nextInfo->GetFirstIndex() == 1 && nextInfo->GetCount() == GetColCount())
				{
				UpdateAllColors(kJFalse);
				}
			else
				{
				UpdateRectColors(JRect(1,               nextInfo->GetFirstIndex(),
									   GetRowCount()+1, nextInfo->GetLastIndex()+1), kJFalse);
				}
			}
		}

	// new JTableData object

	else if (sender == GetTable() && message.Is(JTable::kPrepareForTableDataChange))
		{
		itsProcessMessagesFlag = kJFalse;
		UpdateAllColors(kJFalse);
		// This is safe because everything is reset to the default color.
		}
	else if (sender == GetTable() && message.Is(JTable::kTableDataChanged))
		{
		UpdateAllColors(kJTrue);
		itsProcessMessagesFlag = kJTrue;
		}

	// always pass down our own messages

	JAuxTableData<JFontStyle>::Receive(sender, message);
}

/******************************************************************************
 UpdateAllColors (private)

 ******************************************************************************/

void
JStyleTableData::UpdateAllColors
	(
	const JBoolean allocate
	)
{
	const JRunArray<JFontStyle>& data = GetData();
	const JSize runCount              = data.GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		const JFontStyle& style = data.GetRunDataRef(i);
		const JSize runLength   = data.GetRunLength(i);
		for (JIndex j=1; j<=runLength; j++)
			{
			if (allocate)
				{
				itsColormap->UsingColor(style.color);
				}
			else
				{
				itsColormap->DeallocateColor(style.color);
				}
			}
		}
}

/******************************************************************************
 UpdateRectColors (private)

 ******************************************************************************/

void
JStyleTableData::UpdateRectColors
	(
	const JRect&	rect,
	const JBoolean	allocate
	)
{
	for (JIndex y=rect.top; y < (JSize) rect.bottom; y++)
		{
		for (JIndex x=rect.left; x < (JSize) rect.right; x++)
			{
			const JFontStyle style = GetCellStyle(JPoint(x,y));
			if (allocate)
				{
				itsColormap->UsingColor(style.color);
				}
			else
				{
				itsColormap->DeallocateColor(style.color);
				}
			}
		}
}
