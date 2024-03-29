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

	~JXNamedTreeListWidget() override;

	JNamedTreeList*			GetNamedTreeList();
	const JNamedTreeList*	GetNamedTreeList() const;

	JNamedTreeNode*			GetNamedTreeNode(const JIndex index);
	const JNamedTreeNode*	GetNamedTreeNode(const JIndex index) const;

	bool	WillHilightTextOnly() const;
	void	ShouldHilightTextOnly(const bool textOnly);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;
	void	ClearIncrementalSearchBuffer();

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

	bool	GetNode(const JPoint& pt, JPoint* cell, NodePart* part) const;
	bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	void	AdjustToTree() override;
	JSize	GetMinCellWidth(const JPoint& cell) const override;

	void			TLWDrawNode(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual bool	GetImage(const JIndex index, const JXImage** image) const;
	JSize			GetImageWidth(const JIndex index) const;
	bool			GetImageRect(const JIndex index, JRect* rect) const;

	JSize	GetTextWidth(const JIndex index) const;
	JRect	GetTextRect(const JIndex index) const;
	JRect	GetNodeRect(const JIndex index) const;

	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;
	void			PlaceInputField(const JCoordinate x, const JCoordinate y) override;
	void			SetInputFieldSize(const JCoordinate w, const JCoordinate h) override;
	JCoordinate		GetInputFieldIndent(const JIndex index) const;

	virtual JXInputField*
		CreateTreeListInput(const JPoint& cell, JXContainer* enclosure,
							const HSizingOption hSizing, const VSizingOption vSizing,
							const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	void	HandleFocusEvent() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JNamedTreeList*	itsNamedTreeList;	// owned by base class
	JString			itsKeyBuffer;
	bool			itsHilightTextOnlyFlag;
	JXInputField*	itsNameInputField;
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

inline bool
JXNamedTreeListWidget::WillHilightTextOnly()
	const
{
	return itsHilightTextOnlyFlag;
}

inline void
JXNamedTreeListWidget::ShouldHilightTextOnly
	(
	const bool textOnly
	)
{
	itsHilightTextOnlyFlag = textOnly;
	ShouldDrawSelection(!itsHilightTextOnlyFlag);
}

#endif
