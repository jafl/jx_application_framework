/******************************************************************************
 TestPopupChoiceDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestPopupChoiceDialog.h"
#include "SmileyBitmaps.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFontNameMenu.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPopupChoiceDialog::TestPopupChoiceDialog()
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 230,170, JGetString("WindowTitle::TestPopupChoiceDialog::JXLayout"));

	auto* fontMenu =
		jnew JXFontNameMenu(JGetString("FontMenuLabel::TestPopupChoiceDialog"), true,window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 190,30);
	assert( fontMenu != nullptr );

	auto* sizeMenu =
		jnew JXFontSizeMenu(fontMenu, JGetString("FontSizeMenuLabel::TestPopupChoiceDialog"),window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,60, 190,30);
	assert( sizeMenu != nullptr );

	auto* iconMenu =
		jnew JXImageMenu(JGetString("ImageMenuLabel::TestPopupChoiceDialog"), 2,window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,100, 190,30);
	assert( iconMenu != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestPopupChoiceDialog::JXLayout"),window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,140, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::TestPopupChoiceDialog::JXLayout"),window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 129,139, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::TestPopupChoiceDialog::JXLayout"));

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

	JXImage* titleImage = nullptr;
	for (JUnsignedOffset i=0; i<kSmileyBitmapCount; i++)
	{
		JXImage* image = jnew JXImage(display, kSmileyBitmap[i], kSmileyColor[i]);
		iconMenu->AppendItem(image, true);

		if (i == 0)
		{
			titleImage = image;
		}
	}

	// set title to image so SetToPopupChoice() will change the title
	iconMenu->SetTitle(JString::empty, titleImage, false);

	iconMenu->SetUpdateAction(JXMenu::kDisableNone);
	iconMenu->SetToPopupChoice(true, 1);
	iconMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);

	// We should actually set all the items on iconMenu to be radio buttons
	// and then ListenTo() it so we know what the selection is, but this window
	// does nothing, so we don't bother.
}
