/******************************************************************************
 MDAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "MDAboutDialog.h"
#include "mdGlobals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

MDAboutDialog::MDAboutDialog
	(
	JXDirector*		supervisor,
	const JString&	prevVersStr
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsIsUpgradeFlag = false;

	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDAboutDialog::~MDAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "md_about_icon.xpm"

void
MDAboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,120, JString::empty);
	assert( window != nullptr );

	auto* textWidget =
		jnew JXStaticText(JGetString("textWidget::MDAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 280,50);
	assert( textWidget != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,90, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::MDAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,90, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::MDAboutDialog::shortcuts::JXLayout"));

	auto* imageWidget =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( imageWidget != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,90, 60,20);
	assert( itsCreditsButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDAboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	imageWidget->SetXPM(md_about_icon);

	JString text = MDGetVersionStr();
	if (!prevVersStr.IsEmpty())
	{
		const JUtf8Byte* map[] =
		{
			"vers", prevVersStr.GetBytes()
		};
		text += JGetString("UpgradeNotice::MDAboutDialog");
		JGetStringManager()->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::MDAboutDialog"));
		itsIsUpgradeFlag = true;
	}
	textWidget->GetText()->SetText(text);

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
	{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
	}
}

/******************************************************************************
 Receive (virtual protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
MDAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
		if (itsIsUpgradeFlag)
		{
			(JXGetHelpManager())->ShowChangeLog();
		}
		else
		{
			(JXGetHelpManager())->ShowSection("MDOverviewHelp");
		}
		EndDialog(true);
	}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
	{
		(JXGetHelpManager())->ShowCredits();
		EndDialog(true);
	}

	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}
