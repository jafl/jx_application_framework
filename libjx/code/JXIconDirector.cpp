/******************************************************************************
 JXIconDirector.cpp

	Maintains a window for displaying an icon when the real window is
	iconified.

	BASE CLASS = JXWindowDirector

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXIconDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowIcon.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXIconDirector::JXIconDirector
	(
	JXWindowDirector*	supervisor,
	JXImage*			normalIcon,
	JXImage*			dropIcon
	)
	:
	JXWindowDirector(supervisor)
{
	JXWindow* window = new JXWindow(this, normalIcon->GetWidth(),
									normalIcon->GetHeight(), "");
	assert( window != NULL );
	SetWindow(window);

	itsIconWidget =
		new JXWindowIcon(normalIcon, dropIcon, supervisor->GetWindow(), window,
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsIconWidget != NULL );
	itsIconWidget->FitToEnclosure();

	// window is mapped when main window is iconified -- director never activated

	window->Activate();

	// avoid mapping the X window

	window->JXContainer::Show();

	// keep JXWindow::HandleMapNotify() happy

	JXDisplay* display = GetDisplay();
	long data = NormalState;
	XChangeProperty(*display, window->GetXWindow(),
					display->GetWMStateXAtom(),
					display->GetWMStateXAtom(), 32,
					PropModeReplace,
					(unsigned char*) &data, 1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXIconDirector::~JXIconDirector()
{
}
