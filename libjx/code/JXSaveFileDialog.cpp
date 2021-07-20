/******************************************************************************
 JXSaveFileDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXSaveFileDialog.h"
#include "JXDirTable.h"
#include "JXCurrentPathMenu.h"
#include <JDirInfo.h>
#include "JXNewDirButton.h"
#include "JXDirectSaveSource.h"

#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXSaveFileInput.h"
#include "JXPathInput.h"
#include "JXPathHistoryMenu.h"
#include "JXScrollbarSet.h"
#include "jXGlobals.h"

#include <JString.h>
#include <JTableSelection.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXSaveFileDialog*
JXSaveFileDialog::Create
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JString&	origName,
	const JString&	prompt,
	const JString&	message
	)
{
	auto* dlog =
		jnew JXSaveFileDialog(supervisor, dirInfo, fileFilter);
	assert( dlog != nullptr );
	dlog->BuildWindow(origName, prompt, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXSaveFileDialog::JXSaveFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter
	)
	:
	JXCSFDialogBase(supervisor, dirInfo, fileFilter)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSaveFileDialog::~JXSaveFileDialog()
{
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

bool
JXSaveFileDialog::GetFileName
	(
	JString* name
	)
	const
{
	*name = itsFileName;
	return !name->IsEmpty();
}

/******************************************************************************
 GetFileNameInput

 ******************************************************************************/

JXInputField*
JXSaveFileDialog::GetFileNameInput()
{
	return itsFileNameInput;
}

/******************************************************************************
 Activate

	We want the file name input field to have focus.

 ******************************************************************************/

