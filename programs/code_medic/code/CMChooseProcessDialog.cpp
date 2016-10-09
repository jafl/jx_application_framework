/******************************************************************************
 CMChooseProcessDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

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

#include <JColormap.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

#ifdef _J_CYGWIN
static const JCharacter* kCmdStr = "ps s";
#else
static const JCharacter* kCmdStr = "ps xco pid,stat,command";
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

CMChooseProcessDialog::CMChooseProcessDialog
	(
	JXDirector*		supervisor,
	const JBoolean	attachToSelection,
	const JBoolean	stopProgram
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsAttachToSelectionFlag(attachToSelection),
	itsStopProgramFlag(stopProgram)
{
	BuildWindow();

	int inFD;
	const JError err = JExecute(kCmdStr, NULL,
								kJIgnoreConnection, NULL,
								kJCreatePipe, &inFD);
	if (err.OK())
		{
		JString text;
		JReadAll(inFD, &text);
		text.TrimWhitespace();
		itsText->SetText(text);
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

	JXWindow* window = jnew JXWindow(this, 390,500, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 350,400);
	assert( scrollbarSet != NULL );

	itsProcessIDInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,470, 80,20);
	assert( itsProcessIDInput != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,470, 70,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 215,470, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,470, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CMChooseProcessDialog::shortcuts::JXLayout"));

	JXStaticText* obj2_JXLayout =
		jnew JXStaticText(JGetString("obj2_JXLayout::CMChooseProcessDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 360,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Choose process");
	SetButtons(okButton, cancelButton);

	itsText =
		jnew CMProcessText(this, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsText != NULL );
	itsText->FitToEnclosure();
	itsText->SetDefaultFont(window->GetFontManager()->GetDefaultMonospaceFont());
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
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			JInteger pid;
			const JBoolean ok = itsProcessIDInput->GetValue(&pid);
			assert(ok);
			if (itsAttachToSelectionFlag)
				{
				(CMGetLink())->AttachToProcess(pid);
				}
			else
				{
				dynamic_cast<GDBLink*>(CMGetLink())->ProgramStarted(pid);
				}

			if (itsStopProgramFlag)
				{
				(CMGetLink())->StopProgram();
				}
			}
		}

	JXDialogDirector::Receive(sender, message);
}
