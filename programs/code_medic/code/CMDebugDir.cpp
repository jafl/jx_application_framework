/******************************************************************************
 CMDebugDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "CMDebugDir.h"
#include "CMTextDisplayBase.h"
#include "cmGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColorManager.h>
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
	CMGetPrefsManager()->SaveWindowSize(kDebugWindSizeID, GetWindow());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CMDebugDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 450,480);
	assert( scrollbarSet != nullptr );

	itsCopyButton =
		jnew JXTextButton(JGetString("itsCopyButton::CMDebugDir::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,20);
	assert( itsCopyButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CMDebugDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->SetWMClass(CMGetWMClassInstance(), CMGetDebugWindowClass());
	CMGetPrefsManager()->GetWindowSize(kDebugWindSizeID, window);

	itsText =
		jnew JXStaticText(JString::empty, false, true, true,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsText != nullptr );
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

static const JString kLogPrefix("=== ", JString::kNoCopy);

void
CMDebugDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink)
		{
		itsText->SetCaretLocation(itsText->GetText()->GetText().GetCharacterCount() + 1);

		if (message.Is(CMLink::kDebugOutput))
			{
			const auto* msg =
				dynamic_cast<const CMLink::DebugOutput*>(&message);
			assert( msg != nullptr );

			const CMLink::DebugType type = msg->GetType();
			if (type == CMLink::kCommandType)
				{
				itsText->SetCurrentFontColor(JColorManager::GetLightBlueColor());
				itsText->Paste(itsLink->GetPrompt() + " ");
				itsFile << itsLink->GetPrompt() << " ";
				}
			else if (type == CMLink::kOutputType)
				{
				itsText->SetCurrentFontColor(JColorManager::GetBlueColor());
				}
			else if (type == CMLink::kLogType)
				{
				itsText->Paste(kLogPrefix);
				itsFile << kLogPrefix.GetBytes();
				}

			itsText->Paste(msg->GetText());
			itsText->SetCurrentFontColor(JColorManager::GetBlackColor());
			itsText->Paste(JString::newline);

			itsFile << msg->GetText();
			itsFile << std::endl;
			}
		else if (!message.Is(CMLink::kUserOutput))
			{
			itsText->Paste(kLogPrefix);
			itsText->Paste(JString(message.GetType(), JString::kNoCopy));
			itsText->Paste(JString::newline);

			itsFile << kLogPrefix;
			itsFile << message.GetType();
			itsFile << std::endl;
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

		itsText->GetText()->SetText(JString::empty);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}
