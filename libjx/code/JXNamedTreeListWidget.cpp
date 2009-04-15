/******************************************************************************
 JXNamedTreeListWidget.cpp

	Displays a JNamedTreeList and allows an image to the left of each
	node's name.  It also supports incremental searching of the visible
	nodes by simply typing.

	Note that things may look strange if sibling nodes have icons
	of different widths.

	Derived classes can override:

		GetImage
			Returns kJTrue if an image should be displayed to the left
			of the node's name.

		CreateTreeListInput
			Create a derived class of JXInputField and/or set extra options.

	BASE CLASS = JXTreeListWidget

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXNamedTreeListWidget.h>
#include <JXInputField.h>
#include <JXFontManager.h>
#include <JXImage.h>
#include <JNamedTreeList.h>
#include <JNamedTreeNode.h>
#include <JPainter.h>
#include <JTableSelection.h>
#include <JStyleTableData.h>
#include <JString.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JCoordinate kImageTextSpacing = 5;
const JCoordinate kRightMarginWidth = 10;	// for min cell width
const JCoordinate kEmptyTextWidth   = 100;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXNamedTreeListWidget::JXNamedTreeListWidget
	(
	JNamedTreeList*		treeList,
	JXScrollbarSet*	scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTreeListWidget(treeList, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsNamedTreeList       = treeList;
	itsNameInputField      = NULL;
	itsHilightTextOnlyFlag = kJFalse;

	WantInput(kJTrue);

	ListenTo(&(GetStyleData()));	// could stick in JXTreeListWidgetX() if crashes
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXNamedTreeListWidget::~JXNamedTreeListWidget()
{
}

/******************************************************************************
 GetNamedTreeNode

 ******************************************************************************/

JNamedTreeNode*
JXNamedTreeListWidget::GetNamedTreeNode
	(
	const JIndex index
	)
{
	return itsNamedTreeList->GetNamedNode(index);
}

const JNamedTreeNode*
JXNamedTreeListWidget::GetNamedTreeNode
	(
	const JIndex index
	)
	const
{
	return itsNamedTreeList->GetNamedNode(index);
}

/******************************************************************************
 TLWDrawNode (virtual protected)

 ******************************************************************************/

void
JXNamedTreeListWidget::TLWDrawNode
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	const JXImage* image;
	const JBoolean hasImage = GetImage(cell.y, &image);
	if (hasImage)
		{
		JRect r = rect;
		r.right = r.left + image->GetWidth();
		p.Image(*image, image->GetBounds(), r);
		}

	JPoint editCell;
	if (!GetEditedCell(&editCell) || cell != editCell)
		{
		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);
		p.SetFont(fontName, fontSize, GetCellStyle(cell));

		const JString& text = itsNamedTreeList->GetNodeName(cell.y);

		JRect textRect = rect;
		if (hasImage)
			{
			textRect.left += GetIndentWidth() + kImageTextSpacing;
			}
		if (itsHilightTextOnlyFlag)
			{
			textRect.right   = textRect.left + p.GetStringWidth(text);
			textRect.top    += 1;
			textRect.bottom -= 1;
			HilightIfSelected(p, cell, textRect);
			}

		p.String(textRect, text, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 GetImage (virtual protected)

 ******************************************************************************/

JBoolean
JXNamedTreeListWidget::GetImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	*image = NULL;
	return kJFalse;
}

/******************************************************************************
 GetImageWidth

 ******************************************************************************/

JSize
JXNamedTreeListWidget::GetImageWidth
	(
	const JIndex index
	)
	const
{
	const JXImage* image;
	if (GetImage(index, &image))
		{
		return GetIndentWidth() + kImageTextSpacing;
		}
	else
		{
		return 0;
		}
}

/******************************************************************************
 GetImageRect

	Returns kJFalse if there is no image.

 ******************************************************************************/

