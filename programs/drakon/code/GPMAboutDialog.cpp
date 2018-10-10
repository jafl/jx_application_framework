/******************************************************************************
 GPMAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2001 by Glenn W. Bach.

 ******************************************************************************/

#include "GPMAboutDialog.h"
#include "gpmGlobals.h"
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

GPMAboutDialog::GPMAboutDialog
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

GPMAboutDialog::~GPMAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "gpm_about_icon.xpm"
#include <new_planet_software.xpm>

void
GPMAboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, JString::empty);
	assert( window != nullptr );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::GPMAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GPMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GPMAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::GPMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != nullptr );

	JXImageWidget* gpmIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( gpmIcon != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::GPMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != nullptr );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GPMAboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = jnew JXImage(GetDisplay(), gpm_about_icon);
	assert( image != nullptr );
	gpmIcon->SetImage(image, kJTrue);

	image = jnew JXImage(GetDisplay(), new_planet_software);
	assert( image != nullptr );
	npsIcon->SetImage(image, kJTrue);

	JString text = GPMGetVersionStr();
	if (!prevVersStr.IsEmpty())
		{
		const JUtf8Byte* map[] =
			{
			"vers", prevVersStr.GetBytes()
			};
		text += JGetString("UpgradeNotice::GPMAboutDialog");
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::GPMAboutDialog"));
		itsIsUpgradeFlag = kJTrue;
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
GPMAboutDialog::Receive
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
			(JXGetHelpManager())->ShowTOC();
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowCredits();
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
