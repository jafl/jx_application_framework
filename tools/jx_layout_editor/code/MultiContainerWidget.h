/******************************************************************************
 MultiContainerWidget.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_MultiContainerWidget
#define _H_MultiContainerWidget

#include "LayoutWidget.h"
#include <jx-af/jcore/JPtrArray.h>

class MultiContainerWidget : public LayoutWidget
{
public:

	MultiContainerWidget(const bool wantsInput, LayoutContainer* layout,
						 const HSizingOption hSizing, const VSizingOption vSizing,
						 const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);
	MultiContainerWidget(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
						 const HSizingOption hSizing, const VSizingOption vSizing,
						 const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	~MultiContainerWidget() override;

	bool	GetLayoutContainer(const JIndex index, LayoutContainer** layout) const override;
	bool	GetLayoutContainerIndex(const LayoutWidget* widget, JIndex* index) const override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

	LayoutContainer*	GetLayoutContainer(const JIndex index) const;
	LayoutContainer*	InsertLayoutContainer(const JIndex index, JXContainer* enclosure);
	bool				OwnsLayoutContainer(const LayoutContainer* layout);

	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	LayoutContainer*			itsParent;
	JPtrArray<LayoutContainer>*	itsLayoutList;

private:

	void	MultiContainerWidgetX();
};


/******************************************************************************
 GetLayoutContainer (protected)

 ******************************************************************************/

inline LayoutContainer*
MultiContainerWidget::GetLayoutContainer
	(
	const JIndex index
	)
	const
{
	return itsLayoutList->GetItem(index);
}

/******************************************************************************
 OwnsLayoutContainer

 ******************************************************************************/

inline bool
MultiContainerWidget::OwnsLayoutContainer
	(
	const LayoutContainer* layout
	)
{
	return itsLayoutList->Includes(layout);
}

#endif
