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
	itsCommands	= new JPtrArray<JString>(JPtrArrayT::kForgetAll);
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
	delete itsCommands;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CMEditCommandsDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 350,370, "");
	assert( window != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 150,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 230,280);
	assert( scrollbarSet != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 80,340, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,340, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsNewButton =
		new JXTextButton(JGetString("itsNewButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 260,70, 70,20);
	assert( itsNewButton != NULL );
	itsNewButton->SetShortcuts(JGetString("itsNewButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsRemoveButton =
		new JXTextButton(JGetString("itsRemoveButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 260,110, 70,20);
	assert( itsRemoveButton != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetFontSize(8);
	obj2_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Edit custom gdb commands");
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);
	SetButtons(okButton, cancelButton);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	itsWidget =
		new CMEditCommandsTable(this, itsRemoveButton, scrollbarSet,
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
	*(itsCommands->NthElement(index)) = str;
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
	istream& input
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
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ';
	GetWindow()->WriteGeometry(output);
}
