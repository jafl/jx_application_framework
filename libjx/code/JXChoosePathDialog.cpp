/******************************************************************************
 JXChoosePathDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXChoosePathDialog.h>
#include <JXDirTable.h>
#include <JXCurrentPathMenu.h>
#include <JDirInfo.h>
#include <JXNewDirButton.h>

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXPathInput.h>
#include <JXPathHistoryMenu.h>
#include <JXScrollbarSet.h>
#include <jXGlobals.h>

#include <JTableSelection.h>
#include <JUserNotification.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXChoosePathDialog*
JXChoosePathDialog::Create
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JBoolean	selectOnlyWritable,
	const JString&	message
	)
{
	JXChoosePathDialog* dlog =
		jnew JXChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable);
	assert( dlog != NULL );
	dlog->BuildWindow(message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXChoosePathDialog::JXChoosePathDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JBoolean	selectOnlyWritable
	)
	:
	JXCSFDialogBase(supervisor, dirInfo, fileFilter),
	itsSelectOnlyWritableFlag( selectOnlyWritable )
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChoosePathDialog::~JXChoosePathDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXChoosePathDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,370, JString::empty);
	assert( window != NULL );

	JXTextButton* openButton =
		jnew JXTextButton(JGetString("openButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != NULL );
	openButton->SetShortcuts(JGetString("openButton::JXChoosePathDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != NULL );

	JXTextButton* selectButton =
		jnew JXTextButton(JGetString("selectButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,250, 70,20);
	assert( selectButton != NULL );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXStaticText* explanText =
		jnew JXStaticText(JGetString("explanText::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 270,20);
	assert( explanText != NULL );
	explanText->SetToLabel();

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXNewDirButton* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", 0, kJFalse), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, selectButton, cancelButton,
			   upButton, homeButton, desktopButton,
			   newDirButton, showHiddenCB, currPathMenu, message);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXChoosePathDialog::SetObjects
	(
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			pathLabel,
	JXPathInput*			pathInput,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXInputField*			filterInput,
	JXStringHistoryMenu*	filterHistory,
	JXTextButton*			openButton,
	JXTextButton*			selectButton,
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
	itsOpenButton   = openButton;
	itsSelectButton = selectButton;

	(scrollbarSet->GetWindow())->SetTitle(JGetString("Title::JXChoosePathDialog"));

	SetButtons(itsSelectButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		itsOpenButton, upButton, homeButton, desktopButton, newDirButton,
		showHiddenCB, currPathMenu, message);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->AllowSelectFiles(kJFalse, kJFalse);
	UpdateDisplay();

	ListenTo(itsOpenButton);
	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));

	cancelButton->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));
	itsSelectButton->SetShortcuts(JGetString("SelectShortcut::JXChoosePathDialog"));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXChoosePathDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXDirTable* fileBrowser = GetFileBrowser();

	if (sender == itsOpenButton && message.Is(JXButton::kPushed))
		{
		JXPathInput* pathInput    = GetPathInput();
		JXInputField* filterInput = GetFilterInput();

		if (pathInput->HasFocus())
			{
			GoToItsPath();
			}
		else if (filterInput->HasFocus())
			{
			AdjustFilter();
			}
		else if (fileBrowser->HasFocus())
			{
			fileBrowser->GoToSelectedDirectory();
			}
		}

	else if (sender == fileBrowser &&
			 (message.Is(JXWidget::kGotFocus) ||
			  message.Is(JXWidget::kLostFocus)))
		{
		UpdateDisplay();
		}

	else
		{
		if (sender == &(fileBrowser->GetTableSelection()))
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
JXChoosePathDialog::OKToDeactivate()
{
	if (!JXCSFDialogBase::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (GetPathInput()->HasFocus() && !GoToItsPath())
		{
		return kJFalse;
		}

	else if (itsSelectOnlyWritableFlag && !JDirectoryWritable(GetPath()))
		{
		(JGetUserNotification())->ReportError(JGetString("DirMustBeWritable::JXChoosePathDialog"));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
JXChoosePathDialog::UpdateDisplay()
{
	JXCSFDialogBase::UpdateDisplay();

	if (itsSelectOnlyWritableFlag && !GetDirInfo()->IsWritable())
		{
		itsSelectButton->Deactivate();
		}
	else
		{
		itsSelectButton->Activate();
		}

	JXDirTable* dirTable = GetFileBrowser();
	if (dirTable->HasFocus() && !dirTable->HasSelection())
		{
		itsOpenButton->Deactivate();
		}
	else
		{
		itsOpenButton->Activate();
		}
}
