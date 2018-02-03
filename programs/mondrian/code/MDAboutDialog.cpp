/******************************************************************************
 MDAboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "MDAboutDialog.h"
#include "mdGlobals.h"
#include "mdHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::MDAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::MDAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

MDAboutDialog::MDAboutDialog
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
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,200, "");
	assert( window != NULL );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::MDAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,130);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,170, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::MDAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,170, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::MDAboutDialog::shortcuts::JXLayout"));

	JXImageWidget* imageWidget =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( imageWidget != NULL );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::MDAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,170, 60,20);
	assert( itsCreditsButton != NULL );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDAboutDialog"));
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = jnew JXImage(GetDisplay(), JXPM(md_about_icon));
	assert( image != NULL );
	imageWidget->SetImage(image, kJTrue);

	JString text = MDGetVersionStr();
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
			(JXGetHelpManager())->ShowSection(kMDChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kMDOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kMDCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
