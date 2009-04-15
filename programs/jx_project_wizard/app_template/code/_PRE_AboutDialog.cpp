/******************************************************************************
 <PRE>AboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright © <Year> by <Company>. All rights reserved.

 ******************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>AboutDialog.h"
#include "<pre>Globals.h"
#include "<pre>HelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::<PRE>AboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::<PRE>AboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

<PRE>AboutDialog::<PRE>AboutDialog
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

<PRE>AboutDialog::~<PRE>AboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "<pre>_about_icon.xpm"

void
<PRE>AboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,120, "");
    assert( window != NULL );
    SetWindow(window);

    JXStaticText* textWidget =
        new JXStaticText(JGetString("textWidget::<PRE>AboutDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 280,50);
    assert( textWidget != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::<PRE>AboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 259,89, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::<PRE>AboutDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::<PRE>AboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,90, 60,20);
    assert( itsHelpButton != NULL );

    JXImageWidget* imageWidget =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
    assert( imageWidget != NULL );

    itsCreditsButton =
        new JXTextButton(JGetString("itsCreditsButton::<PRE>AboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,90, 60,20);
    assert( itsCreditsButton != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::<PRE>AboutDialog"));
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = new JXImage(GetDisplay(), GetColormap(), JXPM(<pre>_about_icon));
	assert( image != NULL );
	imageWidget->SetImage(image, kJTrue);

	JString text = <PRE>GetVersionStr();
	if (!JStringEmpty(prevVersStr))
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
<PRE>AboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(k<PRE>ChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(k<PRE>OverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(k<PRE>CreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
