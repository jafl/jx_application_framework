/******************************************************************************
 JXPGMessageDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXPGMessageDirector.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXFontManager.h>
#include <JXPTPrinter.h>
#include <jXGlobals.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

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
	itsPrinter = NULL;
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

	JXWindow* window = jnew JXWindow(this, 500,210, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,170);
	assert( scrollbarSet != NULL );

	itsSaveButton =
		jnew JXTextButton(JGetString("itsSaveButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,180, 60,20);
	assert( itsSaveButton != NULL );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,180, 60,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXPGMessageDirector::shortcuts::JXLayout"));

	itsPrintButton =
		jnew JXTextButton(JGetString("itsPrintButton::JXPGMessageDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,180, 60,20);
	assert( itsPrintButton != NULL );

// end JXLayout

	window->SetTitle("Message Window");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->HideFromTaskbar();

	itsPrintButton->Hide();
	itsSaveButton->Hide();
	itsCloseButton->Hide();

	itsMessageText =
		jnew JXStaticText("",kJFalse,kJTrue, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 10,10);
	itsMessageText->FitToEnclosure();

	itsMessageText->SetDefaultFont(window->GetFontManager()->GetDefaultMonospaceFont());
}

/******************************************************************************
 AddMessageLine

	Appends a newline and then the text.

 ******************************************************************************/

void
JXPGMessageDirector::AddMessageLine
	(
	const JCharacter* text
	)
{
	itsMessageText->SetCaretLocation(itsMessageText->GetTextLength() + 1);

	if (!itsMessageText->IsEmpty())
		{
		itsMessageText->Paste("\n");
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
	const JCharacter* text
	)
{
	itsMessageText->SetCaretLocation(itsMessageText->GetTextLength() + 1);
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
		if (itsPrinter == NULL)
			{
			itsPrinter = jnew JXPTPrinter;
			assert( itsPrinter != NULL );
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
	JString fileName;
	if ((JGetChooseSaveFile())->SaveFile("Save messages in:", NULL, "messages", &fileName))
		{
		std::ofstream output(fileName);
		const JString& text = itsMessageText->GetText();
		text.Print(output);
		output << std::endl;		// cosmetics
		}
}
