/******************************************************************************
 CMDebugDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMDebugDir.h"
#include "CMTextDisplayBase.h"
#include "cmGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDebugDir::CMDebugDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsFile("/tmp/code_medic_log")
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDebugDir::~CMDebugDir()
{
	(CMGetPrefsManager())->SaveWindowSize(kDebugWindSizeID, GetWindow());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CMDebugDir::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 450,500, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 450,480);
	assert( scrollbarSet != NULL );

	itsCopyButton =
		new JXTextButton(JGetString("itsCopyButton::CMDebugDir::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,20);
	assert( itsCopyButton != NULL );

// end JXLayout

	window->SetTitle("Debug");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->SetWMClass(CMGetWMClassInstance(), CMGetDebugWindowClass());
	(CMGetPrefsManager())->GetWindowSize(kDebugWindSizeID, window);

	itsText =
		new JXStaticText("", kJFalse, kJTrue,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsText != NULL );
	itsText->FitToEnclosure();
	CMTextDisplayBase::AdjustFont(itsText);

	ListenTo(itsCopyButton);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMDebugDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMDebugDir");
}

/******************************************************************************
 Receive

 ******************************************************************************/

static const JCharacter* kLogPrefix = "=== ";

void
CMDebugDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink)
		{
		itsText->SetCaretLocation(itsText->GetTextLength() + 1);

		if (message.Is(CMLink::kDebugOutput))
			{
			const CMLink::DebugOutput* msg =
				dynamic_cast<const CMLink::DebugOutput*>(&message);
			assert( msg != NULL );

			const CMLink::DebugType type = msg->GetType();
			if (type == CMLink::kCommandType)
				{
				itsText->SetCurrentFontColor((GetColormap())->GetLightBlueColor());
				itsText->Paste(itsLink->GetPrompt() + " ");
				itsFile << itsLink->GetPrompt() << " ";
				}
			else if (type == CMLink::kOutputType)
				{
				itsText->SetCurrentFontColor((GetColormap())->GetBlueColor());
				}
			else if (type == CMLink::kLogType)
				{
				itsText->Paste(kLogPrefix);
				itsFile << kLogPrefix;
				}

			itsText->Paste(msg->GetText());
			itsText->SetCurrentFontColor((GetColormap())->GetBlackColor());
			itsText->Paste("\n");

			itsFile << msg->GetText();
			itsFile << endl;
			}
		else if (JStringCompare(message.GetType(), CMLink::kUserOutput, kJTrue) != 0)
			{
			itsText->Paste(kLogPrefix);
			itsText->Paste(message.GetType());
			itsText->Paste("\n");

			itsFile << kLogPrefix;
			itsFile << message.GetType();
			itsFile << endl;
			}
		}

	else if (sender == itsCopyButton && message.Is(JXButton::kPushed))
		{
		itsText->SelectAll();
		itsText->Copy();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMDebugDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		itsText->SetText("");
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}
