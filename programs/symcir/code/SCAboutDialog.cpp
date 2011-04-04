/******************************************************************************
 SCAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCAboutDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

extern const JCharacter* kSymcirVersionStr;

/******************************************************************************
 Constructor

 ******************************************************************************/

SCAboutDialog::SCAboutDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCAboutDialog::~SCAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "sc_about.xpm"

void
SCAboutDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,110, "");
	assert( window != NULL );

	JXImageWidget* icon =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != NULL );

	JXStaticText* text =
		new JXStaticText(JGetString("text::SCAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 280,50);
	assert( text != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SCAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 160,80, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SCAboutDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	text->SetText(kSymcirVersionStr);

	JXImage* image = new JXImage(GetDisplay(), window->GetColormap(), sc_about);
	assert( image != NULL );
	image->SetDefaultState(JXImage::kRemoteStorage);
	image->ConvertToDefaultState();
	icon->SetImage(image, kJTrue);

	const JSize bdh = text->GetBoundsHeight();
	const JSize aph = text->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}
}
