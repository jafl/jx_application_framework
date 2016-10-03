/******************************************************************************
 SyGNewGitRemoteDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2014 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <SyGNewGitRemoteDialog.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGNewGitRemoteDialog::SyGNewGitRemoteDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGNewGitRemoteDialog::~SyGNewGitRemoteDialog()
{
}

/******************************************************************************
 GetRepoURL

 ******************************************************************************/

const JString&
SyGNewGitRemoteDialog::GetRepoURL()
	const
{
	return itsRemoteURLInputField->GetText();
}

/******************************************************************************
 GetLocalName

 ******************************************************************************/

const JString&
SyGNewGitRemoteDialog::GetLocalName()
	const
{
	return itsLocalNameInputField->GetText();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SyGNewGitRemoteDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,160, "");
	assert( window != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SyGNewGitRemoteDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,130, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SyGNewGitRemoteDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SyGNewGitRemoteDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,130, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SyGNewGitRemoteDialog::shortcuts::JXLayout"));

	itsRemoteURLInputField =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 270,20);
	assert( itsRemoteURLInputField != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SyGNewGitRemoteDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 270,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsLocalNameInputField =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 270,20);
	assert( itsLocalNameInputField != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SyGNewGitRemoteDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,70, 270,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SyGNewGitRemoteDialog"));
	SetButtons(okButton, cancelButton);

	itsRemoteURLInputField->SetIsRequired();
	itsLocalNameInputField->SetIsRequired();
}
