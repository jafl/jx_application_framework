/******************************************************************************
 THXAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXAboutDialog.h"
#include "thxHelpText.h"
#include "thxGlobals.h"
#include <JXApplication.h>
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::THXAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::THXAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

THXAboutDialog::THXAboutDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	prevVersStr
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

THXAboutDialog::~THXAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_about.xpm"
#include <new_planet_software.xpm>

void
THXAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, "");
	assert( window != NULL );

	JXImageWidget* thxIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( thxIcon != NULL );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::THXAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::THXAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::THXAboutDialog::shortcuts::JXLayout"));

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::THXAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::THXAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::THXAboutDialog::shortcuts::JXLayout"));

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	// text

	JString text = THXGetVersionStr();
	if (!JString::IsEmpty(prevVersStr))
		{
		const JCharacter* map[] =
			{
			"vers", prevVersStr
			};
		text += JGetString(kUpgradeNoticeID);
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString(kChangeButtonLabelID));
		itsIsUpgradeFlag = kJTrue;
		}
	textWidget->SetText(text);

	// Program icon

	JXImage* image = jnew JXImage(GetDisplay(), thx_about);
	assert( image != NULL );
	thxIcon->SetImage(image, kJTrue);

	// NPS icon

	image = jnew JXImage(GetDisplay(), new_planet_software);
	assert( image != NULL );
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
THXAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kTHXChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kTHXOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kTHXCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
