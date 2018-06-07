/******************************************************************************
 CBAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "CBAboutDialog.h"
#include "CBAboutDialogIconTask.h"
#include "cbGlobals.h"
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

CBAboutDialog::CBAboutDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	prevVersStr
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsIsUpgradeFlag(kJFalse)
{
	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBAboutDialog::~CBAboutDialog()
{
	jdelete itsAnimTask;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <new_planet_software.xpm>

void
CBAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, "");
	assert( window != nullptr );

	JXImageWidget* jccIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( jccIcon != nullptr );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::CBAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBAboutDialog::shortcuts::JXLayout"));

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != nullptr );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != nullptr );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	// text

	JString text = CBGetVersionStr();
	if (!JString::IsEmpty(prevVersStr))
		{
		const JCharacter* map[] =
			{
			"vers", prevVersStr
			};
		text += JGetString("UpgradeNotice::CBAboutDialog");
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::CBAboutDialog"));
		itsIsUpgradeFlag = kJTrue;
		}
	textWidget->SetText(text);

	// Code Crusader icon

	itsAnimTask = jnew CBAboutDialogIconTask(jccIcon);
	assert( itsAnimTask != nullptr );
	itsAnimTask->Start();

	// NPS icon

	JXImage* image = jnew JXImage(GetDisplay(), new_planet_software);
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

	Handle extra buttons.

 ******************************************************************************/

void
CBAboutDialog::Receive
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
