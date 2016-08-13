/******************************************************************************
 GLAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GLAboutDialog.h"
#include <GLHelpText.h>

#include "glove_icon.xpm"
#include "nps.xpm"

#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXDisplay.h>
#include <JXHelpManager.h>

#include <jXGlobals.h>

#include <jAssert.h>

extern const JCharacter* kGloveVersionStr;

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::GLAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::GLAboutDialog";
static const JCharacter* kCopyright			  = "COPYRIGHT";
static const JCharacter* kDescriptionID		  = "GLDescription";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GLAboutDialog::GLAboutDialog
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

GLAboutDialog::~GLAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 430,180, "");
    assert( window != NULL );

    JXImageWidget* gloveIcon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,15, 50,50);
    assert( gloveIcon != NULL );

    JXStaticText* textWidget =
        new JXStaticText(JGetString("textWidget::GLAboutDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
    assert( textWidget != NULL );

    JXImageWidget* npsIcon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
    assert( npsIcon != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::GLAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::GLAboutDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GLAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
    assert( itsHelpButton != NULL );

    itsCreditsButton =
        new JXTextButton(JGetString("itsCreditsButton::GLAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
    assert( itsCreditsButton != NULL );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	const JCharacter* map1[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	JString text	= JGetString(kDescriptionID, map1, sizeof(map1));

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

	JXImage* image = new JXImage(GetDisplay(), JXPM(glove_icon));
	assert(image != NULL);
	gloveIcon->SetImage(image, kJTrue);

	// NPS icon

	image = new JXImage(GetDisplay(), nps);
	assert( image != NULL );
	npsIcon->SetImage(image, kJTrue);

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
GLAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kGLChangeLogName);
			}
		else
			{
			// should really be table of contents

			(JXGetHelpManager())->ShowSection(kGLTOCHelpName);
			}
		EndDialog(kJFalse);
		}
	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGLCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
