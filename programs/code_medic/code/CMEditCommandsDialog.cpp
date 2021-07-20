/******************************************************************************
 CMEditCommandsDialog.cpp

	BASE CLASS = public JXDialogDirector, JPrefObject

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <CMEditCommandsDialog.h>
#include <CMEditCommandsTable.h>
#include <CMPrefsManager.h>
#include <cmGlobals.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXWindow.h>
#include <JFontManager.h>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

CMEditCommandsDialog::CMEditCommandsDialog()
	:
	JXDialogDirector(JXGetApplication(), true),
	JPrefObject(CMGetPrefsManager(), kEditCommandsDialogID)
{
	itsCommands	= jnew JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsCommands != nullptr);

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

	auto* window = jnew JXWindow(this, 350,370, JString::empty);
	assert( window != nullptr );

	auto* gdbCmdTitle =
		jnew JXStaticText(JGetString("gdbCmdTitle::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 150,20);
	assert( gdbCmdTitle != nullptr );
	gdbCmdTitle->SetToLabel();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 230,280);
	assert( scrollbarSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,340, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 210,340, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,70, 70,20);
	assert( itsNewButton != nullptr );
	itsNewButton->SetShortcuts(JGetString("itsNewButton::CMEditCommandsDialog::shortcuts::JXLayout"));

	itsRemoveButton =
		jnew JXTextButton(JGetString("itsRemoveButton::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,110, 70,20);
	assert( itsRemoveButton != nullptr );

	auto* hint =
		jnew JXStaticText(JGetString("hint::CMEditCommandsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( hint != nullptr );
	hint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	hint->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CMEditCommandsDialog"));
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	UseModalPlacement(false);
	SetButtons(okButton, cancelButton);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	itsWidget =
		jnew CMEditCommandsTable(this, itsRemoveButton, scrollbarSet,
								scrollbarSet->GetScrollEnclosure(),
								JXWidget::kHElastic, JXWidget::kVElastic,
								0,0, 10,10);
	assert(itsWidget != nullptr);
	itsWidget->SyncWithData();
	itsWidget->FitToEnclosure(true, true);	// requires sync
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
		itsCommands->Append(JString::empty);
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

bool
CMEditCommandsDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return true;
		}

	if (!itsWidget->EndEditing())
		{
		return false;
		}

	CMGetPrefsManager()->SetCmdList(*itsCommands);
	return true;
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
