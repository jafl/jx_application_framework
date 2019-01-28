/******************************************************************************
 SyGAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGAboutDialog.h"
#include "SyGGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

SyGAboutDialog::SyGAboutDialog
	(
	JXDirector*		supervisor,
	const JString&	prevVersStr
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsIsUpgradeFlag = kJFalse;

	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGAboutDialog::~SyGAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "SyGMainIcon.xpm"
#include <new_planet_software.xpm>

void
SyGAboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, JString::empty);
	assert( window != nullptr );

	JXImageWidget* sygIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( sygIcon != nullptr );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::SyGAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::SyGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SyGAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::SyGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SyGAboutDialog::shortcuts::JXLayout"));

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::SyGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != nullptr );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SyGAboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	// text

	JString text;
	text = SyGGetVersionStr();
	if (!prevVersStr.IsEmpty())
		{
		const JUtf8Byte* map[] =
			{
			"vers", prevVersStr.GetBytes()
			};
		text += JGetString("UpgradeNotice::SyGAboutDialog");
		JGetStringManager()->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::SyGAboutDialog"));
		itsIsUpgradeFlag = kJTrue;
		}
	textWidget->GetText()->SetText(text);

	// System G icon

	JXImage* image = jnew JXImage(GetDisplay(), SyGMainIcon);
	assert( image != nullptr );
	sygIcon->SetImage(image, kJTrue);

	// NPS icon

	image = jnew JXImage(GetDisplay(), new_planet_software);
	assert( image != nullptr );
	npsIcon->SetImage(image, kJTrue);

	// adjust window to fit text

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}
}

/******************************************************************************
 Receive (protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
SyGAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			JXGetHelpManager()->ShowChangeLog();
			}
		else
			{
			JXGetHelpManager()->ShowTOC();
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		JXGetHelpManager()->ShowCredits();
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
