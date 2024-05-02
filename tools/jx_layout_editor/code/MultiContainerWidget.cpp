/******************************************************************************
 MultiContainerWidget.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "MultiContainerWidget.h"
#include "LayoutContainer.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MultiContainerWidget::MultiContainerWidget
	(
	const bool			wantsInput,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(wantsInput, layout, hSizing, vSizing, x,y, w,h),
	itsParent(layout)
{
	MultiContainerWidgetX();
}

MultiContainerWidget::MultiContainerWidget
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(input, vers, layout, hSizing, vSizing, x,y, w,h),
	itsParent(layout)
{
	MultiContainerWidgetX();
}

// private

void
MultiContainerWidget::MultiContainerWidgetX()
{
	itsLayoutList = jnew JPtrArray<LayoutContainer>(JPtrArrayT::kForgetAll);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MultiContainerWidget::~MultiContainerWidget()
{
}

/******************************************************************************
 GetLayoutContainer (virtual)

 ******************************************************************************/

bool
MultiContainerWidget::GetLayoutContainer
	(
	const JIndex		index,
	LayoutContainer**	layout
	)
	const
{
	*layout = itsLayoutList->GetItem(index);
	return true;
}

/******************************************************************************
 GetLayoutContainerIndex (virtual)

 ******************************************************************************/

bool
MultiContainerWidget::GetLayoutContainerIndex
	(
	const LayoutWidget*	widget,
	JIndex*				index
	)
	const
{
	JIndex i = 1;
	for (auto* layout : *itsLayoutList)
	{
		if (layout->IsAncestor(widget))
		{
			*index = i;
			return true;
		}
		i++;
	}

	*index = 0;
	return false;
}

/******************************************************************************
 InsertLayoutContainer (protected)

 ******************************************************************************/

LayoutContainer*
MultiContainerWidget::InsertLayoutContainer
	(
	const JIndex	index,
	JXContainer*	enclosure
	)
{
	auto* layout = jnew LayoutContainer(itsParent, this, enclosure, kHElastic, kVElastic, 0,0, 100,100);
	layout->FitToEnclosure();

	itsLayoutList->InsertAtIndex(index, layout);
	ListenTo(layout);

	return layout;
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
MultiContainerWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	auto* layout = dynamic_cast<LayoutContainer*>(sender);
	itsLayoutList->Remove(layout);

	LayoutWidget::ReceiveGoingAway(sender);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
MultiContainerWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
MultiContainerWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawOver (virtual protected)

 ******************************************************************************/

void
MultiContainerWidget::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JString s = ToString();
	for (auto* layout : *itsLayoutList)
	{
		layout->SetHint(ToString());
	}

	LayoutWidget::DrawOver(p, rect);
}

/******************************************************************************
 PrintConfiguration (virtual protected)

	Don't generate assert, since we are guaranteed to be referenced.

 ******************************************************************************/

void
MultiContainerWidget::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
}
