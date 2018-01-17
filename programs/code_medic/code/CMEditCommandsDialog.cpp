/******************************************************************************
 CMEditCommandsDialog.cpp

	BASE CLASS = public JXDialogDirector, JPrefObject

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <CMEditCommandsDialog.h>
#include <CMEditCommandsTable.h>
#include <CMPrefsManager.h>
#include <cmGlobals.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXWindow.h>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

CMEditCommandsDialog::CMEditCommandsDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue),
	JPrefObject(CMGetPrefsManager(), kEditCommandsDialogID)
{
	itsCommands	= jnew JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsCommands != NULL);

	CMGetPrefsManager()->GetCmdList(itsCommands);

	BuildWindow();

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMEditCommandsDialog::~CMEditCommandsDialog()
{
	JPrefObject::WritePrefs();

	itsCommands->DeleteAll();
	jdelete itsCommands;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CMEditCommandsDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 350,370, "");
	assert( window != NULL );

	JXStaticText* gdbCmdTitle =
		jnew JXStaticText(JGetString("gdbCmdTitle::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 150,20);
	assert( gdbCmdTitle != NULL );
	gdbCmdTitle->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 230,280);
	assert( scrollbarSet != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,340, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 210,340, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,70, 70,20);
	assert( itsNewButton != NULL );
	itsNewButton->SetShortcuts(JGetString("itsNewButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsRemoveButton =
		jnew JXTextButton(JGetString("itsRemoveButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,110, 70,20);
	assert( itsRemoveButton != NULL );

	JXStaticText* hint =
		jnew JXStaticText(JGetString("hint::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( hint != NULL );
	hint->SetFontSize(JGetDefaultFontSize()-2);
	hint->SetToLabel();

// end JXLayout

	window->SetTitle("Edit custom gdb commands");
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);
	SetButtons(okButton, cancelButton);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	itsWidget =
		jnew CMEditCommandsTable(this, itsRemoveButton, scrollbarSet,
								scrollbarSet->GetScrollEnclosure(),
								JXWidget::kHElastic, JXWidget::kVElastic,
								0,0, 10,10);
	assert(itsWidget != NULL);
	itsWidget->SyncWithData();
	itsWidget->FitToEnclosure(kJTrue, kJTrue);	// requires sync
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMEditCommandsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		itsCommands->Append("");
		itsWidget->NewStringAppended();
		}
	if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		itsWidget->RemoveCurrent();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 AppendString

 ******************************************************************************/

void
CMEditCommandsDialog::AppendString
	(
	const JString& str
	)
{
	itsCommands->Append(str);
}

/******************************************************************************
 SetString

 ******************************************************************************/

void
CMEditCommandsDialog::SetString
	(
	const JIndex	index,
	const JString&	str
	)
{
	*(itsCommands->GetElement(index)) = str;
}

/******************************************************************************
 DeleteString

 ******************************************************************************/

void
CMEditCommandsDialog::DeleteString
	(
	const JIndex index
	)
{
	itsCommands->DeleteElement(index);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
CMEditCommandsDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}

	if (!itsWidget->EndEditing())
		{
		return kJFalse;
		}

	CMGetPrefsManager()->SetCmdList(*itsCommands);
	return kJTrue;
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CMEditCommandsDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		JXWindow* window = GetWindow();
		window->ReadGeometry(input);
		window->Deiconify();
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CMEditCommandsDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ';
	GetWindow()->WriteGeometry(output);
}
