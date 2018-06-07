/******************************************************************************
 CBSubprojectConfigDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBSubprojectConfigDialog.h"
#include "CBProjectFileInput.h"
#include "CBRelPathCSF.h"
#include "CBProjectDocument.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSubprojectConfigDialog::CBSubprojectConfigDialog
	(
	CBProjectDocument*	supervisor,
	const JBoolean		includeInDepList,
	const JCharacter*	subProjName,
	const JBoolean		shouldBuild,
	CBRelPathCSF*		csf
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsCSF = csf;

	BuildWindow(supervisor, includeInDepList, subProjName, shouldBuild,
				itsCSF->GetProjectPath());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSubprojectConfigDialog::~CBSubprojectConfigDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBSubprojectConfigDialog::BuildWindow
	(
	CBProjectDocument*	supervisor,
	const JBoolean		includeInDepList,
	const JCharacter*	subProjName,
	const JBoolean		shouldBuild,
	const JCharacter*	basePath
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 360,160, "");
	assert( window != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,130, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBSubprojectConfigDialog::shortcuts::JXLayout"));

	JXStaticText* fileLabel =
		jnew JXStaticText(JGetString("fileLabel::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 70,20);
	assert( fileLabel != nullptr );
	fileLabel->SetToLabel();

	itsShouldBuildCB =
		jnew JXTextCheckbox(JGetString("itsShouldBuildCB::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 230,20);
	assert( itsShouldBuildCB != nullptr );
	itsShouldBuildCB->SetShortcuts(JGetString("itsShouldBuildCB::CBSubprojectConfigDialog::shortcuts::JXLayout"));

	itsSubProjName =
		jnew CBProjectFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 90,60, 200,20);
	assert( itsSubProjName != nullptr );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,60, 50,20);
	assert( itsChooseFileButton != nullptr );
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::CBSubprojectConfigDialog::shortcuts::JXLayout"));

	itsIncludeInDepListCB =
		jnew JXTextCheckbox(JGetString("itsIncludeInDepListCB::CBSubprojectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 250,20);
	assert( itsIncludeInDepListCB != nullptr );
	itsIncludeInDepListCB->SetShortcuts(JGetString("itsIncludeInDepListCB::CBSubprojectConfigDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Sub-project Configuration");
	SetButtons(okButton, cancelButton);

	itsSubProjName->SetText(subProjName);
	itsSubProjName->SetBasePath(basePath);
	itsSubProjName->ShouldAllowInvalidFile();

	if ((supervisor->GetBuildManager())->GetMakefileMethod() == CBBuildManager::kMakemake)
		{
		itsIncludeInDepListCB->SetState(includeInDepList);
		itsShouldBuildCB->SetState(shouldBuild);

		UpdateDisplay();
		}
	else
		{
		itsIncludeInDepListCB->SetState(kJFalse);
		itsShouldBuildCB->SetState(kJFalse);

		UpdateDisplay();	// before other deactivations

		itsIncludeInDepListCB->Deactivate();
		itsShouldBuildCB->Deactivate();
		}

	ListenTo(itsChooseFileButton);
	ListenTo(itsIncludeInDepListCB);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBSubprojectConfigDialog::UpdateDisplay()
{
	itsShouldBuildCB->SetActive(itsIncludeInDepListCB->IsChecked());
	if (!itsIncludeInDepListCB->IsChecked())
		{
		itsShouldBuildCB->SetState(kJFalse);
		}
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
CBSubprojectConfigDialog::GetConfig
	(
	JBoolean*	includeInDepList,
	JString*	subProjName,
	JBoolean*	shouldBuild
	)
	const
{
	*includeInDepList = itsIncludeInDepListCB->IsChecked();

	*subProjName = itsSubProjName->GetText();
	*shouldBuild = itsShouldBuildCB->IsChecked();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSubprojectConfigDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
		{
		ChooseProjectFile();
		}
	else if (sender == itsIncludeInDepListCB && message.Is(JXCheckbox::kPushed))
		{
		UpdateDisplay();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseProjectFile (private)

 ******************************************************************************/

void
CBSubprojectConfigDialog::ChooseProjectFile()
{
	const JString origName = itsSubProjName->GetTextForChooseFile();
	JString newName;
	if (itsCSF->ChooseRelFile("", "Select the project file that builds the library.",
							  origName, &newName))
		{
		JString fullName;
		if (JConvertToAbsolutePath(newName, itsCSF->GetProjectPath(), &fullName) &&
			CBProjectDocument::CanReadFile(fullName) == JXFileDocument::kFileReadable)
			{
			itsSubProjName->SetText(newName);
			}
		else
			{
			(JGetUserNotification())->ReportError("That is not a project file.");
			}
		}
}
