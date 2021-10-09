/******************************************************************************
 JXMessageDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXMessageDialog.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextButton.h"
#include "jx-af/jx/JXStaticText.h"
#include "jx-af/jx/JXImageWidget.h"
#include "jx-af/jx/JXImage.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_un_message.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMessageDialog::JXMessageDialog
	(
	JXDirector*		supervisor,
	const JString&	message
	)
	:
	JXUNDialogBase(supervisor)
{
	BuildWindow(message);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMessageDialog::~JXMessageDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXMessageDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,110, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXMessageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 140,80, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXMessageDialog::shortcuts::JXLayout"));

	auto* text =
		jnew JXStaticText(JGetString("text::JXMessageDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXMessageDialog"));
	SetButtons(okButton, nullptr);

	Init(window, text, message, icon, jx_un_message);
}