JBoolean
JXNamedTreeListWidget::GetImageRect
	(
	const JIndex	index,
	JRect*			rect
	)
	const
{
	const JXImage* image;
	if (GetImage(index, &image))
		{
		*rect       = GetCellRect(JPoint(GetNodeColIndex(), index));
		rect->left += GetNodeIndent(index);
		rect->right = rect->left + GetIndentWidth();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetTextWidth

 ******************************************************************************/

JSize
JXNamedTreeListWidget::GetTextWidth
	(
	const JIndex index
	)
	const
{
	JSize fontSize;
	const JString& fontName = GetFont(&fontSize);
	return (GetFontManager())->GetStringWidth(fontName, fontSize,
											  GetCellStyle(JPoint(GetNodeColIndex(), index)),
											  itsNamedTreeList->GetNodeName(index));
}

/******************************************************************************
 GetTextRect

 ******************************************************************************/

JRect
JXNamedTreeListWidget::GetTextRect
	(
	const JIndex index
	)
	const
{
	JRect r = GetCellRect(JPoint(GetNodeColIndex(), index));
	r.left += GetNodeIndent(index) + GetImageWidth(index);
	r.right = r.left + GetTextWidth(index);
	return r;
}

/******************************************************************************
 GetNodeRect

 ******************************************************************************/

JRect
JXNamedTreeListWidget::GetNodeRect
	(
	const JIndex index
	)
	const
{
	JRect r = GetCellRect(JPoint(GetNodeColIndex(), index));
	r.left += GetNodeIndent(index);
	r.right = r.left + GetImageWidth(index) + GetTextWidth(index);
	return r;
}

/******************************************************************************
 AdjustToTree (virtual protected)

 ******************************************************************************/

void
JXNamedTreeListWidget::AdjustToTree()
{
	JCoordinate h = GetDefaultRowHeight();
	JCoordinate w = GetIndentWidth();	// start with current width

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JXImage* image;
		if (GetImage(i, &image))
			{
			h = JMax(h, image->GetHeight());
			w = JMax(w, image->GetWidth());
			}
		}

	SetAllRowHeights(h);
	SetIndentWidth(w);

	JXTreeListWidget::AdjustToTree();
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

	This only works for the node column.  Derived classes must
	override it if the other columns need to adjust their widths.

 ******************************************************************************/

JSize
JXNamedTreeListWidget::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) == GetNodeColIndex())
		{
		JSize fontSize;
		const JString& fontName = GetFont(&fontSize);
		const JString& text     = itsNamedTreeList->GetNodeName(cell.y);

		JSize textWidth = kEmptyTextWidth;
		JPoint editCell;
		JXInputField* input = NULL;
		if (GetEditedCell(&editCell) && editCell == cell &&
			GetXInputField(&input) && !(input->GetText()).IsEmpty())
			{
			textWidth = input->GetBoundsWidth() - kRightMarginWidth - 1;
			}
		else if (!text.IsEmpty())
			{
			textWidth = (GetFontManager())->GetStringWidth(fontName, fontSize,
														   GetCellStyle(cell), text);
			}

		return GetNodeIndent(cell.y) +
			   GetImageWidth(cell.y) +
			   textWidth +
			   kRightMarginWidth;	// margin
		}
	else
		{
		return GetColWidth(cell.x);
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXNamedTreeListWidget::HandleFocusEvent()
{
	JXTreeListWidget::HandleFocusEvent();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXNamedTreeListWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == ' ' || (key == kJEscapeKey && !IsEditing()))
		{
		ClearIncrementalSearchBuffer();
		(GetTableSelection()).ClearSelection();
		}

	// incremental search

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.AppendCharacter(key);

		JIndex index;
		if (itsNamedTreeList->ClosestMatch(itsKeyBuffer, &index))
			{
			const JString saveBuffer = itsKeyBuffer;
			SelectSingleCell(JPoint(GetNodeColIndex(), index));
			itsKeyBuffer = saveBuffer;
			}
		else
			{
			(GetTableSelection()).ClearSelection();
			}
		}

	else
		{
		if (0 < key && key <= 255)
			{
			ClearIncrementalSearchBuffer();
			}
		JXTreeListWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 ClearIncrementalSearchBuffer

 ******************************************************************************/

void
JXNamedTreeListWidget::ClearIncrementalSearchBuffer()
{
	itsKeyBuffer.Clear();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXNamedTreeListWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNamedTreeList &&
		(message.Is(JTreeList::kNodeInserted) ||
		 message.Is(JTreeList::kNodeRemoved) ||
		 message.Is(JTreeList::kNodeChanged)))
		{
		ClearIncrementalSearchBuffer();
		}

	else if (sender == const_cast<JStyleTableData*>(&(GetStyleData())) &&
			 message.Is(JStyleTableData::kFontChanged))
		{
		NeedsAdjustToTree();
		}

	JXTreeListWidget::Receive(sender, message);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JXNamedTreeListWidget::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsNameInputField == NULL );

	const JCoordinate dx = JIndex(cell.x) == GetNodeColIndex() ?
							GetInputFieldIndent(cell.y) :
							-(JTextEditor::kMinLeftMarginWidth + 1);
	assert( dx < w );

	itsNameInputField =
		CreateTreeListInput(cell, this, kFixedLeft, kFixedTop, x+dx,y, w-dx,h);
	assert( itsNameInputField != NULL );

	JSize fontSize;
	const JString& fontName = GetFont(&fontSize);

	itsNameInputField->SetFont(fontName, fontSize, GetCellStyle(cell));
	itsNameInputField->SetText(itsNamedTreeList->GetNodeName(cell.y));
	return itsNameInputField;
}

