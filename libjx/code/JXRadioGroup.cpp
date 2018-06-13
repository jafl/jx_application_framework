/******************************************************************************
 JXRadioGroup.cpp

	Maintains a group of JXRadioButtons.  If you don't like the border that
	is provided, call SetBorderWidth(0) and put this widget inside a
	JXDecorRect.

	BASE CLASS = JXWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXRadioGroup.h>
#include <JXRadioButton.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

const JSize kFrameDownWidth    = 1;
const JSize kFrameBetweenWidth = 0;
const JSize kFrameUpWidth      = 1;

// JBroadcaster message types

const JUtf8Byte* JXRadioGroup::kSelectionChanged = "SelectionChanged::JXRadioGroup";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRadioGroup::JXRadioGroup
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsButtons = jnew JPtrArray<JXRadioButton>(JPtrArrayT::kForgetAll);
	assert( itsButtons != nullptr );

	itsSelection = nullptr;

	SetBorderWidth(kFrameDownWidth + kFrameUpWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRadioGroup::~JXRadioGroup()
{
	jdelete itsButtons;		// JXEnclosure deletes objects
}

/******************************************************************************
 GetSelectedItem

 ******************************************************************************/

JIndex
JXRadioGroup::GetSelectedItem()
	const
{
	assert( itsSelection != nullptr );
	return itsSelection->GetID();
}

/******************************************************************************
 SelectItem

 ******************************************************************************/

void
JXRadioGroup::SelectItem
	(
	const JIndex id
	)
{
	assert( itsSelection != nullptr );

	if (id != itsSelection->GetID())
		{
		JXRadioButton** newButton =
			std::find_if(begin(*itsButtons), end(*itsButtons),
				[id] (JXRadioButton* b) { return b->GetID() == id; });
		assert( newButton != end(*itsButtons) );

		NewSelection(*newButton);
		}
}

/******************************************************************************
 NewSelection (private)

 ******************************************************************************/

void
JXRadioGroup::NewSelection
	(
	JXRadioButton* button
	)
{
	assert( button != nullptr );

	if (itsSelection != button)
		{
		if (itsSelection != nullptr)
			{
			itsSelection->Deselect();
			}
		itsSelection = button;
		itsSelection->Select();
		Broadcast(SelectionChanged(itsSelection->GetID()));
		}
}

/******************************************************************************
 GetRadioButton

	Returns the JXRadioButton with the given ID, if it exists.

 ******************************************************************************/

JBoolean
JXRadioGroup::GetRadioButton
	(
	const JIndex	id,
	JXRadioButton**	rb
	)
	const
{
	JXRadioButton**	rb1 =
		std::find_if(begin(*itsButtons), end(*itsButtons),
			[id] (JXRadioButton* b) { return b->GetID() == id; });

	if (rb1 == end(*itsButtons))
		{
		*rb = nullptr;
		return kJFalse;
		}
	else
		{
		*rb = *rb1;
		return kJTrue;
		}
}

/******************************************************************************
 NewButton (private)

	Called when a new JXRadioButton is created.

 ******************************************************************************/

void
JXRadioGroup::NewButton
	(
	JXRadioButton* button
	)
{
	itsButtons->Append(button);
	if (itsSelection == nullptr)
		{
		NewSelection(button);
		}
}

/******************************************************************************
 Draw (virtual protected)

	There is nothing to do since we only act as a container.

 ******************************************************************************/

void
JXRadioGroup::Draw
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
JXRadioGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEngravedFrame(p, frame, kFrameDownWidth, kFrameBetweenWidth, kFrameUpWidth);
}

/******************************************************************************
 NeedsInternalFTC (virtual protected)

	Return kJTrue if the contents are a set of widgets that need to expand.

 ******************************************************************************/

JBoolean
JXRadioGroup::NeedsInternalFTC()
	const
{
	return kJTrue;
}
