/******************************************************************************
 ImportDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImportDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXFileInput.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

static JString theActionDefsFile;	// saved to help import multiple menus

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

ImportDialog::ImportDialog()
	:
	JXModalDialogDirector()
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImportDialog::~ImportDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ImportDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);

	itsMenuText =
		jnew JXInputField(false, true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 20,30, 220,180);
	assert( itsMenuText != nullptr );
	itsMenuText->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsMenuEnum =
		jnew JXInputField(false, true, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,30, 220,180);
	assert( itsMenuEnum != nullptr );
	itsMenuEnum->SetFont(JFontManager::GetDefaultMonospaceFont());

	auto* actionDefsLabel =
		jnew JXStaticText(JGetString("actionDefsLabel::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 90,20);
	assert( actionDefsLabel != nullptr );
	actionDefsLabel->SetToLabel();

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 350,270, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ImportDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 215,270, 60,20);
	assert( itsHelpButton != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,270, 60,20);
	assert( cancelButton != nullptr );

	itsActionDefsFileInput =
		jnew JXFileInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,230, 310,20);
	assert( itsActionDefsFileInput != nullptr );

	auto* textLabel =
		jnew JXStaticText(JGetString("textLabel::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 120,20);
	assert( textLabel != nullptr );
	textLabel->SetToLabel();

	auto* enumLabel =
		jnew JXStaticText(JGetString("enumLabel::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 260,10, 110,20);
	assert( enumLabel != nullptr );
	enumLabel->SetToLabel();

	itsChooseActionDefsFileButton =
		jnew JXTextButton(JGetString("itsChooseActionDefsFileButton::ImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 420,230, 60,20);
	assert( itsChooseActionDefsFileButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::AboutDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton, std::function([](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowSection("ImportHelp");
	}));

	itsMenuText->SetIsRequired();

	itsActionDefsFileInput->SetIsRequired(false);
	itsActionDefsFileInput->GetText()->SetText(theActionDefsFile);
	itsActionDefsFileInput->ShouldRequireWritable(false);

	ListenTo(itsChooseActionDefsFileButton, std::function([this](const JXButton::Pushed&)
	{
		itsActionDefsFileInput->ChooseFile();
	}));
}

/******************************************************************************
 Getters

 ******************************************************************************/

const JString&
ImportDialog::GetMenuText()
	const
{
	return itsMenuText->GetText()->GetText();
}

const JString&
ImportDialog::GetEnumText()
	const
{
	return itsMenuEnum->GetText()->GetText();
}

bool
ImportDialog::GetActionDefsFile
	(
	JString* fullName
	)
	const
{
	if (itsActionDefsFileInput->GetFile(fullName))
	{
		theActionDefsFile = *fullName;
		return true;
	}
	else
	{
		theActionDefsFile.Clear();
		return false;
	}
}