/******************************************************************************
 CreateTreeListInput (virtual protected)

	Derived class can override this to instantiate a derived class of
	JXInputField.

 ******************************************************************************/

JXInputField*
JXNamedTreeListWidget::CreateTreeListInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JXInputField* obj = new JXInputField(enclosure, hSizing, vSizing, x,y, w,h);
	assert( obj != NULL );
	return obj;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
JXNamedTreeListWidget::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsNameInputField != NULL );

	if (JIndex(cell.x) == GetNodeColIndex() && itsNameInputField->InputValid())
		{
		itsNamedTreeList->SetNodeName(cell.y, itsNameInputField->GetText());
		NeedsAdjustToTree();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JXNamedTreeListWidget::PrepareDeleteXInputField()
{
	itsNameInputField = NULL;
}

/******************************************************************************
 Adjusting the input field (virtual protected)

	When editing the node column, the input field is indented and shouldn't
	cover the image.

 ******************************************************************************/

void
JXNamedTreeListWidget::PlaceInputField
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	JPoint cell;
	const JBoolean ok = GetEditedCell(&cell);
	assert( ok );
	JXTreeListWidget::PlaceInputField(
		JIndex(cell.x) == GetNodeColIndex() ? x + GetInputFieldIndent(cell.y) : x, y);
}

void
JXNamedTreeListWidget::SetInputFieldSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	JPoint cell;
	const JBoolean ok = GetEditedCell(&cell);
	assert( ok );
	JXTreeListWidget::SetInputFieldSize(
		JIndex(cell.x) == GetNodeColIndex() ? w - GetInputFieldIndent(cell.y) : w, h);
}

/******************************************************************************
 GetInputFieldIndent (protected)

 ******************************************************************************/

JCoordinate
JXNamedTreeListWidget::GetInputFieldIndent
	(
	const JIndex index
	)
	const
{
	return GetNodeIndent(index) + GetImageWidth(index)
		   - JTextEditor::kMinLeftMarginWidth - 1;
}

/******************************************************************************
 GetNode (protected)

	Augments JTable::GetCell() by calculating the part of the node
	corresponding to pt.

 ******************************************************************************/

JBoolean
JXNamedTreeListWidget::GetNode
	(
	const JPoint&	pt,
	JPoint*			cell,
	NodePart*		part
	)
	const
{
	if (!GetCell(pt, cell))
		{
		return kJFalse;
		}
	else if (JIndex(cell->x) == GetToggleOpenColIndex())
		{
		*part = kToggleColumn;
		return kJTrue;
		}
	else if (JIndex(cell->x) != GetNodeColIndex())
		{
		*part = kOtherColumn;
		return kJTrue;
		}

	const JRect r = GetCellRect(*cell);
	JCoordinate x = pt.x - r.left - GetNodeIndent(cell->y);
	if (x < 0)
		{
		*part = kBeforeImage;
		return kJTrue;
		}

	x -= GetImageWidth(cell->y);
	if (x < 0)
		{
		*part = kInImage;
		return kJTrue;
		}

	*part = (x <= (JCoordinate) GetTextWidth(cell->y) ? kInText : kAfterText);
	return kJTrue;
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

JBoolean
JXNamedTreeListWidget::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	JPoint cell1, cell2;
	NodePart part1, part2;
	return JConvertToBoolean( GetNode(pt1, &cell1, &part1) &&
							  GetNode(pt2, &cell2, &part2) &&
							  cell1 == cell2 && part1 == part2 );
}
