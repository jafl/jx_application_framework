/******************************************************************************
 JXChoosePathDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXChoosePathDialog.h"
#include "JXDirTable.h"
#include "JXCurrentPathMenu.h"
#include <jx-af/jcore/JDirInfo.h>
#include "JXNewDirButton.h"

#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXPathInput.h"
#include "JXPathHistoryMenu.h"
#include "JXScrollbarSet.h"
#include "jXGlobals.h"

#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JUserNotification.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

	This allows derived classes to override BuildWindow().

 ******************************************************************************/

JXChoosePathDialog*
JXChoosePathDialog::Create
	(
	const SelectPathType	type,
	const JString&			startPath,
	const JString&			fileFilter,
	const JString&			message
	)
{
	auto* dlog = jnew JXChoosePathDialog(type, fileFilter);
	dlog->BuildWindow(startPath, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXChoosePathDialog::JXChoosePathDialog
	(
	const SelectPathType	type,
	const JString&			fileFilter
	)
	:
	JXCSFDialogBase(fileFilter),
	itsSelectPathType(type)
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
	const JString& startPath,
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,370, JString::empty);

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::JXChoosePathDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* selectButton =
		jnew JXTextButton(JGetString("selectButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,250, 70,20);
	assert( selectButton != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	auto* explanText =
		jnew JXStaticText(JGetString("explanText::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 270,20);
	assert( explanText != nullptr );
	explanText->SetToLabel();

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, selectButton, cancelButton,
			   upButton, homeButton, desktopButton,
			   newDirButton, showHiddenCB, currPathMenu, startPath, message);
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
	const JString&			startPath,
	const JString&			message
	)
{
	itsOpenButton   = openButton;
	itsSelectButton = selectButton;

	scrollbarSet->GetWindow()->SetTitle(JGetString("Title::JXChoosePathDialog"));

	SetButtons(itsSelectButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		itsOpenButton, upButton, homeButton, desktopButton, newDirButton,
		showHiddenCB, currPathMenu, message);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->AllowSelectFiles(false, false);
	UpdateDisplay();

	ListenTo(itsOpenButton);
	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));

	cancelButton->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));
	itsSelectButton->SetShortcuts(JGetString("SelectShortcut::JXChoosePathDialog"));

	if (!startPath.IsEmpty())
	{
		GetDirInfo()->GoToClosest(startPath);
		DoNotSaveCurrentPath();
	}

	RestoreState();
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

bool
JXChoosePathDialog::OKToDeactivate()
{
	if (!JXCSFDialogBase::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	else if (GetPathInput()->HasFocus() && !GoToItsPath())
	{
		return false;
	}

	else if (itsSelectPathType == kRequireWritable && !JDirectoryWritable(GetPath()))
	{
		JGetUserNotification()->ReportError(JGetString("DirMustBeWritable::JXChoosePathDialog"));
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
JXChoosePathDialog::UpdateDisplay()
{
	JXCSFDialogBase::UpdateDisplay();

	if (itsSelectPathType == kRequireWritable && !GetDirInfo()->IsWritable())
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
