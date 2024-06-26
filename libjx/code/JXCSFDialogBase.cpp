/******************************************************************************
 JXCSFDialogBase.cpp

	Derived classes must call JXCSFDialogBase::SetObjects() after the
	JXLayout code and then set up the extra items.  To allow further derived
	classes, this code should be put in a SetObjects() function instead of
	being inlined after the JXLayout code.

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXCSFDialogBase.h"
#include "JXDirTable.h"
#include "JXCurrentPathMenu.h"
#include <jx-af/jcore/JDirInfo.h>
#include "JXNewDirButton.h"
#include "JXGetNewDirDialog.h"
#include "JXUrgentFunctionTask.h"

#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXPathInput.h"
#include "JXPathHistoryMenu.h"
#include "JXScrollbarSet.h"
#include "JXScrollbar.h"
#include "JXFontManager.h"
#include "jXGlobals.h"

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/JStdError.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength       = 20;
const JCoordinate kMessageMargin = 20;

JString JXCSFDialogBase::theState;
JString JXCSFDialogBase::thePath;

// setup information

const JFileVersion kCurrentSetupVersion = 1;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

	// version 1 stores window geometry and file browser scroll position

/******************************************************************************
 Constructor (protected)

	Derived classes must call SetObjects().

 ******************************************************************************/

