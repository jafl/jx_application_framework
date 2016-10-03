/******************************************************************************
 GNBAboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GNBAboutDialog.h"
#include <GNBMainIcon.xpm>
#include <GNBHelpText.h>

#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>

#include <jXGlobals.h>

#include <jAssert.h>

extern const JCharacter* kNotebookVersionStr;

static const JCharacter* kUpgradeNotice     = "\n\n\nYou have just upgraded from version ";
static const JCharacter* kChangeButtonLabel = "Changes";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GNBAboutDialog::GNBAboutDialog
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

GNBAboutDialog::~GNBAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GNBAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 350,170, "");
	assert( window != NULL );

	JXImageWidget* imageWidget =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,15, 40,40);
	assert( imageWidget != NULL );

	JXStaticText* textWidget =
		new JXStaticText("", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,15, 280,110);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		new JXTextButton("OK", window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 209,139, 62,22);
	assert( okButton != NULL );
	okButton->SetShortcuts("^M");

	itsHelpButton =
		new JXTextButton("Help", window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,140, 60,20);
	assert( itsHelpButton != NULL );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);

	JXImage* image = new JXImage(GetDisplay(), JXPM(GNBMainIcon));
	assert(image != NULL);

	imageWidget->SetImage(image,kJTrue);

	JString text = kNotebookVersionStr;
	if (!JStringEmpty(prevVersStr))
		{
		text += kUpgradeNotice;
		text += prevVersStr;
		itsHelpButton->SetLabel(kChangeButtonLabel);
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
 Receive (protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
GNBAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kChangeLogName);
			}
		else
			{
			// should really be table of contents

			(JXGetHelpManager())->ShowTOC();
			}
		EndDialog(kJFalse);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
