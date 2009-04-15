/******************************************************************************
 JXTreeListWidget.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTreeListWidget
#define _H_JXTreeListWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStyleTable.h>

class JTreeNode;
class JTreeList;
class JXTLWAdjustToTreeTask;

class JXTreeListWidget : public JXStyleTable
{
	friend class JXTLWAdjustToTreeTask;

public:

	JXTreeListWidget(JTreeList* treeList,
					 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual	~JXTreeListWidget();

	JTreeList*			GetTreeList();
	const JTreeList*	GetTreeList() const;

	JIndex	GetToggleOpenColIndex() const;
	JIndex	GetNodeColIndex() const;

	void	ScrollToNode(const JTreeNode* node);
	void	ScrollToShowChildren(const JIndex index);
	void	GetSelectedNodes(JPtrArray<JTreeNode>* list);
	void	SelectNodes(const JPtrArray<JTreeNode>& list);
	void	OpenSelectedNodes(const JBoolean openSiblings = kJFalse,
							  const JBoolean openDescendants = kJFalse);
	void	CloseSelectedNodes(const JBoolean closeSiblings = kJFalse,
							   const JBoolean closeDescendants = kJFalse);
	void	ToggleNode(const JIndex index, const JBoolean siblings = kJFalse,
					   const JBoolean descendants = kJFalse);

	JSize			GetIndentWidth() const;
	void			SetIndentWidth(const JSize width);
	JSize			GetNodeIndent(const JIndex index) const;
	static JSize	GetDefaultIndentWidth();

	JSize	GetMaxOpenDepth() const;
	void	SetMaxOpenDepth(const JSize maxDepth);

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual JBoolean	IsSelectable(const JPoint& cell,
									 const JBoolean forExtend) const;
	virtual JBoolean	IsEditable(const JPoint& cell) const;

	void	ForceAdjustToTree();

protected:

	JBoolean	WillDrawSelection() const;
	void		ShouldDrawSelection(const JBoolean draw);

	JIndex	GetElasticColIndex() const;
	void	SetElasticColIndex(const JIndex index);

	void			NeedsAdjustToTree();
	virtual void	AdjustToTree();
	virtual JSize	GetMinCellWidth(const JPoint& cell) const = 0;

	JBoolean		WantsToDrawCell(const JPoint& cell) const;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	TLWDrawNode(JPainter& p, const JPoint& cell, const JRect& rect) = 0;

	JBoolean		IsDraggingToggle() const;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	JBoolean	GetDNDTargetIndex(JIndex* index) const;
	void		SetDNDTargetIndex(const JIndex index);
	void		ClearDNDTargetIndex();

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JTreeList*	itsTreeList;
	JIndex		itsToggleOpenColIndex;
	JIndex		itsNodeColIndex;
	JSize		itsIndentWidth;
	JBoolean	itsDrawSelectionFlag;

	JArray<JSize>*	itsMinColWidths;		// minimum width of each column
	JIndex			itsElasticColIndex;		// column that can expand to fill aperture width

	JXTLWAdjustToTreeTask*	itsAdjustToTreeTask;	// can be NULL

	// used during node move

	JPtrArray<JTreeNode>*	itsReselectNodeList;	// contents owned by JTree
	JString*				itsSavedScrollSetup;	// NULL except during move

	// used while dragging

	JBoolean		itsMouseInToggleFlag;
	JCoordinate		itsToggleDragIndex;		// 0 => off; JCoordinate to avoid signed-unsigned compare
	JSize			itsMaxOpenDepth;		// max depth when opening descendants

	JIndex			itsDNDTargetIndex;		// highlight arrow if > 0

private:

	void	AdjustColWidths();

	void	HandlePrepareForNodeMove();
	void	HandleNodeMoveFinished();

	// not allowed

	JXTreeListWidget(const JXTreeListWidget& source);
	const JXTreeListWidget& operator=(const JXTreeListWidget& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNodeDblClicked;

	class NodeDblClicked : public JBroadcaster::Message
		{
		public:

			NodeDblClicked(const JTreeNode* node, const JPoint& cell)
				:
				JBroadcaster::Message(kNodeDblClicked),
				itsNode(node),
				itsCell(cell)
			{ };

			const JTreeNode*
			GetNode() const
			{
				return itsNode;
			};

			JIndex
			GetRow() const
			{
				return itsCell.y;
			};

			JIndex
			GetColumn() const
			{
				return itsCell.x;
			};

			const JPoint&
			GetCell() const
			{
				return itsCell;
			};

		private:

			const JTreeNode*	itsNode;
			JPoint				itsCell;
		};
};


/******************************************************************************
 GetTreeList

 ******************************************************************************/

