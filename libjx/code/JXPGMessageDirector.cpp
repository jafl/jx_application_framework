/******************************************************************************
 JXPGMessageDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#include "JXPGMessageDirector.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXScrollbarSet.h"
#include "JXFontManager.h"
#include "JXPTPrinter.h"
#include "JXSaveFileDialog.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPGMessageDirector::JXPGMessageDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsPrinter = nullptr;
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPGMessageDirector::~JXPGMessageDirector()
{
	jdelete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPGMessageDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,210, JGetString("WindowTitle::JXPGMessageDirector::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,170);
	assert( scrollbarSet != nullptr );

	itsPrintButton =
		jnew JXTextButton(JGetString("itsPrintButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,180, 60,20);

	itsSaveButton =
		jnew JXTextButton(JGetString("itsSaveButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,180, 60,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,180, 60,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::JXPGMessageDirector::JXLayout"));

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->HideFromTaskbar();

	itsPrintButton->Hide();
	itsSaveButton->Hide();
	itsCloseButton->Hide();

	itsMessageText =
		jnew JXStaticText(JString::empty,false,true,false, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 10,10);
	itsMessageText->FitToEnclosure();

	itsMessageText->GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
}

/******************************************************************************
 AddMessageLine

	Appends a newline and then the text.

 ******************************************************************************/

void
JXPGMessageDirector::AddMessageLine
	(
	const JString& text
	)
{
	itsMessageText->GoToEndOfLine();

	if (!itsMessageText->GetText()->IsEmpty())
	{
		itsMessageText->Paste(JString::newline);
	}

	itsMessageText->Paste(text);
	GetWindow()->Update();
}

/******************************************************************************
 AddMessageString

	Appends the text.

 ******************************************************************************/

void
JXPGMessageDirector::AddMessageString
	(
	const JString& text
	)
{
	itsMessageText->GoToEndOfLine();
	itsMessageText->Paste(text);
	GetWindow()->Update();
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
JXPGMessageDirector::ProcessFinished()
{
	GetWindow()->SetCloseAction(JXWindow::kCloseDirector);
	GetWindow()->Show();

	itsPrintButton->Show();
	itsSaveButton->Show();
	itsCloseButton->Show();

	ListenTo(itsPrintButton);
	ListenTo(itsSaveButton);
	ListenTo(itsCloseButton);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXPGMessageDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrintButton && message.Is(JXButton::kPushed))
	{
		if (itsPrinter == nullptr)
		{
			itsPrinter = jnew JXPTPrinter;
			itsMessageText->SetPTPrinter(itsPrinter);
		}
		itsMessageText->PrintPT();
	}
	else if (sender == itsSaveButton && message.Is(JXButton::kPushed))
	{
		SaveMessages();
	}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Close();
	}
	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 SaveMessages (private)

 ******************************************************************************/

void
JXPGMessageDirector::SaveMessages()
{
	auto* dlog = JXSaveFileDialog::Create(JGetString("SavePrompt::JXPGMessageDirector"),
										  JGetString("DefaultName::JXPGMessageDirector"));

	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetFullName();

		std::ofstream output(fullName.GetBytes());
		itsMessageText->GetText()->GetText().Print(output);
		output << std::endl;		// cosmetics
	}
}
