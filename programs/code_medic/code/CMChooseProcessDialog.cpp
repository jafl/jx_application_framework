/******************************************************************************
 CMChooseProcessDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "CMChooseProcessDialog.h"
#include "CMProcessText.h"
#include "GDBLink.h"
#include "cmGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXFontManager.h>

#include <JXColorManager.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

#ifdef _J_CYGWIN
static const JString kCmdStr("ps s", JString::kNoCopy);
#else
static const JString kCmdStr("ps xco pid,stat,command", JString::kNoCopy);
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

CMChooseProcessDialog::CMChooseProcessDialog
	(
	JXDirector*		supervisor,
	const bool	attachToSelection,
	const bool	stopProgram
	)
	:
	JXDialogDirector(supervisor, true),
	itsAttachToSelectionFlag(attachToSelection),
	itsStopProgramFlag(stopProgram)
{
	BuildWindow();

	int inFD;
	const JError err = JExecute(kCmdStr, nullptr,
								kJIgnoreConnection, nullptr,
								kJCreatePipe, &inFD);
	if (err.OK())
		{
		JString text;
		JReadAll(inFD, &text);
		text.TrimWhitespace();
		itsText->GetText()->SetText(text);
		}

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMChooseProcessDialog::~CMChooseProcessDialog()
{
}

/******************************************************************************
 SetProcessID

 ******************************************************************************/

void
CMChooseProcessDialog::SetProcessID
	(
	const JInteger value
	)
{
	itsProcessIDInput->SetValue(value);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CMChooseProcessDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,500, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 350,400);
	assert( scrollbarSet != nullptr );

	itsProcessIDInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,470, 80,20);
	assert( itsProcessIDInput != nullptr );

	auto* pidLabel =
		jnew JXStaticText(JGetString("pidLabel::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,470, 70,20);
	assert( pidLabel != nullptr );
	pidLabel->SetToLabel();

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 215,470, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,470, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CMChooseProcessDialog::shortcuts::JXLayout"));

	auto* hint =
		jnew JXStaticText(JGetString("hint::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 360,20);
	assert( hint != nullptr );
	hint->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CMChooseProcessDialog"));
	SetButtons(okButton, cancelButton);

	itsText =
		jnew CMProcessText(this, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsText != nullptr );
	itsText->FitToEnclosure();
	itsText->GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMChooseProcessDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			JInteger pid;
			const bool ok = itsProcessIDInput->GetValue(&pid);
			assert(ok);
			if (itsAttachToSelectionFlag)
				{
				CMGetLink()->AttachToProcess(pid);
				}
			else
				{
				dynamic_cast<GDBLink*>(CMGetLink())->ProgramStarted(pid);
				}

			if (itsStopProgramFlag)
				{
				CMGetLink()->StopProgram();
				}
			}
		}

	JXDialogDirector::Receive(sender, message);
}
