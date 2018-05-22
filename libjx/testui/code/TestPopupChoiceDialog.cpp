/******************************************************************************
 TestPopupChoiceDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestPopupChoiceDialog.h"
#include "SmileyBitmaps.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXImageMenu.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPopupChoiceDialog::TestPopupChoiceDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPopupChoiceDialog::~TestPopupChoiceDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestPopupChoiceDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 230,170, JString::empty);
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestPopupChoiceDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,140, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::TestPopupChoiceDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,140, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::TestPopupChoiceDialog::shortcuts::JXLayout"));

	JXFontNameMenu* fontMenu =
		jnew JXFontNameMenu(JGetString("FontMenuLabel::TestPopupChoiceDialog"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 190,30);
	assert( fontMenu != NULL );

	JXFontSizeMenu* sizeMenu =
		jnew JXFontSizeMenu(fontMenu, JGetString("FontSizeMenuLabel::TestPopupChoiceDialog"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 190,30);
	assert( sizeMenu != NULL );

	JXImageMenu* iconMenu =
		jnew JXImageMenu(JGetString("ImageMenuLabel::TestPopupChoiceDialog"), 2, window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 190,30);
	assert( iconMenu != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestPopupChoiceDialog"));
	SetButtons(okButton, cancelButton);

	fontMenu->SetToPopupChoice();

	sizeMenu->SetToPopupChoice();
	sizeMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);

	// set up icon menu

	JXDisplay* display = window->GetDisplay();

	static const JColorID kSmileyColor[] =
	{
		JColorManager::GetWhiteColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetBlueColor(),
		JColorManager::GetBlackColor()
	};

	JXImage* titleImage = NULL;
	for (JIndex i=0; i<kSmileyBitmapCount; i++)
		{
		JXImage* image = jnew JXImage(display, kSmileyBitmap[i], kSmileyColor[i]);
		assert( image != NULL );
		iconMenu->AppendItem(image, kJTrue);

		if (i == 0)
			{
			titleImage = image;
			}
		}

	// set title to image so SetToPopupChoice() will change the title
	iconMenu->SetTitle(NULL, titleImage, kJFalse);

	iconMenu->SetUpdateAction(JXMenu::kDisableNone);
	iconMenu->SetToPopupChoice(kJTrue, 1);
	iconMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);

	// We should actually set all the items on iconMenu to be radio buttons
	// and then ListenTo() it so we know what the selection is, but this window
	// does nothing, so we don't bother.
}
