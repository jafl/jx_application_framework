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
    SetWindow(window);

    JXImageWidget* icon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
    assert( icon != NULL );

    JXStaticText* text =
        new JXStaticText("", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 280,50);
    assert( text != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 159,79, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

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
