/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "AboutDialog.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

AboutDialog::AboutDialog
	(
	const JString& prevVersStr
	)
	:
	JXModalDialogDirector()
{
	itsIsUpgradeFlag = false;

	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AboutDialog::~AboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
AboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,120, JGetString("WindowTitle::AboutDialog::JXLayout"));

	auto* imageWidget =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_about_icon
#define _H_about_icon
#include "about_icon.xpm"
#endif
	imageWidget->SetImage(GetDisplay()->GetImageCache()->GetImage(about_icon), false);

	auto* textWidget =
		jnew JXStaticText(JString::empty, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 290,50);
	assert( textWidget != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,90, 60,20);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,90, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::AboutDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 259,89, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::AboutDialog::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::AboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton, std::function([this](const JXButton::Pushed&)
	{
		if (itsIsUpgradeFlag)
		{
			JXGetHelpManager()->ShowChangeLog();
		}
		else
		{
			JXGetHelpManager()->ShowTOC();
		}
		EndDialog(true);
	}));

	ListenTo(itsCreditsButton, std::function([this](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowCredits();
		EndDialog(true);
	}));

	JString text = GetVersionStr();
	if (!prevVersStr.IsEmpty())
	{
		const JUtf8Byte* map[] =
		{
			"vers", prevVersStr.GetBytes()
		};
		text += JGetString("UpgradeNotice::AboutDialog");
		JGetStringManager()->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::AboutDialog"));
		itsIsUpgradeFlag = true;
	}
	textWidget->GetText()->SetText(text);

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
	{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
	}
}
