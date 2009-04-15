/******************************************************************************
 JXVarLenPGDirector.cpp

	BASE CLASS = JXPGDirectorBase

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXVarLenPGDirector.h>
#include <JXProgressDisplay.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVarLenPGDirector::JXVarLenPGDirector
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
	const JCharacter*	message,
	const JBoolean		allowCancel
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 270,80, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXVarLenPGDirector::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,40, 60,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::JXVarLenPGDirector::shortcuts::JXLayout"));

    JXStaticText* text =
        new JXStaticText(JGetString("text::JXVarLenPGDirector::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 230,20);
    assert( text != NULL );

    JXStaticText* counter =
        new JXStaticText(JGetString("counter::JXVarLenPGDirector::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 60,40, 100,20);
    assert( counter != NULL );

// end JXLayout

	Init(window, text, message, allowCancel, cancelButton);

	pg->SetItems(cancelButton, counter, NULL);
}
