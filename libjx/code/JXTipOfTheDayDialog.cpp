/******************************************************************************
 JXTipOfTheDayDialog.cpp

	Parses and displays the contents of the TIP_OF_THE_DAY string resource.
	The tips are HTML, and they are separated by <div>.

	If desired, a "Show tips at startup" checkbox can be displayed.  This
	is off by default since it merely annoys most users.  If the checkbox
	is displayed, the dialog will broadcast the result when it is closed.

	BASE CLASS = JXDialogDirector

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
#include "JXImage.h"
#include "JXColorManager.h"
#include "JXFontManager.h"
#include "jXGlobals.h"
#include <jTextUtil.h>
#include <JKLRand.h>
#include <jAssert.h>

static const JUtf8Byte* kDelimiter = "=====";

// JBroadcaster message types

const JUtf8Byte* JXTipOfTheDayDialog::kShowAtStartup = "ShowAtStartup::JXTipOfTheDayDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTipOfTheDayDialog::JXTipOfTheDayDialog
	(
	const bool showStartupCB,
	const bool showAtStartup
	)
	:
	JXDialogDirector(JXGetPersistentWindowOwner(), false)
{
	itsTipList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsTipList != nullptr );

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
 BuildWindow (private)

 ******************************************************************************/

#include <jx_tip_of_the_day.xpm>

void
JXTipOfTheDayDialog::BuildWindow
	(
	const bool showStartupCB,
	const bool showAtStartup
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 410,260, JString::empty);
	assert( window != nullptr );

	auto* sideBar =
		jnew JXFlatRect(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 10,10, 50,200);
	assert( sideBar != nullptr );
	sideBar->SetColor(JColorManager::GetInactiveLabelColor());

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 340,225, 60,20);
	assert( itsCloseButton != nullptr );

	itsNextTipButton =
		jnew JXTextButton(JGetString("itsNextTipButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,225, 60,20);
	assert( itsNextTipButton != nullptr );
	itsNextTipButton->SetShortcuts(JGetString("itsNextTipButton::JXTipOfTheDayDialog::shortcuts::JXLayout"));

	auto* icon =
		jnew JXImageWidget(sideBar,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,15, 30,30);
	assert( icon != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 340,150);
	assert( scrollbarSet != nullptr );

	auto* title =
		jnew JXStaticText(JGetString("title::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 340,50);
	assert( title != nullptr );

	itsShowAtStartupCB =
		jnew JXTextCheckbox(JGetString("itsShowAtStartupCB::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,225, 140,20);
	assert( itsShowAtStartupCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXTipOfTheDayDialog"));
	window->LockCurrentMinSize();
	window->PlaceAsDialogWindow();

	JXImage* wIcon;
	const JError err = JXImage::CreateFromXPM(GetDisplay(), JXPM(jx_tip_of_the_day), &wIcon);
	assert_ok( err );
	window->SetIcon(wIcon);

	sideBar->SetColor(JColorManager::GetGrayColor(50));
	icon->SetXPM(jx_tip_of_the_day, JColorManager::GetGrayColor(50));

	title->SetToLabel();
	title->SetBorderWidth(kJXDefaultBorderWidth);
	title->TESetLeftMarginWidth(5);
	title->SetBackColor(title->GetFocusColor());
	title->GetText()->SetFont(title->GetText()->SelectAll(),
		JFontManager::GetFont(
			JGetString("FontName::JXTipOfTheDayDialog"), 18,
			JFontStyle(true, false, 0, false)),
		true);

	itsText =
		jnew JXStaticText(JString::empty, true, false, true,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsText != nullptr );
	itsText->FitToEnclosure();
	itsText->TESetLeftMarginWidth(5);

	itsShowAtStartupCB->SetVisible(showStartupCB);
	itsShowAtStartupCB->SetState(showAtStartup);

	ListenTo(itsNextTipButton);
	ListenTo(itsCloseButton);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXTipOfTheDayDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNextTipButton && message.Is(JXButton::kPushed))
		{
		itsTipIndex++;
		if (!itsTipList->IndexValid(itsTipIndex))
			{
			itsTipIndex = 1;
			}
		DisplayTip();
		}

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		if (itsShowAtStartupCB->IsVisible())
			{
			Broadcast(ShowAtStartup(itsShowAtStartupCB->IsChecked()));
			}
		Close();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
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

	for (JString* s : list)
		{
		if (!s->IsEmpty())
			{
			s->Prepend("\n");
			AddTip(*s);
			}
		}

	assert( !itsTipList->IsEmpty() );

	itsTipIndex = theRandomGenerator.UniformLong(1, itsTipList->GetElementCount());
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
	assert( s != nullptr );
	itsTipList->Append(s);
}

/******************************************************************************
 DisplayTip (private)

 ******************************************************************************/

void
JXTipOfTheDayDialog::DisplayTip()
{
	JReadLimitedMarkdown(*itsTipList->GetElement(itsTipIndex), itsText->GetText());
}
