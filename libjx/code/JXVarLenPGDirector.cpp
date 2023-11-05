/******************************************************************************
 JXVarLenPGDirector.cpp

	BASE CLASS = JXPGDirectorBase

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JXVarLenPGDirector.h"
#include "JXProgressDisplay.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVarLenPGDirector::JXVarLenPGDirector
	(
	JXProgressDisplay*	pg,
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
	:
	JXPGDirectorBase(modal)
{
	BuildWindow(pg, message, allowCancel);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXVarLenPGDirector::~JXVarLenPGDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXVarLenPGDirector::BuildWindow
	(
	JXProgressDisplay*	pg,
	const JString&		message,
	const bool			allowCancel
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 270,80, JString::empty);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXVarLenPGDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,40, 60,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXVarLenPGDirector::shortcuts::JXLayout"));

	auto* text =
		jnew JXStaticText(JGetString("text::JXVarLenPGDirector::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 230,20);
	assert( text != nullptr );
	text->SetToLabel();

	auto* counter =
		jnew JXStaticText(JGetString("counter::JXVarLenPGDirector::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,40, 100,20);
	assert( counter != nullptr );
	counter->SetToLabel();

// end JXLayout

	Init(window, text, message, allowCancel, cancelButton);

	pg->SetItems(cancelButton, counter, nullptr);
}
