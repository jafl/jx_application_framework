/******************************************************************************
 JXChooseFileDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXChooseFileDialog.h"
#include "jx-af/jx/JXDirTable.h"
#include "jx-af/jx/JXCurrentPathMenu.h"
#include <jx-af/jcore/JDirInfo.h>

#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXStaticText.h"
#include "jx-af/jx/JXTextButton.h"
#include "jx-af/jx/JXTextCheckbox.h"
#include "jx-af/jx/JXPathInput.h"
#include "jx-af/jx/JXPathHistoryMenu.h"
#include "jx-af/jx/JXScrollbarSet.h"
#include "jx-af/jx/jXGlobals.h"

#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXChooseFileDialog*
JXChooseFileDialog::Create
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple,
	const JString&	origName,
	const JString&	message
	)
{
	auto* dlog =
		jnew JXChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != nullptr );
	dlog->BuildWindow(origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXChooseFileDialog::JXChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	allowSelectMultiple
	)
	:
	JXCSFDialogBase(supervisor, dirInfo, fileFilter),
	itsSelectMultipleFlag(allowSelectMultiple)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseFileDialog::~JXChooseFileDialog()
{
}

/******************************************************************************
 GetFullName

 ******************************************************************************/

bool
JXChooseFileDialog::GetFullName
	(
	JString* fullName
	)
	const
{
	const JDirEntry* entry;
	if (GetFileBrowser()->GetFirstSelection(&entry))
	{
		*fullName = entry->GetFullName();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetFullNames

 ******************************************************************************/

bool
JXChooseFileDialog::GetFullNames
	(
	JPtrArray<JString>* fullNameList
	)
	const
{
	fullNameList->CleanOut();

	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	if (GetFileBrowser()->GetSelection(&entryList))
	{
		for (const auto* entry : entryList)
		{
			auto* s = jnew JString(entry->GetFullName());
			assert( s != nullptr );
			fullNameList->Append(s);
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXChooseFileDialog::BuildWindow
	(
	const JString& origName,
	const JString& message
	)
{
// begin JXLayout

	const auto* window = jnew JXWindow(this, 310,340, JString::empty);
	assert( window != nullptr );

	const auto* openButton =
		jnew JXTextButton(JGetString("openButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( openButton != nullptr );
	openButton->SetShortcuts(JGetString("openButton::JXChooseFileDialog::shortcuts::JXLayout"));

	const auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	const auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	const auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	const auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	const auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	const auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	const auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	const auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	const auto* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	const auto* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	const auto* upButton =
		jnew JXTextButton(JGetString("upButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	const auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	const auto* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", JString::kNoCopy), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	const auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton, desktopButton,
			   selectAllButton, showHiddenCB, currPathMenu,
			   origName, message);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXChooseFileDialog::SetObjects
	(
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			pathLabel,
	JXPathInput*			pathInput,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXInputField*			filterInput,
	JXStringHistoryMenu*	filterHistory,
	JXTextButton*			openButton,
	JXTextButton*			cancelButton,
	JXTextButton*			upButton,
	JXTextButton*			homeButton,
	JXTextButton*			desktopButton,
	JXTextButton*			selectAllButton,
	JXTextCheckbox*			showHiddenCB,
	JXCurrentPathMenu*		currPathMenu,
	const JString&			origName,
	const JString&			message
	)
{
	if (itsSelectMultipleFlag)
	{
		(scrollbarSet->GetWindow())->SetTitle(JGetString("ChooseFilesTitle::JXChooseFileDialog"));
	}
	else
	{
		(scrollbarSet->GetWindow())->SetTitle(JGetString("ChooseFileTitle::JXChooseFileDialog"));
	}

	itsOpenButton      = openButton;
	itsSelectAllButton = selectAllButton;

	SetButtons(openButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		openButton, upButton, homeButton, desktopButton, nullptr, showHiddenCB,
		currPathMenu, message);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->AllowSelectFiles(true, itsSelectMultipleFlag);
	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));

	if (itsSelectMultipleFlag)
	{
		ListenTo(itsSelectAllButton);
	}
	else
	{
		itsSelectAllButton->Hide();
	}

	cancelButton->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));

	// select initial file

	JIndex index;
	if (!origName.IsEmpty() &&
		fileBrowser->ClosestMatch(origName, &index))
	{
		const JDirEntry& entry = GetDirInfo()->GetEntry(index);
		if (entry.GetName() == origName)
		{
			fileBrowser->UpdateScrollbars();
			fileBrowser->SelectSingleEntry(index);
		}
	}
}

/******************************************************************************
 AdjustSizings (virtual protected)

	Adjust the sizing options for the widgets in the window after the
	message has been installed at the top.

 ******************************************************************************/

void
JXChooseFileDialog::AdjustSizings()
{
	JXCSFDialogBase::AdjustSizings();
	itsSelectAllButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXChooseFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXDirTable* fileBrowser = GetFileBrowser();

	if (sender == fileBrowser && message.Is(JXDirTable::kFileDblClicked))
	{
		const auto* info =
			dynamic_cast<const JXDirTable::FileDblClicked*>(&message);
		assert( info != nullptr );
		if (info->IsActive())
		{
			EndDialog(true);
		}
	}

	else if (sender == itsSelectAllButton && message.Is(JXButton::kPushed))
	{
		fileBrowser->SelectAll();
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
 UpdateDisplay (virtual protected)

	Disable the Open button if nothing is selected.

 ******************************************************************************/

void
JXChooseFileDialog::UpdateDisplay()
{
	JXCSFDialogBase::UpdateDisplay();

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

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXChooseFileDialog::OKToDeactivate()
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
	if (fileBrowser->GoToSelectedDirectory())
	{
		return false;
	}

	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	if (fileBrowser->GetSelection(&entryList))
	{
		for (auto* entry : entryList)
		{
			entry->ForceUpdate();	// check that link hasn't been broken behind our back
			if (!entry->IsFile())
			{
				GetDirInfo()->ForceUpdate();
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
