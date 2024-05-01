/******************************************************************************
 JXTipOfTheDayDialog.cpp

	Parses and displays the contents of the TIP_OF_THE_DAY string resource.
	The tips are HTML, and they are separated by <div>.

	If desired, a "Show tips at startup" checkbox can be displayed.  This
	is off by default since it merely annoys most users.  If the checkbox
	is displayed, the dialog will broadcast the result when it is closed.

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include "JXTipOfTheDayDialog.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXScrollbarSet.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXFlatRect.h"
#include "JXImageCache.h"
#include "JXImage.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jTextUtil.h>
#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDelimiter = "=====";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTipOfTheDayDialog::JXTipOfTheDayDialog
	(
	const bool showStartupCB,
	const bool showAtStartup
	)
	:
	JXModalDialogDirector()
{
	itsTipList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	itsTipIndex = 1;

	BuildWindow(showStartupCB, showAtStartup);
	ParseTips();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTipOfTheDayDialog::~JXTipOfTheDayDialog()
{
	jdelete itsTipList;
}

/******************************************************************************
 ShowAtStartup

 ******************************************************************************/

bool
JXTipOfTheDayDialog::ShowAtStartup()
	const
{
	return itsShowAtStartupCB->IsChecked();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXTipOfTheDayDialog::BuildWindow
	(
	const bool showStartupCB,
	const bool showAtStartup
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 410,260, JGetString("WindowTitle::JXTipOfTheDayDialog::JXLayout"));

	auto* sideBar =
		jnew JXFlatRect(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 10,10, 50,200);
	sideBar->SetBackColor(JColorManager::GetColorID(JRGB(39321, 39321, 39321)));

	auto* title =
		jnew JXStaticText(JGetString("title::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 340,50);
	title->SetToLabel(false);

	auto* icon =
		jnew JXImageWidget(sideBar,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,15, 30,30);
#ifndef _H_jx_af_image_jx_jx_tip_of_the_day
#define _H_jx_af_image_jx_jx_tip_of_the_day
#include <jx-af/image/jx/jx_tip_of_the_day.xpm>
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_tip_of_the_day), false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 340,150);
	assert( scrollbarSet != nullptr );

	itsText =
		jnew JXStaticText(JString::empty, true, false, true, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 340,150);
	itsText->SetBorderWidth(0);

	itsShowAtStartupCB =
		jnew JXTextCheckbox(JGetString("itsShowAtStartupCB::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,225, 140,20);
	itsShowAtStartupCB->SetShortcuts(JGetString("itsShowAtStartupCB::shortcuts::JXTipOfTheDayDialog::JXLayout"));

	itsNextTipButton =
		jnew JXTextButton(JGetString("itsNextTipButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,225, 60,20);
	itsNextTipButton->SetShortcuts(JGetString("itsNextTipButton::shortcuts::JXTipOfTheDayDialog::JXLayout"));

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 340,225, 60,20);

// end JXLayout

	SetButtons(itsCloseButton, nullptr);

	JXImage* wIcon;
	const JError err = JXImage::CreateFromXPM(GetDisplay(), JXPM(jx_tip_of_the_day), &wIcon);
	assert_ok( err );
	window->SetIcon(wIcon);

	title->SetBorderWidth(kJXDefaultBorderWidth);
	title->TESetLeftMarginWidth(5);
	title->SetBackColor(title->GetFocusColor());
	title->GetText()->SetFont(title->GetText()->SelectAll(),
		JFontManager::GetFont("Times", 18, JFontStyle(true, false, 0, false)), true);

	itsText->TESetLeftMarginWidth(5);
	itsText->SetBorderWidth(kJXDefaultBorderWidth);

	itsShowAtStartupCB->SetVisible(showStartupCB);
	itsShowAtStartupCB->SetState(showAtStartup);

	ListenTo(itsNextTipButton, std::function([this](const JXButton::Pushed&)
	{
		itsTipIndex++;
		if (!itsTipList->IndexValid(itsTipIndex))
		{
			itsTipIndex = 1;
		}
		DisplayTip();
	}));

	ListenTo(itsCloseButton, std::function([this](const JXButton::Pushed&)
	{
		EndDialog(true);
	}));
}

/******************************************************************************
 ParseTips (private)

 ******************************************************************************/

static JKLRand theRandomGenerator;

void
JXTipOfTheDayDialog::ParseTips()
{
	const JString& data = JGetString("TIP_OF_THE_DAY");

	itsTipList->CleanOut();

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	data.Split(kDelimiter, &list);

	for (auto* s : list)
	{
		if (!s->IsEmpty())
		{
			s->Prepend("\n");
			AddTip(*s);
		}
	}

	assert( !itsTipList->IsEmpty() );

	itsTipIndex = theRandomGenerator.UniformLong(1, itsTipList->GetItemCount());
	DisplayTip();
}

/******************************************************************************
 AddTip (private)

 ******************************************************************************/

void
JXTipOfTheDayDialog::AddTip
	(
	JString tip
	)
{
	tip.TrimWhitespace();
	if (tip.IsEmpty())
	{
		return;
	}

	auto* s = jnew JString(tip);
	itsTipList->Append(s);
}

/******************************************************************************
 DisplayTip (private)

 ******************************************************************************/

void
JXTipOfTheDayDialog::DisplayTip()
{
	JReadLimitedMarkdown(*itsTipList->GetItem(itsTipIndex), itsText->GetText());
}
