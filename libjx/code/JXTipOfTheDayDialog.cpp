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
#include <JKLRand.h>
#include <sstream>
#include <jAssert.h>

static const JUtf8Byte* kDelimiter = "<div>";

// JBroadcaster message types

const JUtf8Byte* JXTipOfTheDayDialog::kShowAtStartup = "ShowAtStartup::JXTipOfTheDayDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTipOfTheDayDialog::JXTipOfTheDayDialog
	(
	const JBoolean showStartupCB,
	const JBoolean showAtStartup
	)
	:
	JXDialogDirector(JXGetPersistentWindowOwner(), kJFalse)
{
	itsTipList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsTipList != NULL );

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
	const JBoolean showStartupCB,
	const JBoolean showAtStartup
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 410,260, JString::empty);
	assert( window != NULL );

	JXFlatRect* sideBar =
		jnew JXFlatRect(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 10,10, 50,200);
	assert( sideBar != NULL );
	sideBar->SetColor(JColorManager::GetInactiveLabelColor());

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 340,225, 60,20);
	assert( itsCloseButton != NULL );

	itsNextTipButton =
		jnew JXTextButton(JGetString("itsNextTipButton::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,225, 60,20);
	assert( itsNextTipButton != NULL );
	itsNextTipButton->SetShortcuts(JGetString("itsNextTipButton::JXTipOfTheDayDialog::shortcuts::JXLayout"));

	JXImageWidget* icon =
		jnew JXImageWidget(sideBar,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,15, 30,30);
	assert( icon != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,60, 340,150);
	assert( scrollbarSet != NULL );

	JXStaticText* title =
		jnew JXStaticText(JGetString("title::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 340,50);
	assert( title != NULL );

	itsShowAtStartupCB =
		jnew JXTextCheckbox(JGetString("itsShowAtStartupCB::JXTipOfTheDayDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,225, 140,20);
	assert( itsShowAtStartupCB != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXTipOfTheDayDialog"));
	window->LockCurrentMinSize();
	window->PlaceAsDialogWindow();

	JXDisplay* display = GetDisplay();
	JXImage* wIcon     = jnew JXImage(display, jx_tip_of_the_day);
	assert( wIcon != NULL );
	window->SetIcon(wIcon);

	sideBar->SetColor(GetColormap()->GetGrayColor(50));
	icon->SetXPM(jx_tip_of_the_day, GetColormap()->GetGrayColor(50));

	title->SetBorderWidth(kJXDefaultBorderWidth);
	title->TESetLeftMarginWidth(5);
	title->SetBackColor(title->GetFocusColor());
	title->JTextEditor::SetFont(1, title->GetTextLength(),
		(window->GetFontManager())->GetFont(
			JGetString("FontName::JXTipOfTheDayDialog"), 18,
			JFontStyle(kJTrue, kJFalse, 0, kJFalse)),
		kJTrue);
	title->SetCaretLocation(1);
	title->Paste(JString("\n", 0, kJFalse));

	itsText =
		jnew JXStaticText(JString::empty, kJTrue, kJFalse,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsText != NULL );
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

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JString* s = list.GetElement(i);
		if (!s->IsEmpty())
			{
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

	JString* s = jnew JString(tip);
	assert( s != NULL );
	itsTipList->Append(s);
}

/******************************************************************************
 DisplayTip (private)

 ******************************************************************************/

void
JXTipOfTheDayDialog::DisplayTip()
{
	const JString* s = itsTipList->GetElement(itsTipIndex);
	const std::string s1(s->GetBytes(), s->GetByteCount());
	std::istringstream input(s1);
	itsText->ReadHTML(input);
	itsText->SetCaretLocation(1);
	itsText->Paste(JString("\n", 0, kJFalse));
}