void
JXSaveFileDialog::Activate()
{
	JXCSFDialogBase::Activate();
	itsFileNameInput->Focus();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSaveFileDialog::BuildWindow
	(
	const JString&	origName,
	const JString&	prompt,
	const JString&	message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,340, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::JXSaveFileDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != nullptr );
	promptLabel->SetToLabel();

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput, origName,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton, homeButton,
			   desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, message);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXSaveFileDialog::SetObjects
	(
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			promptLabel,
	const JString&			prompt,
	JXSaveFileInput*		fileNameInput,
	const JString&			origName,
	JXStaticText*			pathLabel,
	JXPathInput*			pathInput,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXInputField*			filterInput,
	JXStringHistoryMenu*	filterHistory,
	JXTextButton*			saveButton,
	JXTextButton*			cancelButton,
	JXTextButton*			upButton,
	JXTextButton*			homeButton,
	JXTextButton*			desktopButton,
	JXNewDirButton*			newDirButton,
	JXTextCheckbox*			showHiddenCB,
	JXCurrentPathMenu*		currPathMenu,
	const JString&			message
	)
{
	itsSaveButton    = saveButton;
	itsFileNameInput = fileNameInput;

	JXWindow* window = scrollbarSet->GetWindow();
	window->SetTitle(JGetString("Title::JXSaveFileDialog"));

	SetButtons(itsSaveButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		itsSaveButton, upButton, homeButton, desktopButton,
		newDirButton, showHiddenCB, currPathMenu, message);

	JXDirTable* table = GetFileBrowser();
	table->AllowSelectFiles(false, false);
	table->AllowDblClickInactive(true);
	promptLabel->GetText()->SetText(prompt);
	itsFileNameInput->GetText()->SetText(origName);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->ShouldSelectWhenChangePath(false);

	const JRect frame   = itsFileNameInput->GetFrame();
	const JCoordinate w = frame.height();
	itsXDSSource =
		jnew JXDirectSaveSource(this, itsFileNameInput, window,
							   JXWidget::kFixedLeft, JXWidget::kFixedBottom,
							   frame.left, frame.top, w, frame.height());
	assert( itsXDSSource != nullptr );
	itsFileNameInput->Move(w, 0);
	itsFileNameInput->AdjustSize(-w, 0);

	UpdateDisplay();

	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));
	ListenTo(itsFileNameInput);

	cancelButton->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));

	// layout hack for FTC

	const JRect r1 = itsFileNameInput->GetFrameGlobal(),
				r2 = cancelButton->GetFrameGlobal();
	if (r1.top == r2.top)
		{
		cancelButton->Move(0, -1);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXSaveFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXDirTable* fileBrowser = GetFileBrowser();

	if (sender == itsFileNameInput && message.Is(JXWidget::kGotFocus))
		{
		itsSaveButton->SetLabel(JGetString("SaveLabel::JXSaveFileDialog"));
		UpdateDisplay();
		}
	else if (sender == itsFileNameInput && message.Is(JXWidget::kLostFocus))
		{
		itsSaveButton->SetLabel(JGetString("OpenLabel::JXSaveFileDialog"));
		UpdateDisplay();
		}

	else if (sender == itsFileNameInput &&
			 (message.Is(JStyledText::kTextChanged) ||
			  message.Is(JStyledText::kTextSet)))
		{
		UpdateDisplay();
		}

	else if (sender == fileBrowser && message.Is(JXDirTable::kFileDblClicked))
		{
		const auto* info =
			dynamic_cast<const JXDirTable::FileDblClicked*>(&message);
		assert( info != nullptr );
		const JString fileName = (info->GetDirEntry()).GetName();
		itsFileNameInput->GetText()->SetText(fileName);
		itsFileNameInput->Focus();
		}

	else
		{
		if (sender == &(fileBrowser->GetTableSelection()))
			{
			UpdateDisplay();
			}
		else if (sender == fileBrowser &&
				 (message.Is(JXWidget::kGotFocus) ||
				  message.Is(JXWidget::kLostFocus)))
			{
			UpdateDisplay();
			}

		JXCSFDialogBase::Receive(sender, message);
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXSaveFileDialog::OKToDeactivate()
{
	if (!JXCSFDialogBase::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}

	JXPathInput* pathInput = GetPathInput();
	if (pathInput->HasFocus())
		{
		GoToItsPath();
		return false;
		}

	JXInputField* filterInput = GetFilterInput();
	if (filterInput->HasFocus())
		{
		AdjustFilter();
		return false;
		}

	JXDirTable* fileBrowser = GetFileBrowser();
	if (fileBrowser->HasFocus() && fileBrowser->GoToSelectedDirectory())
		{
		return false;
		}

	const JString& fileName = itsFileNameInput->GetText()->GetText();
	if (fileName.IsEmpty())
		{
		JGetUserNotification()->ReportError(JGetString("MustEnterFileName::JXSaveFileDialog"));
		return false;
		}

	const JString& path     = GetPath();
	const JString fullName  = path + fileName;

	const bool fileExists = JFileExists(fullName);

	if (JDirectoryExists(fullName))
		{
		JGetUserNotification()->ReportError(JGetString("DirExists::JXSaveFileDialog"));
		return false;
		}
	else if (!JDirectoryWritable(path) && !fileExists)
		{
		JGetUserNotification()->ReportError(JGetString("DirNotWritable::JXGlobal"));
		return false;
		}
	else if (!fileExists)
		{
		itsFileName = fileName;
		return true;
		}
	else if (!JFileWritable(fullName))
		{
		JGetUserNotification()->ReportError(JGetString("FileNotWritable::JXGlobal"));
		return false;
		}
	else if (JGetUserNotification()->AskUserNo(JGetString("WarnReplaceFile::JXSaveFileDialog")))
		{
		itsFileName = fileName;
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
JXSaveFileDialog::UpdateDisplay()
{
	JXCSFDialogBase::UpdateDisplay();

	JXDirTable* table = GetFileBrowser();

	bool saveWoutFocus = false;
	if (table->HasFocus())
		{
		if ((table->GetTableSelection()).HasSelection())
			{
			itsSaveButton->SetLabel(JGetString("OpenLabel::JXSaveFileDialog"));
			}
		else
			{
			itsSaveButton->SetLabel(JGetString("SaveLabel::JXSaveFileDialog"));
			saveWoutFocus = true;
			}
		}

	if ((itsFileNameInput->HasFocus() || saveWoutFocus) &&
		(itsFileNameInput->GetText()->GetText().IsEmpty() ||
		 !GetDirInfo()->IsWritable()))
		{
		itsSaveButton->Deactivate();
		}
	else
		{
		itsSaveButton->Activate();
		}

	if (itsFileNameInput->GetText()->GetText().IsEmpty())
		{
		itsXDSSource->Deactivate();
		}
	else
		{
		itsXDSSource->Activate();
		}
}

/******************************************************************************
 Save

 ******************************************************************************/

void
JXSaveFileDialog::Save
	(
	const JString& path
	)
{
	JDirInfo* dirInfo = GetDirInfo();
	const JError err  = dirInfo->GoTo(path);
	err.ReportIfError();
	if (err.OK() && itsFileNameInput->Focus())
		{
		EndDialog(true);
		}
}
