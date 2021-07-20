/******************************************************************************
 SyGFindFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1996-2009 by John Lindal.

 ******************************************************************************/

#include "SyGFindFileDialog.h"
#include "SyGFindFileTask.h"
#include "SyGViewManPageDialog.h"
#include "SyGPrefsMgr.h"
#include "SyGGlobals.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathInput.h>
#include <JXChooseSaveFile.h>
#include <JXFontManager.h>

#include <jProcessUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

enum
{
	kFindFileAction = 1,
	kFindExprAction
};

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFindFileDialog::SyGFindFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(SyGGetPrefsMgr(), kSFindFilePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFindFileDialog::~SyGFindFileDialog()
{
	// prefs written by SyGDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive() && itsFileInput->IsActive())
		{
		itsFileInput->Focus();
		itsFileInput->SelectAll();
		}
	else if (IsActive() && itsExprInput->IsActive())
		{
		itsExprInput->Focus();
		itsExprInput->SelectAll();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SyGFindFileDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 360,220, JString::empty);
	assert( window != nullptr );

	itsActionRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,50, 340,120);
	assert( itsActionRG != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,190, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::SyGFindFileDialog::shortcuts::JXLayout"));

	itsSearchButton =
		jnew JXTextButton(JGetString("itsSearchButton::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,190, 60,20);
	assert( itsSearchButton != nullptr );
	itsSearchButton->SetShortcuts(JGetString("itsSearchButton::SyGFindFileDialog::shortcuts::JXLayout"));

	itsFileInput =
		jnew JXInputField(itsActionRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 35,30, 295,20);
	assert( itsFileInput != nullptr );

	auto* fileLabel =
		jnew JXTextRadioButton(kFindFileAction, JGetString("fileLabel::SyGFindFileDialog::JXLayout"), itsActionRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 270,20);
	assert( fileLabel != nullptr );
	fileLabel->SetShortcuts(JGetString("fileLabel::SyGFindFileDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,190, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SyGFindFileDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,190, 90,20);
	assert( itsStayOpenCB != nullptr );

	itsExprInput =
		jnew JXInputField(itsActionRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 35,90, 295,20);
	assert( itsExprInput != nullptr );

	auto* findLabel =
		jnew JXTextRadioButton(kFindExprAction, JGetString("findLabel::SyGFindFileDialog::JXLayout"), itsActionRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,70, 270,20);
	assert( findLabel != nullptr );
	findLabel->SetShortcuts(JGetString("findLabel::SyGFindFileDialog::shortcuts::JXLayout"));

	auto* startLabel =
		jnew JXStaticText(JGetString("startLabel::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
	assert( startLabel != nullptr );
	startLabel->SetToLabel();

	itsPathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,20, 210,20);
	assert( itsPathInput != nullptr );

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::SyGFindFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,20, 60,20);
	assert( itsChoosePathButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SyGFindFileDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	itsActionRG->SetBorderWidth(0);

	ListenTo(itsChoosePathButton);
	ListenTo(itsActionRG);
	ListenTo(itsSearchButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);

	itsPathInput->ShouldAllowInvalidPath();

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsFileInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsFileInput->SetFont(font);
	ListenTo(itsFileInput);

	itsExprInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsExprInput->SetFont(font);
	ListenTo(itsExprInput);

	itsStayOpenCB->SetState(true);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SyGFindFileDialog::UpdateDisplay()
{
	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction)
		{
		itsExprInput->Deactivate();
		itsFileInput->Activate();
		itsFileInput->Focus();
		itsFileInput->SelectAll();
		}
	else if (action == kFindExprAction)
		{
		itsFileInput->Deactivate();
		itsExprInput->Activate();
		itsExprInput->Focus();
		itsExprInput->SelectAll();
		}

	UpdateButtons();
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
SyGFindFileDialog::UpdateButtons()
{
	JXInputField* field = nullptr;

	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction)
		{
		field = itsFileInput;
		}
	else if (action == kFindExprAction)
		{
		field = itsExprInput;
		}

	assert( field != nullptr );
	if (field->GetText()->IsEmpty())
		{
		itsSearchButton->Deactivate();
		}
	else
		{
		itsSearchButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGFindFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSearchButton && message.Is(JXButton::kPushed))
		{
		if (Search() && !itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		GetWindow()->KillFocus();
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(SyGGetManPageDialog())->ViewManPage(JString("find", JString::kNoCopy));
		}

	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		itsPathInput->ChoosePath(JString::empty);
		}

	else if (sender == itsActionRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		UpdateDisplay();
		}

	else if ((sender == itsFileInput || sender == itsExprInput) &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateButtons();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Search (private)

 ******************************************************************************/

bool
SyGFindFileDialog::Search()
{
	itsPathInput->ShouldAllowInvalidPath(false);
	if (!itsPathInput->InputValid())
		{
		itsPathInput->ShouldAllowInvalidPath(true);
		itsPathInput->Focus();
		return false;
		}
	itsPathInput->ShouldAllowInvalidPath(true);

	JString path;
	const bool ok = itsPathInput->GetPath(&path);
	assert( ok );

	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction && itsFileInput->InputValid())
		{
		SearchFileName(path, itsFileInput->GetText()->GetText());
		}
	else if (action == kFindExprAction && itsExprInput->InputValid())
		{
		SearchExpr(path, itsExprInput->GetText()->GetText());
		}

	return true;
}

/******************************************************************************
 Search

 ******************************************************************************/

void
SyGFindFileDialog::Search
	(
	const JString& path
	)
{
	itsPathInput->GetText()->SetText(path);
	Activate();
}

/******************************************************************************
 SearchFileName

 ******************************************************************************/

void
SyGFindFileDialog::SearchFileName
	(
	const JString& path,
	const JString& pattern
	)
{
	JString expr("-iname ");
	expr += JPrepArgForExec(pattern);

	SearchExpr(path, expr);
}

/******************************************************************************
 SearchExpr

 ******************************************************************************/

void
SyGFindFileDialog::SearchExpr
	(
	const JString& path,
	const JString& expr
	)
{
	JString e("( ");
	e += expr;
	e += " )";

	const JUtf8Byte** vcsDirName;
	const JSize vcsDirNameCount = JGetVCSDirectoryNames(&vcsDirName);
	for (JUnsignedOffset i=0; i<vcsDirNameCount; i++)
		{
		e += " -a ! -path */";
		e += vcsDirName[i];
		e += "/*";
		}

	SyGTreeDir* dir;
	if ((SyGGetApplication())->OpenDirectory(path, &dir, nullptr, true, true, false, true))
		{
		SyGFindFileTask* task;
		SyGFindFileTask::Create(dir, path, e, &task);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	bool stayOpen;
	input >> JBoolFromString(stayOpen);
	itsStayOpenCB->SetState(stayOpen);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << JBoolToString(itsStayOpenCB->IsChecked());
}