JXCSFDialogBase::JXCSFDialogBase
	(
	const JString& fileFilter
	)
	:
	JXModalDialogDirector(true),
	itsPrevFilterString(fileFilter),
	itsDeactCancelFlag(false),
	itsSavePathFlag(true),
	itsSelectPrevDirTask(nullptr)
{
	if (thePath.IsEmpty() || !JDirInfo::Create(thePath, &itsDirInfo))
	{
		JString dirName = JGetCurrentDirectory();
		if (!JDirInfo::Create(dirName, &itsDirInfo))
		{
			bool ok = JGetHomeDirectory(&dirName);
			if (!ok || !JDirInfo::Create(dirName, &itsDirInfo))
			{
				dirName = JGetRootDirectory();
				ok = JDirInfo::Create(dirName, &itsDirInfo);
				assert( ok );
			}
		}
	}
	itsDirInfo->SetWildcardFilter(fileFilter);
	itsDirInfo->ShouldSwitchToValidDirectory();
	ListenTo(itsDirInfo);

	itsPrevPath = itsDirInfo->GetDirectory();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCSFDialogBase::~JXCSFDialogBase()
{
	jdelete itsDirInfo;
	// cannot delete itsSelectPrevDirTask
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
JXCSFDialogBase::GetPath()
	const
{
	return itsDirInfo->GetDirectory();
}

/******************************************************************************
 GetFilter

 ******************************************************************************/

const JString&
JXCSFDialogBase::GetFilter()
	const
{
	return itsFilterInput->GetText()->GetText();
}

/******************************************************************************
 HiddenVisible

 ******************************************************************************/

bool
JXCSFDialogBase::HiddenVisible()
	const
{
	return itsShowHiddenCB->IsChecked();
}

/******************************************************************************
 Activate

	We can't add the strings to the history menus until after ReadSetup().

	The file browser must have the initial focus.

 ******************************************************************************/

void
JXCSFDialogBase::Activate()
{
	JXModalDialogDirector::Activate();

	itsPathHistory->AddString(itsDirInfo->GetDirectory());
	itsFilterHistory->AddString(itsPrevFilterString);

	itsFileBrowser->Focus();
}

/******************************************************************************
 Deactivate (virtual)

	Ignore FocusLost messages during this process.

 ******************************************************************************/

bool
JXCSFDialogBase::Deactivate()
{
	if (!IsActive())
	{
		return true;
	}

	SaveState();
	if (!Cancelled() && itsSavePathFlag)
	{
		thePath = GetPath();
	}

	itsDeactCancelFlag = Cancelled();
	if (JXModalDialogDirector::Deactivate())
	{
		return true;
	}
	else
	{
		// We haven't been deleted.
		itsDeactCancelFlag = false;
		return false;
	}
}

/******************************************************************************
 RestoreState (protected)

	Read in path and filter histories and window geometry.
	This is not virtual because it has to apply to all derived classes.

 ******************************************************************************/

void
JXCSFDialogBase::RestoreState()
{
	JXWindow* window = GetWindow();
	assert( window != nullptr && itsFileBrowser != nullptr );

	if (theState.IsEmpty())
	{
		return;
	}

	std::string s(theState.GetRawBytes(), theState.GetByteCount());
	std::istringstream input(s);

	JFileVersion vers;
	input >> vers >> std::ws;

	if (vers == 0 && input.peek() == '"')	// read old JXChooseSaveFile data
	{
		JString tmp;
		input >> tmp;
		s.assign(tmp.GetRawBytes(), tmp.GetByteCount());
		input.str(s);

		input >> vers;
	}

	if (vers <= kCurrentSetupVersion)
	{
		itsPathHistory->ReadSetup(input);
		itsFilterHistory->ReadSetup(input);

		if (vers >= 1)
		{
			window->ReadGeometry(input);

			JCoordinate y;
			input >> y;

			if (itsSavePathFlag)
			{
				JXScrollbar *hScrollbar, *vScrollbar;
				itsFileBrowser->UpdateScrollbars();
				itsFileBrowser->GetScrollbars(&hScrollbar, &vScrollbar);
				assert( vScrollbar != nullptr );
				vScrollbar->SetValue(y);
			}
		}
	}

	JIgnoreUntil(input, kSetupDataEndDelimiter);

	window->Deiconify();
}

/******************************************************************************
 SaveState (protected)

	Write path and filter histories and window geometry.
	This is not virtual because it has to apply to all derived classes.

 ******************************************************************************/

void
JXCSFDialogBase::SaveState()
	const
{
	assert( itsFileBrowser != nullptr );

	std::ostringstream output;
	output << ' ' << kCurrentSetupVersion;

	output << ' ';
	itsPathHistory->WriteSetup(output);
	output << ' ';
	itsFilterHistory->WriteSetup(output);

	output << ' ';
	GetWindow()->WriteGeometry(output);

	JXScrollbar *hScrollbar, *vScrollbar;
	itsFileBrowser->GetScrollbars(&hScrollbar, &vScrollbar);
	assert( vScrollbar != nullptr );
	output << ' ' << vScrollbar->GetValue();

	output << kSetupDataEndDelimiter;

	theState = output.str();
}

/******************************************************************************
 ReadOldState (static)

 ******************************************************************************/

void
JXCSFDialogBase::ReadOldState
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers >> std::ws;

	if (vers == 0 && input.peek() == '"')
	{
		input >> theState;
	}
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXCSFDialogBase::SetObjects
	(
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			pathLabel,
	JXPathInput*			pathInput,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXInputField*			filterInput,
	JXStringHistoryMenu*	filterHistory,
	JXTextButton*			enterButton,
	JXTextButton*			upButton,
	JXTextButton*			homeButton,
	JXTextButton*			desktopButton,
	JXNewDirButton*			newDirButton,	// can be nullptr
	JXTextCheckbox*			showHiddenCB,
	JXCurrentPathMenu*		currPathMenu,
	const JString&			message
	)
{
	JXContainer* encl = scrollbarSet->GetScrollEnclosure();
	itsFileBrowser =
		jnew JXDirTable(itsDirInfo, scrollbarSet, encl,
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, encl->GetBoundsWidth(), encl->GetBoundsHeight());
	itsFileBrowser->FitToEnclosure();
	itsFileBrowser->InstallShortcuts();

	itsPathInput     = pathInput;
	itsPathHistory   = pathHistory;
	itsFilterInput   = filterInput;
	itsFilterHistory = filterHistory;
	itsEnterButton   = enterButton;
	itsUpButton      = upButton;
	itsHomeButton    = homeButton;
	itsDesktopButton = desktopButton;
	itsNewDirButton  = newDirButton;
	itsShowHiddenCB  = showHiddenCB;
	itsCurrPathMenu  = currPathMenu;

	itsPathInput->GetText()->SetText(itsDirInfo->GetDirectory());
	itsPathInput->SetBasePath(itsDirInfo->GetDirectory());
	itsFilterInput->GetText()->SetText(itsPrevFilterString);
	itsFilterInput->GetText()->SetCharacterInWordFunction(IsCharacterInWord);

	itsPathHistory->SetHistoryLength(kHistoryLength);
	itsFilterHistory->SetHistoryLength(kHistoryLength);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();
	itsFilterInput->SetFont(font);
	itsFilterHistory->SetDefaultFont(font, true);

	itsShowHiddenCB->SetState(itsDirInfo->HiddenVisible());
	itsCurrPathMenu->SetPath(itsDirInfo->GetDirectory());

	itsUpButton->SetShortcuts(JGetString("UpShortcut::JXCSFDialogBase"));
	itsHomeButton->SetShortcuts(JGetString("HomeShortcut::JXCSFDialogBase"));
	itsDesktopButton->SetShortcuts(JGetString("DesktopShortcut::JXCSFDialogBase"));
	if (itsNewDirButton != nullptr)
	{
		itsNewDirButton->SetShortcuts(JGetString("NewDirShortcut::JXCSFDialogBase"));
	}
	itsShowHiddenCB->SetShortcuts(JGetString("ShowHiddenShortcut::JXCSFDialogBase"));

	ListenTo(itsFileBrowser);
	ListenTo(itsPathInput);
	ListenTo(itsPathHistory);
	ListenTo(itsFilterInput);
	ListenTo(itsFilterHistory);
	ListenTo(itsShowHiddenCB);
	ListenTo(itsCurrPathMenu);
	ListenTo(itsUpButton);
	ListenTo(itsHomeButton);
	ListenTo(itsDesktopButton);
	if (itsNewDirButton != nullptr)
	{
		ListenTo(itsNewDirButton);
	}

	// show the message at the top of the window

	DisplayMessage(message, scrollbarSet,
				   pathLabel, itsPathHistory, filterLabel, itsFilterHistory);
}

/******************************************************************************
 DisplayMessage (private)

 ******************************************************************************/

void
JXCSFDialogBase::DisplayMessage
	(
	const JString&			message,
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			pathLabel,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXStringHistoryMenu*	filterHistory
	)
{
	JXWindow* window = GetWindow();
	assert( window != nullptr );

	if (!message.IsEmpty())
	{
		auto* messageObj =
			jnew JXStaticText(message, window,
					JXWidget::kHElastic, JXWidget::kFixedTop,
					20,20, window->GetBoundsWidth()-40,20);

		const JRect apG = messageObj->GetApertureGlobal();

		JSize dw          = 0;
		const JSize prefw = messageObj->TEGetMinPreferredGUIWidth();
		const JSize apw   = apG.width();
		if (prefw > apw)
		{
			dw = prefw - apw;
		}

		JSize dh        = 0;
		const JSize bdh = messageObj->GetBoundsHeight();
		const JSize aph = apG.height();
		if (bdh > aph)
		{
			dh = bdh - aph;
		}

		window->AdjustSize(dw, dh + 2*kMessageMargin);
		messageObj->AdjustSize(0, dh);
	}

	window->LockCurrentMinSize();
	AdjustSizings();

	pathLabel->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedTop);
	itsPathInput->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	pathHistory->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	filterLabel->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedTop);
	itsFilterInput->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	filterHistory->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	itsShowHiddenCB->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedTop);
	itsCurrPathMenu->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedTop);

	itsUpButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	itsHomeButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	itsDesktopButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	if (itsNewDirButton != nullptr)
	{
		itsNewDirButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	}

	scrollbarSet->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);
}

