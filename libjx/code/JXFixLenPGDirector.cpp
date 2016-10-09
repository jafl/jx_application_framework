/******************************************************************************
 JXFixLenPGDirector.cpp

	BASE CLASS = JXPGDirectorBase

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.
	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXFixLenPGDirector.h>
#include <JXProgressDisplay.h>
#include <JXProgressIndicator.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFixLenPGDirector::JXFixLenPGDirector
	(
	JXDirector*			supervisor,
	JXProgressDisplay*	pg,
	const JCharacter*	message,
	const JBoolean		allowCancel
	)
	:
	JXPGDirectorBase(supervisor)
{
	BuildWindow(pg, message, allowCancel);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFixLenPGDirector::~JXFixLenPGDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXFixLenPGDirector::BuildWindow
	(
	JXProgressDisplay*	pg,
	const JCharacter*	message,
	const JBoolean		allowCancel
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 270,80, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXFixLenPGDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,10, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXFixLenPGDirector::shortcuts::JXLayout"));

	JXStaticText* text =
		jnew JXStaticText(JGetString("text::JXFixLenPGDirector::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 160,20);
	assert( text != NULL );
	text->SetToLabel();

	JXProgressIndicator* indicator =
		jnew JXProgressIndicator(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,50, 230,10);
	assert( indicator != NULL );

// end JXLayout

	if (!allowCancel)
		{
		// expand text to fill window because Cancel button will be hidden

		text->AdjustSize(indicator->GetFrameWidth() - text->GetFrameWidth(), 0);
		}

	Init(window, text, message, allowCancel, cancelButton);

	pg->SetItems(cancelButton, NULL, indicator);
}
