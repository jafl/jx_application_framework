/******************************************************************************
 JXTreeListWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXTreeListWidget
#define _H_JXTreeListWidget

#include "JXStyleTable.h"

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

	~JXTreeListWidget() override;

	JTreeList*			GetTreeList();
	const JTreeList*	GetTreeList() const;

	JIndex	GetToggleOpenColIndex() const;
	JIndex	GetNodeColIndex() const;

	void	ScrollToNode(const JTreeNode* node);
	void	ScrollToShowChildren(const JIndex index);
	void	GetSelectedNodes(JPtrArray<JTreeNode>* list) const;
	void	SelectNodes(const JPtrArray<JTreeNode>& list);
	void	OpenSelectedNodes(const bool openSiblings = false,
							  const bool openDescendants = false);
	void	CloseSelectedNodes(const bool closeSiblings = false,
							   const bool closeDescendants = false);
	void	ToggleNode(const JIndex index, const bool siblings = false,
					   const bool descendants = false);

	JSize			GetIndentWidth() const;
	void			SetIndentWidth(const JSize width);
	JSize			GetNodeIndent(const JIndex index) const;
	static JSize	GetDefaultIndentWidth();

	JSize	GetMaxOpenDepth() const;
	void	SetMaxOpenDepth(const JSize maxDepth);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;
	bool	IsSelectable(const JPoint& cell,
						 const bool forExtend) const override;
	bool	IsEditable(const JPoint& cell) const override;

	void	ForceAdjustToTree();

protected:

	bool	WillDrawSelection() const;
	void	ShouldDrawSelection(const bool draw);

	JIndex	GetElasticColIndex() const;
	void	SetElasticColIndex(const JIndex index);

	void			NeedsAdjustToTree();
	virtual void	AdjustToTree();
	virtual JSize	GetMinCellWidth(const JPoint& cell) const = 0;

	bool			WantsToDrawCell(const JPoint& cell) const;
	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual void	TLWDrawNode(JPainter& p, const JPoint& cell, const JRect& rect) = 0;

	bool	IsDraggingToggle() const;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	bool	GetDNDTargetIndex(JIndex* index) const;
	void	SetDNDTargetIndex(const JIndex index);
	void	ClearDNDTargetIndex();

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JTreeList*	itsTreeList;
	JIndex		itsToggleOpenColIndex;
	JIndex		itsNodeColIndex;
	JSize		itsIndentWidth;
	bool		itsDrawSelectionFlag;

	JArray<JSize>*	itsMinColWidths;		// minimum width of each column
	JIndex			itsElasticColIndex;		// column that can expand to fill aperture width

	JXTLWAdjustToTreeTask*	itsAdjustToTreeTask;	// can be nullptr

	// used during node move

	JPtrArray<JTreeNode>*	itsReselectNodeList;	// contents owned by JTree
	JString*				itsSavedScrollSetup;	// nullptr except during move

	// used while dragging

	bool		itsMouseInToggleFlag;
	JCoordinate	itsToggleDragIndex;		// 0 => off; JCoordinate to avoid signed-unsigned compare
	JSize		itsMaxOpenDepth;		// max depth when opening descendants

	JIndex		itsDNDTargetIndex;		// highlight arrow if > 0

private:

	void	AdjustColWidths();

	void	HandlePrepareForNodeMove();
	void	HandleNodeMoveFinished();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNodeDblClicked;

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

inline bool
JXTreeListWidget::WantsToDrawCell
	(
	const JPoint& cell
	)
	const
{
	return JIndex(cell.x) == itsToggleOpenColIndex ||
				JIndex(cell.x) == itsNodeColIndex;
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

inline bool
JXTreeListWidget::WillDrawSelection()
	const
{
	return itsDrawSelectionFlag;
}

inline void
JXTreeListWidget::ShouldDrawSelection
	(
	const bool draw
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

inline bool
JXTreeListWidget::IsDraggingToggle()
	const
{
	return itsToggleDragIndex > 0;
}

/******************************************************************************
 DND target index (protected)

	Allows derived classes to indicate the drop target independent of the
	selection if the icon is darkened when selected.

 ******************************************************************************/

inline bool
JXTreeListWidget::GetDNDTargetIndex
	(
	JIndex* index
	)
	const
{
	*index = itsDNDTargetIndex;
	return itsDNDTargetIndex > 0;
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
