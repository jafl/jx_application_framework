/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "AboutDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImage.h>
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

#include "md_about_icon.xpm"

void
AboutDialog::BuildWindow
	(
	const JString& prevVersStr
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,120, JString::empty);

	auto* textWidget =
		jnew JXStaticText(JGetString("textWidget::AboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 300,50);
	assert( textWidget != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,90, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::AboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,90, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::AboutDialog::shortcuts::JXLayout"));

	auto* imageWidget =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( imageWidget != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::AboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,90, 60,20);
	assert( itsCreditsButton != nullptr );

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
			JXGetHelpManager()->ShowSection("OverviewHelp");
		}
		EndDialog(true);
	}));

	ListenTo(itsCreditsButton, std::function([this](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowCredits();
		EndDialog(true);
	}));

	imageWidget->SetXPM(md_about_icon);

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
