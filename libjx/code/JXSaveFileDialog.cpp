/******************************************************************************
 JXSaveFileDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXSaveFileDialog.h>
#include <JXDirTable.h>
#include <JXCurrentPathMenu.h>
#include <JDirInfo.h>
#include <JXNewDirButton.h>
#include <JXDirectSaveSource.h>

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXSaveFileInput.h>
#include <JXPathInput.h>
#include <JXPathHistoryMenu.h>
#include <JXScrollbarSet.h>
#include <jXGlobals.h>

#include <JString.h>
#include <JTableSelection.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

// enter button says "Save" while itsFileNameInput has focus

static const JCharacter* kOpenLabel = "Open";
static const JCharacter* kSaveLabel = "Save";

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXSaveFileDialog*
JXSaveFileDialog::Create
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
	JXSaveFileDialog* dlog =
		jnew JXSaveFileDialog(supervisor, dirInfo, fileFilter);
	assert( dlog != NULL );
	dlog->BuildWindow(origName, prompt, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXSaveFileDialog::JXSaveFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter
	)
	:
	JXCSFDialogBase(supervisor, dirInfo, fileFilter)
{
	itsFileName = jnew JString;
	assert( itsFileName != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSaveFileDialog::~JXSaveFileDialog()
{
	jdelete itsFileName;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

JBoolean
JXSaveFileDialog::GetFileName
	(
	JString* name
	)
	const
{
	*name = *itsFileName;
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
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,340, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != NULL );

	JXTextButton* saveButton =
		jnew JXTextButton(JGetString("saveButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != NULL );
	saveButton->SetShortcuts(JGetString("saveButton::JXSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXSaveFileInput* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != NULL );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXStaticText* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != NULL );
	promptLabel->SetToLabel();

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXNewDirButton* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

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
	const JCharacter*		prompt,
	JXSaveFileInput*		fileNameInput,
	const JCharacter*		origName,
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
	const JCharacter*		message
	)
{
	itsSaveButton    = saveButton;
	itsFileNameInput = fileNameInput;

	itsFileNameInput->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress

	JXWindow* window = scrollbarSet->GetWindow();
	window->SetTitle("Save file");

	SetButtons(itsSaveButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		itsSaveButton, upButton, homeButton, desktopButton,
		newDirButton, showHiddenCB, currPathMenu, message);

	JXDirTable* table = GetFileBrowser();
	table->AllowSelectFiles(kJFalse, kJFalse);
	table->AllowDblClickInactive(kJTrue);
	promptLabel->SetText(prompt);
	itsFileNameInput->SetText(origName);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->ShouldSelectWhenChangePath(kJFalse);

	const JRect frame   = itsFileNameInput->GetFrame();
	const JCoordinate w = frame.height();
	itsXDSSource =
		jnew JXDirectSaveSource(this, itsFileNameInput, window,
							   JXWidget::kFixedLeft, JXWidget::kFixedBottom,
							   frame.left, frame.top, w, frame.height());
	assert( itsXDSSource != NULL );
	itsFileNameInput->Move(w, 0);
	itsFileNameInput->AdjustSize(-w, 0);

	UpdateDisplay();

	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));
	ListenTo(itsFileNameInput);

	cancelButton->SetShortcuts("^[");
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

	if (sender == itsFileNameInput)
		{
		if (message.Is(JXWidget::kGotFocus))
			{
			itsSaveButton->SetLabel(kSaveLabel);
			UpdateDisplay();
			}
		else if (message.Is(JXWidget::kLostFocus))
			{
			itsSaveButton->SetLabel(kOpenLabel);
			UpdateDisplay();
			}
		else if (message.Is(JTextEditor::kTextChanged) ||
				 message.Is(JTextEditor::kTextSet))
			{
			UpdateDisplay();
			}
		}

	else if (sender == fileBrowser && message.Is(JXDirTable::kFileDblClicked))
		{
		const JXDirTable::FileDblClicked* info =
			dynamic_cast<const JXDirTable::FileDblClicked*>(&message);
		assert( info != NULL );
		const JString fileName = (info->GetDirEntry()).GetName();
		itsFileNameInput->SetText(fileName);
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

JBoolean
JXSaveFileDialog::OKToDeactivate()
{
	if (!JXCSFDialogBase::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	JXPathInput* pathInput = GetPathInput();
	if (pathInput->HasFocus())
		{
		GoToItsPath();
		return kJFalse;
		}

	JXInputField* filterInput = GetFilterInput();
	if (filterInput->HasFocus())
		{
		AdjustFilter();
		return kJFalse;
		}

	JXDirTable* fileBrowser = GetFileBrowser();
	if (fileBrowser->HasFocus() && fileBrowser->GoToSelectedDirectory())
		{
		return kJFalse;
		}

	const JString& fileName = itsFileNameInput->GetText();
	if (fileName.IsEmpty())
		{
		(JGetUserNotification())->ReportError("You need to enter a file name.");
		return kJFalse;
		}

	const JString& path     = GetPath();
	const JString fullName  = path + fileName;

	const JBoolean fileExists = JFileExists(fullName);

	if (JDirectoryExists(fullName))
		{
		(JGetUserNotification())->ReportError(
			"This name is already used for a directory.");
		return kJFalse;
		}
	else if (!JDirectoryWritable(path) && !fileExists)
		{
		(JGetUserNotification())->ReportError(
			"You are not allowed to write to this directory.");
		return kJFalse;
		}
	else if (!fileExists)
		{
		*itsFileName = fileName;
		return kJTrue;
		}
	else if (!JFileWritable(fullName))
		{
		(JGetUserNotification())->ReportError(
			"You are not allowed to write to this file.");
		return kJFalse;
		}
	else if ((JGetUserNotification())->AskUserNo("That file already exists.  Replace it?"))
		{
		*itsFileName = fileName;
		return kJTrue;
		}
	else
		{
		return kJFalse;
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

	JBoolean saveWoutFocus = kJFalse;
	if (table->HasFocus())
		{
		if ((table->GetTableSelection()).HasSelection())
			{
			itsSaveButton->SetLabel(kOpenLabel);
			}
		else
			{
			itsSaveButton->SetLabel(kSaveLabel);
			saveWoutFocus = kJTrue;
			}
		}

	if ((itsFileNameInput->HasFocus() || saveWoutFocus) &&
		((itsFileNameInput->GetText()).IsEmpty() ||
		 !GetDirInfo()->IsWritable()))
		{
		itsSaveButton->Deactivate();
		}
	else
		{
		itsSaveButton->Activate();
		}

	if ((itsFileNameInput->GetText()).IsEmpty())
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
	const JCharacter* path
	)
{
	JDirInfo* dirInfo = GetDirInfo();
	const JError err  = dirInfo->GoTo(path);
	err.ReportIfError();
	if (err.OK() && itsFileNameInput->Focus())
		{
		EndDialog(kJTrue);
		}
}