/******************************************************************************
 AdjustSizings (virtual protected)

	Adjust the sizing options for the widgets in the window after the
	message has been installed at the top.

 ******************************************************************************/

void
JXCSFDialogBase::AdjustSizings()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXCSFDialogBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDirInfo && message.Is(JDirInfo::kPathChanged))
	{
		if (IsActive())
		{
			SelectPrevDirectory();
		}
		const JString& newDir = itsDirInfo->GetDirectory();
		itsPathInput->GetText()->SetText(newDir);
		itsPathInput->SetBasePath(newDir);
		itsPathHistory->AddString(newDir);
		itsCurrPathMenu->SetPath(newDir);
		UpdateDisplay();
	}
	else if (sender == itsDirInfo && message.Is(JDirInfo::kPermissionsChanged))
	{
		UpdateDisplay();
	}

	else if (sender == itsFileBrowser && message.Is(JXDirTable::kFileDropped))
	{
		if (itsSelectPrevDirTask != nullptr)
		{
			itsSelectPrevDirTask->Cancel();
			itsSelectPrevDirTask = nullptr;
		}
	}

	else if (sender == itsPathInput && message.Is(JXWidget::kGotFocus))
	{
		itsEnterButton->SetLabel(JGetString("GoToLabel::JXCSFDialogBase"));
	}
	else if (sender == itsPathInput && message.Is(JXWidget::kLostFocus) &&
			 !itsDeactCancelFlag)
	{
		itsEnterButton->SetLabel(JGetString("OpenLabel::JXCSFDialogBase"));
		GoToItsPath();
	}

	else if (sender == itsFilterInput && message.Is(JXWidget::kGotFocus))
	{
		itsEnterButton->SetLabel(JGetString("ApplyLabel::JXCSFDialogBase"));
	}
	else if (sender == itsFilterInput && message.Is(JXWidget::kLostFocus) &&
			 !itsDeactCancelFlag)
	{
		itsEnterButton->SetLabel(JGetString("OpenLabel::JXCSFDialogBase"));
		AdjustFilter();
	}

	else if (sender == itsPathHistory && message.Is(JXMenu::kItemSelected))
	{
		itsPathHistory->UpdateInputField(message, itsPathInput);
		GoToItsPath();
	}
	else if (sender == itsFilterHistory && message.Is(JXMenu::kItemSelected))
	{
		itsFilterHistory->UpdateInputField(message, itsFilterInput);
		AdjustFilter();
	}

	else if (sender == itsShowHiddenCB && message.Is(JXCheckbox::kPushed))
	{
		auto& state = dynamic_cast<const JXCheckbox::Pushed&>(message);
		itsFileBrowser->ShowHidden(state.IsChecked());
	}

	else if (sender == itsCurrPathMenu && message.Is(JXMenu::kItemSelected))
	{
		const JString newPath = itsCurrPathMenu->GetPath(message);
		itsDirInfo->GoTo(newPath);
	}

	else if (sender == itsUpButton && message.Is(JXButton::kPushed))
	{
		const JError err = itsDirInfo->GoUp();
		if (err.OK())
		{
			itsFileBrowser->Focus();
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("NoAccessUp::JXCSFDialogBase"));
		}
	}

	else if (sender == itsHomeButton && message.Is(JXButton::kPushed))
	{
		JString homeDir;
		const bool found = JGetHomeDirectory(&homeDir);
		if (found)
		{
			itsDirInfo->GoTo(homeDir);
			itsFileBrowser->Focus();
		}
		else if (!found)
		{
			JGetUserNotification()->ReportError(JGetString("NoHomeDir::JXCSFDialogBase"));
		}
	}

	else if (sender == itsDesktopButton && message.Is(JXButton::kPushed))
	{
		JString desktopDir;
		const bool found = JGetHomeDirectory(&desktopDir);
		if (found)
		{
			desktopDir = JCombinePathAndName(desktopDir, JGetString("DesktopName::JXCSFDialogBase"));
			itsDirInfo->GoTo(desktopDir);
			itsFileBrowser->Focus();
		}
		else if (!found)
		{
			JGetUserNotification()->ReportError(JGetString("NoHomeDir::JXCSFDialogBase"));
		}
	}

	else if (sender == itsNewDirButton && message.Is(JXButton::kPushed))
	{
		CreateNewDirectory();
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 GoToItsPath (protected)

 ******************************************************************************/

bool
JXCSFDialogBase::GoToItsPath()
{
	if (itsPathInput->InputValid())
	{
		JString path;
		const bool ok = itsPathInput->GetPath(&path);
		assert( ok );

		const JError err = itsDirInfo->GoTo(path);
		assert_ok( err );
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 UpdateDisplay (virtual protected)

	Derived classes can override this to perform other adjustments.

 ******************************************************************************/

void
JXCSFDialogBase::UpdateDisplay()
{
	if (JIsRootDirectory(itsDirInfo->GetDirectory()))
	{
		itsUpButton->Deactivate();
	}
	else
	{
		itsUpButton->Activate();
	}

	if (itsNewDirButton != nullptr && itsDirInfo->IsWritable())
	{
		itsNewDirButton->Activate();
	}
	else if (itsNewDirButton != nullptr)
	{
		itsNewDirButton->Deactivate();
	}
}

/******************************************************************************
 AdjustFilter (protected)

 ******************************************************************************/

void
JXCSFDialogBase::AdjustFilter()
{
	const JString& newFilter = itsFilterInput->GetText()->GetText();
	if (newFilter != itsPrevFilterString)
	{
		itsDirInfo->SetWildcardFilter(newFilter);
		itsPrevFilterString = newFilter;
		itsFilterHistory->AddString(newFilter);
	}
}

/******************************************************************************
 CreateNewDirectory (private)

	Display a blocking dialog window to get the name of the new directory.

 ******************************************************************************/

void
JXCSFDialogBase::CreateNewDirectory()
{
	auto* dlog =
		jnew JXGetNewDirDialog(JGetString("NewDirWindowTitle::JXCSFDialogBase"),
							   JGetString("NewDirPrompt::JXCSFDialogBase"),
							   JString::empty, itsDirInfo->GetDirectory());

	if (dlog->DoDialog())
	{
		const JString newDirName = dlog->GetNewDirName();

		JError err = JNoError();
		if (JCreateDirectory(newDirName, &err))
		{
			itsDirInfo->GoTo(newDirName);
		}
		else if (err == kJDirEntryAlreadyExists)
		{
			JGetUserNotification()->ReportError(JGetString("DirectoryExists::JXGlobal"));
		}
		else if (err == kJAccessDenied)
		{
			JGetUserNotification()->ReportError(JGetString("DirNotWritable::JXGlobal"));
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("CannotCreateDir::JXCSFDialogBase"));
		}
	}
}

/******************************************************************************
 SelectPrevDirectory (private)

	If the new path is up a directory from the original path, we select
	the directory that leads to the original path.

 ******************************************************************************/

void
JXCSFDialogBase::SelectPrevDirectory()
{
	const JString& newPath = itsDirInfo->GetDirectory();

	if (itsPrevPath.StartsWith(newPath))
	{
		// remove common base path

		JString dirName = itsPrevPath;
		JStringIterator iter(&dirName);
		iter.Next(newPath);
		iter.RemoveAllPrev();

		// remove leading /

		JUtf8Character c;
		while (iter.Next(&c) && c == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
			iter.RemovePrev();
		}

		// keep only first dir name past common base path

		if (iter.Next(ACE_DIRECTORY_SEPARATOR_STR))
		{
			iter.RemovePrev();
			iter.RemoveAllNext();
		}

		iter.Invalidate();

		// create UrgentTask to select this item

		if (!dirName.IsEmpty())
		{
			itsSelectPrevDirTask = jnew JXUrgentFunctionTask(itsFileBrowser, [this, dirName]()
			{
				itsSelectPrevDirTask = nullptr;

				JIndex index;
				if (itsDirInfo->FindEntry(dirName, &index))
				{
					itsFileBrowser->UpdateScrollbars();
					itsFileBrowser->SelectSingleEntry(index);
				}
			},
			"JXCSFDialogBase::SelectPrevDirectory");
			itsSelectPrevDirTask->Go();
		}
	}

	itsPrevPath = newPath;
}

/******************************************************************************
 IsCharacterInWord (static)

	Returns true if the given character should be considered part of
	a word.  Our definition is [A-Za-z0-9_].

 ******************************************************************************/

bool
JXCSFDialogBase::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return c.IsAlnum() || c == '_';
}
