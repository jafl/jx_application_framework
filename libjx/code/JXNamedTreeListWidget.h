/******************************************************************************
 JXNamedTreeListWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXNamedTreeListWidget
#define _H_JXNamedTreeListWidget

#include "JXTreeListWidget.h"

class JNamedTreeList;
class JNamedTreeNode;
class JXImage;

class JXNamedTreeListWidget : public JXTreeListWidget
{
public:

	JXNamedTreeListWidget(JNamedTreeList* treeList,
						  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	virtual ~JXNamedTreeListWidget();

	JNamedTreeList*			GetNamedTreeList();
	const JNamedTreeList*	GetNamedTreeList() const;

	JNamedTreeNode*			GetNamedTreeNode(const JIndex index);
	const JNamedTreeNode*	GetNamedTreeNode(const JIndex index) const;

	JBoolean	WillHilightTextOnly() const;
	void		ShouldHilightTextOnly(const JBoolean textOnly);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;
	void			ClearIncrementalSearchBuffer();

protected:

	enum NodePart
	{
		kToggleColumn,
		kBeforeImage,
		kInImage,
		kInText,
		kAfterText,
		kOtherColumn
	};

protected:

	JBoolean			GetNode(const JPoint& pt, JPoint* cell, NodePart* part) const;
	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	virtual void		AdjustToTree() override;
	virtual JSize		GetMinCellWidth(const JPoint& cell) const override;

	virtual void		TLWDrawNode(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JBoolean	GetImage(const JIndex index, const JXImage** image) const;
	JSize				GetImageWidth(const JIndex index) const;
	JBoolean			GetImageRect(const JIndex index, JRect* rect) const;

	JSize	GetTextWidth(const JIndex index) const;
	JRect	GetTextRect(const JIndex index) const;
	JRect	GetNodeRect(const JIndex index) const;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual JBoolean		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;
	virtual void			PlaceInputField(const JCoordinate x, const JCoordinate y) override;
	virtual void			SetInputFieldSize(const JCoordinate w, const JCoordinate h) override;
	JCoordinate				GetInputFieldIndent(const JIndex index) const;

	virtual JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual void	HandleFocusEvent() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JNamedTreeList*	itsNamedTreeList;	// owned by base class
	JString			itsKeyBuffer;
	JSize			itsMaxNodeWidth;
	JBoolean		itsHilightTextOnlyFlag;
	JXInputField*	itsNameInputField;

private:

	// not allowed

	JXNamedTreeListWidget(const JXNamedTreeListWidget& source);
	const JXNamedTreeListWidget& operator=(const JXNamedTreeListWidget& source);
};


/******************************************************************************
 GetNamedTreeList

 ******************************************************************************/

inline JNamedTreeList*
JXNamedTreeListWidget::GetNamedTreeList()
{
	return itsNamedTreeList;
}

inline const JNamedTreeList*
JXNamedTreeListWidget::GetNamedTreeList()
	const
{
	return itsNamedTreeList;
}

/******************************************************************************
 Hilight text only

	If this is turned on, only the text will be highlighted, not
	the entire cell.

 ******************************************************************************/

inline JBoolean
JXNamedTreeListWidget::WillHilightTextOnly()
	const
{
	return itsHilightTextOnlyFlag;
}

inline void
JXNamedTreeListWidget::ShouldHilightTextOnly
	(
	const JBoolean textOnly
	)
{
	itsHilightTextOnlyFlag = textOnly;
	ShouldDrawSelection(!itsHilightTextOnlyFlag);
}

#endif
