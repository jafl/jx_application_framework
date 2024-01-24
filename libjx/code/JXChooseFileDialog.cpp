/******************************************************************************
 JXChooseFileDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXChooseFileDialog.h"
#include "JXDirTable.h"
#include "JXCurrentPathMenu.h"

#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXPathInput.h"
#include "JXPathHistoryMenu.h"
#include "JXScrollbarSet.h"
#include "jXGlobals.h"

#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

	This allows derived classes to override BuildWindow().

 ******************************************************************************/

JXChooseFileDialog*
JXChooseFileDialog::Create
	(
	const SelectType	selectType,
	const JString&		selectName,
	const JString&		fileFilter,
	const JString&		message
	)
{
	auto* dlog = jnew JXChooseFileDialog(fileFilter);
	dlog->BuildWindow(selectType, selectName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXChooseFileDialog::JXChooseFileDialog
	(
	const JString& fileFilter
	)
	:
	JXCSFDialogBase(fileFilter)
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

const JString&
JXChooseFileDialog::GetFullName()
	const
{
	const JDirEntry* entry;
	const bool ok = GetFileBrowser()->GetFirstSelection(&entry);
	assert( ok );

	return entry->GetFullName();
}

/******************************************************************************
 GetFullNames

 ******************************************************************************/

void
JXChooseFileDialog::GetFullNames
	(
	JPtrArray<JString>* fullNameList
	)
	const
{
	fullNameList->CleanOut();

	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	const bool ok = GetFileBrowser()->GetSelection(&entryList);
	assert( ok );

	for (const auto* entry : entryList)
	{
		auto* s = jnew JString(entry->GetFullName());
		assert( s != nullptr );
		fullNameList->Append(s);
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXChooseFileDialog::BuildWindow
	(
	const SelectType	selectType,
	const JString&		selectName,
	const JString&		message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,340, JString::empty);

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	pathLabel->SetToLabel(false);

	auto* pathHistory =
		jnew JXPathHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	auto* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	filterLabel->SetToLabel(false);

	auto* filterHistory =
		jnew JXStringHistoryMenu(1, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	auto* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	auto* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != nullptr );

	auto* upButton =
		jnew JXTextButton(JGetString("upButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	auto* homeButton =
		jnew JXTextButton(JGetString("homeButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	auto* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

	auto* selectAllButton =
		jnew JXTextButton(JGetString("selectAllButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( selectAllButton != nullptr );

	auto* openButton =
		jnew JXTextButton(JGetString("openButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 219,279, 72,22);
	openButton->SetShortcuts(JGetString("openButton::shortcuts::JXChooseFileDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	auto* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	pathInput->SetIsRequired(true);
	pathInput->ShouldAllowInvalidPath(false);
	pathInput->ShouldRequireWritable(false);

	auto* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton, desktopButton,
			   selectAllButton, showHiddenCB, currPathMenu,
			   selectType, selectName, message);
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
	const SelectType		selectType,
	const JString&			selectName,
	const JString&			message
	)
{
	itsOpenButton      = openButton;
	itsSelectAllButton = selectAllButton;

	SetButtons(openButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		openButton, upButton, homeButton, desktopButton, nullptr, showHiddenCB,
		currPathMenu, message);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->AllowSelectFiles(true, selectType == kSelectMultipleFiles);
	ListenTo(fileBrowser);
	ListenTo(&fileBrowser->GetTableSelection());

	if (selectType == kSelectMultipleFiles)
	{
		scrollbarSet->GetWindow()->SetTitle(JGetString("ChooseFilesTitle::JXChooseFileDialog"));
		ListenTo(itsSelectAllButton);
	}
	else
	{
		scrollbarSet->GetWindow()->SetTitle(JGetString("ChooseFileTitle::JXChooseFileDialog"));
		itsSelectAllButton->Hide();
	}

	cancelButton->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));

	JDirInfo* dirInfo = GetDirInfo();
	JString name;
	if (selectName.Contains(ACE_DIRECTORY_SEPARATOR_STR))
	{
		JString path;
		JSplitPathAndName(selectName, &path, &name);
		dirInfo->GoToClosest(path);
		if (!JSameDirEntry(path, dirInfo->GetDirectory()))
		{
			name.Clear();
		}
		DoNotSaveCurrentPath();
	}

	RestoreState();

	JIndex index;
	if (!name.IsEmpty() &&
		fileBrowser->ClosestMatch(name, &index))
	{
		const JDirEntry& entry = dirInfo->GetEntry(index);
		if (entry.GetName() == name)
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