inline JTreeList*
JXTreeListWidget::GetTreeList()
{
	return itsTreeList;
}

inline const JTreeList*
JXTreeListWidget::GetTreeList()
	const
{
	return itsTreeList;
}

/******************************************************************************
 Column indices

 ******************************************************************************/

inline JIndex
JXTreeListWidget::GetToggleOpenColIndex()
	const
{
	return itsToggleOpenColIndex;
}

inline JIndex
JXTreeListWidget::GetNodeColIndex()
	const
{
	return itsNodeColIndex;
}

/******************************************************************************
 WantsToDrawCell

 ******************************************************************************/

inline JBoolean
JXTreeListWidget::WantsToDrawCell
	(
	const JPoint& cell
	)
	const
{
	return JI2B(JIndex(cell.x) == itsToggleOpenColIndex ||
				JIndex(cell.x) == itsNodeColIndex);
}

/******************************************************************************
 Elastic column index

	Set the index to zero to force all columns to their minimum width.

 ******************************************************************************/

inline JIndex
JXTreeListWidget::GetElasticColIndex()
	const
{
	return itsElasticColIndex;
}

inline void
JXTreeListWidget::SetElasticColIndex
	(
	const JIndex index
	)
{
	if (index != itsElasticColIndex)
		{
		itsElasticColIndex = index;
		NeedsAdjustToTree();
		}
}

/******************************************************************************
 Indent width

	This specified how far a nodes children should be indented relative
	to the node itself.

 ******************************************************************************/

inline JSize
JXTreeListWidget::GetIndentWidth()
	const
{
	return itsIndentWidth;
}

inline void
JXTreeListWidget::SetIndentWidth
	(
	const JSize width
	)
{
	if (width != itsIndentWidth)
		{
		itsIndentWidth = width;
		NeedsAdjustToTree();
		}
}

/******************************************************************************
 Draw selection (protected)

 ******************************************************************************/

inline JBoolean
JXTreeListWidget::WillDrawSelection()
	const
{
	return itsDrawSelectionFlag;
}

inline void
JXTreeListWidget::ShouldDrawSelection
	(
	const JBoolean draw
	)
{
	if (draw != itsDrawSelectionFlag)
		{
		itsDrawSelectionFlag = draw;
		Refresh();
		}
}

/******************************************************************************
 Max open depth

	This specifies the maximum depth to which a node should be opened
	when the arrow is meta-clicked.

 ******************************************************************************/

inline JSize
JXTreeListWidget::GetMaxOpenDepth()
	const
{
	return itsMaxOpenDepth;
}

inline void
JXTreeListWidget::SetMaxOpenDepth
	(
	const JSize maxDepth
	)
{
	itsMaxOpenDepth = maxDepth;
}

/******************************************************************************
 IsDraggingToggle (protected)

 ******************************************************************************/

inline JBoolean
JXTreeListWidget::IsDraggingToggle()
	const
{
	return JI2B(itsToggleDragIndex > 0);
}

/******************************************************************************
 DND target index (protected)

	Allows derived classes to indicate the drop target independent of the
	selection if the icon is darkened when selected.

 ******************************************************************************/

inline JBoolean
JXTreeListWidget::GetDNDTargetIndex
	(
	JIndex* index
	)
	const
{
	*index = itsDNDTargetIndex;
	return JI2B(itsDNDTargetIndex > 0);
}

inline void
JXTreeListWidget::SetDNDTargetIndex
	(
	const JIndex index
	)
{
	// use TableRefreshRow() as convenience to allow other changes

	if (itsDNDTargetIndex > 0)
		{
		TableRefreshRow(itsDNDTargetIndex);
		}
	itsDNDTargetIndex = index;
	if (itsDNDTargetIndex > 0)
		{
		TableRefreshRow(itsDNDTargetIndex);
		}
}

inline void
JXTreeListWidget::ClearDNDTargetIndex()
{
	SetDNDTargetIndex(0);
}

#endif
