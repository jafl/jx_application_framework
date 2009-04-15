/******************************************************************************
 JXRadioGroup.cpp

	Maintains a group of JXRadioButtons.  If you don't like the border that
	is provided, call SetBorderWidth(0) and put this widget inside a
	JXDecorRect.

	BASE CLASS = JXWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXRadioGroup.h>
#include <JXRadioButton.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

const JSize kFrameDownWidth    = 1;
const JSize kFrameBetweenWidth = 0;
const JSize kFrameUpWidth      = 1;

// JBroadcaster message types

const JCharacter* JXRadioGroup::kSelectionChanged = "SelectionChanged::JXRadioGroup";

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
	itsButtons = new JPtrArray<JXRadioButton>(JPtrArrayT::kForgetAll);
	assert( itsButtons != NULL );

	itsSelection = NULL;

	SetBorderWidth(kFrameDownWidth + kFrameUpWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRadioGroup::~JXRadioGroup()
{
	delete itsButtons;		// JXEnclosure deletes objects
}

/******************************************************************************
 GetSelectedItem

 ******************************************************************************/

JIndex
JXRadioGroup::GetSelectedItem()
	const
{
	assert( itsSelection != NULL );
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
	assert( itsSelection != NULL );

	if (id != itsSelection->GetID())
		{
		JXRadioButton* newButton = NULL;
		const JSize count = itsButtons->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXRadioButton* aButton = itsButtons->NthElement(i);
			if (aButton->GetID() == id)
				{
				newButton = aButton;
				break;
				}
			}
		assert( newButton != NULL );

		NewSelection(newButton);
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
	assert( button != NULL );

	if (itsSelection != button)
		{
		if (itsSelection != NULL)
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
	const JSize count = itsButtons->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXRadioButton* aButton = itsButtons->NthElement(i);
		if (aButton->GetID() == id)
			{
			*rb = aButton;
			return kJTrue;
			}
		}

	*rb = NULL;
	return kJFalse;
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
	if (itsSelection == NULL)
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
