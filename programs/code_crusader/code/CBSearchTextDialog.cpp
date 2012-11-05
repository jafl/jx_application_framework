/******************************************************************************
 CBSearchTextDialog.cpp

	BASE CLASS = JXSearchTextDialog, JPrefObject

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBSearchTextDialog.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBSearchPathHistoryMenu.h"
#include "CBSearchFilterHistoryMenu.h"
#include "CBListCSF.h"
#include "CBProjectDocument.h"
#include "cbHelpText.h"
#include "cbmUtil.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXPathInput.h>
#include <JXTextButton.h>
#include <JXSearchTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXScrollbarSet.h>
#include <JXFileListTable.h>
#include <JXStringHistoryMenu.h>
#include <JXDocumentMenu.h>
#include <JXDownRect.h>
#include <JXStandAlonePG.h>
#include <JXChooseSaveFile.h>
#include <JDirInfo.h>
#include <JRegex.h>
#include <jVCSUtil.h>
#include <JLatentPG.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <sstream>
#include <stdio.h>
#include <jAssert.h>

static const JCharacter* kReplaceFileListStr  = "Replace contents";
static const JCharacter* kAppendToFileListStr = "Merge with contents";

// setup information

const JFileVersion kCurrentSetupVersion = 73;	// first digit must be >= 7

	// version 73 stores path filter info
	// version 72 stores itsInvertFileFilterCB
	// version 71 stores directory information
	// version 7  split off JXSearchTextDialog
	// version 6  no longer stores search direction
	// version 5  stores itsListFilesWithoutMatchFlag, itsFileSetName
	// version 4  stores search file list, itsOnlyListFilesFlag
	// version 3  stores singleLine, matchCase
	// version 2  stores replaceIsRegex
	// version 1  uses JXWindow::Read/WriteGeometry()

// File list menu

static const JCharacter* kFileListMenuStr =
	"    Show text of each match      %b"
	"  | List files that do not match %b"
	"%l| Add files..."
	"  | Remove selected files        %k Backspace."
	"  | Remove all files"
	"%l| Load file set..."
	"  | Save file set...";

enum
{
	kShowMatchTextCmd = 1, kListFilesWithoutMatchCmd,
	kAddFilesCmd, kRemoveSelCmd, kRemoveAllCmd,
	kLoadFileSetCmd, kSaveFileSetCmd
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBSearchTextDialog*
CBSearchTextDialog::Create()
{
	CBSearchTextDialog* dlog = new CBSearchTextDialog;
	assert( dlog != NULL );
	dlog->BuildWindow();
	dlog->JXSearchTextDialogX();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBSearchTextDialog::CBSearchTextDialog()
	:
	JXSearchTextDialog(kCBSearchTextHelpName),
	JPrefObject(CBGetPrefsManager(), kCBSearchTextID),
	itsFileSetName("Untitled")
{
	itsCSF = new CBListCSF(kReplaceFileListStr, kAppendToFileListStr);
	assert( itsCSF != NULL );

	itsOnlyListFilesFlag         = kJFalse;
	itsListFilesWithoutMatchFlag = kJFalse;

	itsReplaceProcess = NULL;
	itsReplaceLink    = NULL;
	itsReplacePG      = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchTextDialog::~CBSearchTextDialog()
{
	assert( itsReplaceProcess == NULL && itsReplacePG == NULL );
	CBSearchDocument::DeleteReplaceLink(&itsReplaceLink);

	// prefs written by CBDeleteGlobals()

	delete itsCSF;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBSearchTextDialog::Activate()
{
	JXSearchTextDialog::Activate();

	if (IsActive())
		{
		itsMultifileCB->SetState(kJFalse);
		itsSearchDirCB->SetState(kJFalse);
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBSearchTextDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 450,470, "");
	assert( window != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,101, 90,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,31, 80,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXInputField* searchInput =
		new JXInputField(kJTrue, kJFalse, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,50, 220,40);
	assert( searchInput != NULL );

	JXInputField* replaceInput =
		new JXInputField(kJTrue, kJFalse, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,120, 220,40);
	assert( replaceInput != NULL );

	JXTextButton* closeButton =
		new JXTextButton(JGetString("closeButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,440, 80,20);
	assert( closeButton != NULL );

	JXTextCheckbox* ignoreCaseCB =
		new JXTextCheckbox(JGetString("ignoreCaseCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 130,20);
	assert( ignoreCaseCB != NULL );

	JXTextCheckbox* wrapSearchCB =
		new JXTextCheckbox(JGetString("wrapSearchCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 130,20);
	assert( wrapSearchCB != NULL );

	JXTextButton* replaceButton =
		new JXTextButton(JGetString("replaceButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
	assert( replaceButton != NULL );

	JXTextCheckbox* entireWordCB =
		new JXTextCheckbox(JGetString("entireWordCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 130,20);
	assert( entireWordCB != NULL );

	JXStringHistoryMenu* prevReplaceMenu =
		new JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 30,20);
	assert( prevReplaceMenu != NULL );

	JXStringHistoryMenu* prevSearchMenu =
		new JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 30,20);
	assert( prevSearchMenu != NULL );

	JXTextButton* helpButton =
		new JXTextButton(JGetString("helpButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,440, 80,20);
	assert( helpButton != NULL );
	helpButton->SetShortcuts(JGetString("helpButton::CBSearchTextDialog::shortcuts::JXLayout"));

	JXTextCheckbox* searchIsRegexCB =
		new JXTextCheckbox(JGetString("searchIsRegexCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,180, 150,20);
	assert( searchIsRegexCB != NULL );

	JXTextCheckbox* replaceIsRegexCB =
		new JXTextCheckbox(JGetString("replaceIsRegexCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,180, 110,20);
	assert( replaceIsRegexCB != NULL );

	JXTextCheckbox* singleLineCB =
		new JXTextCheckbox(JGetString("singleLineCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,210, 150,20);
	assert( singleLineCB != NULL );

	JXTextCheckbox* preserveCaseCB =
		new JXTextCheckbox(JGetString("preserveCaseCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,210, 110,20);
	assert( preserveCaseCB != NULL );

	JXTextButton* qRefButton =
		new JXTextButton(JGetString("qRefButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,440, 80,20);
	assert( qRefButton != NULL );

	itsMultifileCB =
		new JXTextCheckbox(JGetString("itsMultifileCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,360, 100,20);
	assert( itsMultifileCB != NULL );
	itsMultifileCB->SetShortcuts(JGetString("itsMultifileCB::CBSearchTextDialog::shortcuts::JXLayout"));

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,360, 300,60);
	assert( scrollbarSet != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 355,20, 40,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXSearchTextButton* findBackButton =
		new JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
	assert( findBackButton != NULL );

	JXSearchTextButton* findFwdButton =
		new JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
	assert( findFwdButton != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 323,80, 100,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXSearchTextButton* replaceFindBackButton =
		new JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
	assert( replaceFindBackButton != NULL );

	JXSearchTextButton* replaceFindFwdButton =
		new JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
	assert( replaceFindFwdButton != NULL );

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 335,110, 80,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	JXSearchTextButton* replaceAllBackButton =
		new JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 20,20);
	assert( replaceAllBackButton != NULL );

	JXSearchTextButton* replaceAllFwdButton =
		new JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,110, 20,20);
	assert( replaceAllFwdButton != NULL );

	JXTextButton* replaceAllInSelButton =
		new JXTextButton(JGetString("replaceAllInSelButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,140, 140,20);
	assert( replaceAllInSelButton != NULL );

	JXTextCheckbox* stayOpenCB =
		new JXTextCheckbox(JGetString("stayOpenCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,240, 150,20);
	assert( stayOpenCB != NULL );

	JXTextCheckbox* retainFocusCB =
		new JXTextCheckbox(JGetString("retainFocusCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,240, 110,20);
	assert( retainFocusCB != NULL );

	itsFileListMenu =
		new JXTextMenu(JGetString("itsFileListMenu::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,390, 90,30);
	assert( itsFileListMenu != NULL );

	itsSearchDirCB =
		new JXTextCheckbox(JGetString("itsSearchDirCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,270, 130,20);
	assert( itsSearchDirCB != NULL );
	itsSearchDirCB->SetShortcuts(JGetString("itsSearchDirCB::CBSearchTextDialog::shortcuts::JXLayout"));

	itsDirInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,270, 190,20);
	assert( itsDirInput != NULL );

	JXStaticText* obj6_JXLayout =
		new JXStaticText(JGetString("obj6_JXLayout::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,300, 100,20);
	assert( obj6_JXLayout != NULL );
	obj6_JXLayout->SetToLabel();

	itsDirHistory =
		new CBSearchPathHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,270, 30,20);
	assert( itsDirHistory != NULL );

	itsFileFilterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,300, 190,20);
	assert( itsFileFilterInput != NULL );

	itsFileFilterHistory =
		new CBSearchFilterHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,300, 30,20);
	assert( itsFileFilterHistory != NULL );

	itsRecurseDirCB =
		new JXTextCheckbox(JGetString("itsRecurseDirCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,330, 230,20);
	assert( itsRecurseDirCB != NULL );

	itsChooseDirButton =
		new JXTextButton(JGetString("itsChooseDirButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,270, 60,20);
	assert( itsChooseDirButton != NULL );

	itsInvertFileFilterCB =
		new JXTextCheckbox(JGetString("itsInvertFileFilterCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,300, 60,20);
	assert( itsInvertFileFilterCB != NULL );

	itsPathFilterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 270,330, 140,20);
	assert( itsPathFilterInput != NULL );

	itsPathFilterHistory =
		new CBSearchFilterHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,330, 30,20);
	assert( itsPathFilterHistory != NULL );

// end JXLayout

	SetObjects(searchInput, prevSearchMenu,
			   ignoreCaseCB, entireWordCB, wrapSearchCB,
			   searchIsRegexCB, singleLineCB,
			   replaceInput, prevReplaceMenu,
			   replaceIsRegexCB, preserveCaseCB,
			   stayOpenCB, retainFocusCB,
			   findFwdButton, findBackButton,
			   replaceButton,
			   replaceFindFwdButton, replaceFindBackButton,
			   replaceAllFwdButton, replaceAllBackButton,
			   replaceAllInSelButton,
			   closeButton, helpButton, qRefButton);

	ListenTo(itsSearchDirCB);
	ListenTo(itsDirInput);
	ListenTo(itsMultifileCB);

	itsFileList =
		new JXFileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);
	assert( itsFileList != NULL );
	itsFileList->FitToEnclosure();
	itsFileList->ShouldAcceptFileDrop();
	itsFileList->BackspaceShouldRemoveSelectedFiles();
	ListenTo(itsFileList);

	itsFileListMenu->SetMenuItems(kFileListMenuStr, "CBSearchTextDialog");
	ListenTo(itsFileListMenu);

	ListenTo(itsChooseDirButton);
	ListenTo(itsDirHistory);
	ListenTo(itsFileFilterHistory);
	ListenTo(itsPathFilterHistory);

	itsDirInput->ShouldAllowInvalidPath();
	itsDirHistory->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle(), kJTrue);
	itsFileFilterInput->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());
	itsFileFilterHistory->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle(), kJTrue);
	itsPathFilterInput->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());
	itsPathFilterHistory->SetDefaultFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle(), kJTrue);
	itsRecurseDirCB->SetState(kJTrue);

	UpdateBasePath();
	ListenTo(CBGetDocumentManager());

	CBPrefsManager* prefsMgr = CBGetPrefsManager();
	JPrefObject::ReadPrefs();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize);

	searchInput->SetCharacterInWordFunction(CBMIsCharacterInWord);
	replaceInput->SetCharacterInWordFunction(CBMIsCharacterInWord);

	// create hidden JXDocument so Meta-# shortcuts work

	JXDocumentMenu* fileListMenu =
		new JXDocumentMenu("", window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != NULL );

	// decor

	const JRect wFrame  = window->GetFrame();
	const JRect cbFrame = itsSearchDirCB->GetFrame();

	JXDownRect* line =
		new JXDownRect(window, JXWidget::kFixedLeft, JXWidget::kFixedTop,
					   cbFrame.left, cbFrame.top-6,
					   wFrame.right-10-cbFrame.left, 2);
	assert( line != NULL );
	line->SetBorderWidth(1);
}

/******************************************************************************
 UpdateDisplay (virtual protected)

 ******************************************************************************/

void
CBSearchTextDialog::UpdateDisplay()
{
	JXSearchTextDialog::UpdateDisplay();

	if (itsFileList->GetRowCount() == 0)
		{
		itsMultifileCB->SetState(kJFalse);
		}

	if (itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked())
		{
		(GetFindBackButton())->Deactivate();
		(GetReplaceButton())->Deactivate();
		(GetReplaceFindFwdButton())->Deactivate();
		(GetReplaceFindBackButton())->Deactivate();
		(GetReplaceAllBackButton())->Deactivate();
		(GetReplaceAllInSelButton())->Deactivate();

		// These two buttons will already be deactivated if !HasSearchText().

		if (HasSearchText())
			{
			(GetFindFwdButton())->Activate();

			if ((CBGetDocumentManager())->WillEditTextFilesLocally())
				{
				(GetReplaceAllFwdButton())->Activate();
				}
			}
		}

	// We can't delete the link immediately if the process is cancelled
	// because there may be more messages to be broadcast.  Deleting it here
	// is safe because the stack doesn't pass through JMessageProtocol code.

	if (itsReplaceProcess == NULL && itsReplaceLink != NULL)
		{
		CBSearchDocument::DeleteReplaceLink(&itsReplaceLink);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSearchTextDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetFindFwdButton() && message.Is(JXButton::kPushed) &&
		(itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked()))
		{
		SearchFiles();
		}
	else if (sender == GetReplaceAllFwdButton() && message.Is(JXButton::kPushed) &&
			 (itsMultifileCB->IsChecked() || itsSearchDirCB->IsChecked()))
		{
		SearchFilesAndReplace();
		}

	else if (sender == itsFileList && message.Is(JXFileListTable::kProcessSelection))
		{
		OpenSelectedFiles();
		}
	else if (sender == itsFileList &&
			 (message.Is(JTable::kRowsInserted) ||
			  message.Is(JTable::kRowsRemoved)))
		{
		UpdateDisplay();
		}

	else if (sender == itsFileListMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileListMenu();
		}
	else if (sender == itsFileListMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileListMenu(selection->GetIndex());
		}

	else if (sender == itsSearchDirCB && message.Is(JXCheckbox::kPushed))
		{
		UpdateDisplay();
		}
	else if (sender == itsDirInput &&
			 (message.Is(JTextEditor::kTextChanged) ||
			  message.Is(JTextEditor::kTextSet)))
		{
		itsSearchDirCB->SetState(kJTrue);
		}
	else if (sender == itsMultifileCB && message.Is(JXCheckbox::kPushed))
		{
		const JBoolean on = itsMultifileCB->IsChecked();
		if (on && itsFileList->GetRowCount() == 0)
			{
			AddSearchFiles();
			}
		UpdateDisplay();
		}

	else if (sender == CBGetDocumentManager() &&
			 message.Is(CBDocumentManager::kProjectDocumentActivated))
		{
		UpdateBasePath();
		}
	else if (sender == itsChooseDirButton && message.Is(JXButton::kPushed))
		{
		itsDirInput->ChoosePath("");
		itsSearchDirCB->SetState(kJTrue);
		}

	else if (sender == itsDirHistory && message.Is(JXMenu::kItemSelected))
		{
		JBoolean recurse;
		itsDirInput->SetText(itsDirHistory->GetPath(message, &recurse));
		itsRecurseDirCB->SetState(recurse);
		itsSearchDirCB->SetState(kJTrue);
		}
	else if (sender == itsFileFilterHistory && message.Is(JXMenu::kItemSelected))
		{
		JBoolean invert;
		itsFileFilterInput->SetText(itsFileFilterHistory->GetFilter(message, &invert));
		itsInvertFileFilterCB->SetState(invert);
		itsSearchDirCB->SetState(kJTrue);
		}
	else if (sender == itsPathFilterHistory && message.Is(JXMenu::kItemSelected))
		{
		JBoolean invert;
		itsPathFilterInput->SetText(itsPathFilterHistory->GetFilter(message, &invert));
		itsSearchDirCB->SetState(kJTrue);
		}

	else if (sender == itsReplaceLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		MultifileReplaceAll();
		}
	else if (sender == itsReplaceLink && message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
		MultifileReplaceFinished();
		}

	else
		{
		JXSearchTextDialog::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileListMenu (private)

 ******************************************************************************/

void
CBSearchTextDialog::UpdateFileListMenu()
{
	itsFileListMenu->EnableItem(kShowMatchTextCmd);
	if (!itsOnlyListFilesFlag)
		{
		itsFileListMenu->CheckItem(kShowMatchTextCmd);
		}

	itsFileListMenu->EnableItem(kListFilesWithoutMatchCmd);
	if (itsListFilesWithoutMatchFlag)
		{
		itsFileListMenu->CheckItem(kListFilesWithoutMatchCmd);
		}

	itsFileListMenu->EnableItem(kAddFilesCmd);
	itsFileListMenu->EnableItem(kLoadFileSetCmd);

	if (itsFileList->GetRowCount() > 0)
		{
		itsFileListMenu->EnableItem(kRemoveAllCmd);
		itsFileListMenu->EnableItem(kSaveFileSetCmd);

		if (itsFileList->HasSelection())
			{
			itsFileListMenu->EnableItem(kRemoveSelCmd);
			}
		}
}

/******************************************************************************
 HandleFileListMenu (private)

 ******************************************************************************/

void
CBSearchTextDialog::HandleFileListMenu
	(
	const JIndex index
	)
{
	if (index == kShowMatchTextCmd)
		{
		itsOnlyListFilesFlag = !itsOnlyListFilesFlag;
		}
	else if (index == kListFilesWithoutMatchCmd)
		{
		itsListFilesWithoutMatchFlag = !itsListFilesWithoutMatchFlag;
		}

	else if (index == kAddFilesCmd)
		{
		AddSearchFiles();
		}

	else if (index == kRemoveSelCmd)
		{
		itsFileList->RemoveSelectedFiles();
		}
	else if (index == kRemoveAllCmd)
		{
		itsFileList->RemoveAllFiles();
		}

	else if (index == kLoadFileSetCmd)
		{
		LoadFileSet();
		}
	else if (index == kSaveFileSetCmd)
		{
		SaveFileSet();
		}
}

/******************************************************************************
 SearchFiles (private)

	We use GetSearchParameters() to check that everything is ok.

 ******************************************************************************/

void
CBSearchTextDialog::SearchFiles()
	const
{
	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll),
					   nameList(JPtrArrayT::kDeleteAll);

	if (GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex) &&
		BuildSearchFileList(&fileList, &nameList))
		{
		const JError err =
			CBSearchDocument::Create(fileList, nameList,
									 searchStr, itsOnlyListFilesFlag,
									 itsListFilesWithoutMatchFlag);
		err.ReportIfError();
		}
}

/******************************************************************************
 SearchFilesAndReplace (private)

 ******************************************************************************/

void
CBSearchTextDialog::SearchFilesAndReplace()
{
	assert( itsReplaceProcess == NULL && itsReplacePG == NULL );
	CBSearchDocument::DeleteReplaceLink(&itsReplaceLink);

	JString searchStr, replaceStr;
	JBoolean searchIsRegex, caseSensitive, entireWord, wrapSearch;
	JBoolean replaceIsRegex, preserveCase;
	JRegex* regex;

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll),
					   nameList(JPtrArrayT::kDeleteAll);

	if (GetSearchParameters(
			&searchStr, &searchIsRegex, &caseSensitive, &entireWord, &wrapSearch,
			&replaceStr, &replaceIsRegex, &preserveCase,
			&regex) &&
		BuildSearchFileList(&fileList, &nameList))
		{
		const JError err =
			CBSearchDocument::GetFileListForReplace(fileList, nameList,
													&itsReplaceProcess, &itsReplaceLink);
		if (err.OK())
			{
			ListenTo(itsReplaceLink);

			// We background the pg so the main event loop runs and checks our link,
			// but we have to disable everything so the user can't edit files while
			// ReplaceAll is running.

			(JXGetApplication())->Suspend();	// do this first so pg window is active

			itsReplacePG = new JXStandAlonePG;
			assert( itsReplacePG != NULL );
			itsReplacePG->RaiseWhenUpdate();
			itsReplacePG->VariableLengthProcessBeginning("Searching files...", kJTrue, kJTrue);
			}
		else
			{
			err.ReportIfError();
			}
		}
}

/******************************************************************************
 MultifileReplaceAll (private)

 ******************************************************************************/

void
CBSearchTextDialog::MultifileReplaceAll()
{
	JString msg, fileName;
	JBoolean ok = itsReplaceLink->GetNextMessage(&msg);
	assert( ok );
	const std::string s(msg.GetCString(), msg.GetLength());
	std::istringstream input(s);
	input >> fileName;

	JBoolean keepGoing = kJTrue;

	CBTextDocument* doc;
	if ((CBGetDocumentManager())->OpenTextDocument(fileName, 0, &doc))
		{
		(doc->GetWindow())->Update();

		keepGoing = itsReplacePG->IncrementProgress();

		CBTextEditor* te = doc->GetTextEditor();
		te->SetCaretLocation(1);
		te->ReplaceAllForward();
		}
	else
		{
		keepGoing = itsReplacePG->IncrementProgress();
		}

	if (!keepGoing)
		{
		itsReplaceProcess->Kill();
		MultifileReplaceFinished();
		}
}

/******************************************************************************
 MultifileReplaceFinished (private)

 ******************************************************************************/

void
CBSearchTextDialog::MultifileReplaceFinished()
{
	delete itsReplaceProcess;
	itsReplaceProcess = NULL;

	StopListening(itsReplaceLink);
	// delete itsReplaceLink in idle task

	itsReplacePG->ProcessFinished();
	delete itsReplacePG;
	itsReplacePG = NULL;

	(JXGetApplication())->Resume();
}

/******************************************************************************
 BuildSearchFileList (private)

	The client need not call DeleteAll() because the clean up action is set
	for each array.

 ******************************************************************************/

JBoolean
CBSearchTextDialog::BuildSearchFileList
	(
	JPtrArray<JString>* fileList,
	JPtrArray<JString>* nameList
	)
	const
{
	(JXGetApplication())->DisplayBusyCursor();

	fileList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	fileList->SetCompareFunction(JCompareStringsCaseSensitive);

	nameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	nameList->SetCompareFunction(JCompareStringsCaseSensitive);

	if (itsMultifileCB->IsChecked())
		{
		const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();

		const JSize count = fullNameList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			SaveFileForSearch(*(fullNameList.NthElement(i)), fileList, nameList);
			}
		}

	JBoolean ok = kJTrue;

	JString path, fileFilter, pathFilter;
	if (GetSearchDirectory(&path, &fileFilter, &pathFilter))
		{
		JRegex* fileRegex = NULL;
		JString regexStr;
		if (JDirInfo::BuildRegexFromWildcardFilter(fileFilter, &regexStr))
			{
			fileRegex = new JRegex(regexStr);
			assert( fileRegex != NULL );
			fileRegex->SetCaseSensitive(kJFalse);
			}

		JRegex* pathRegex = NULL;
		if (JDirInfo::BuildRegexFromWildcardFilter(pathFilter, &regexStr))
			{
			pathRegex = new JRegex(regexStr);
			assert( pathRegex != NULL );
			pathRegex->SetCaseSensitive(kJFalse);
			}

		JLatentPG pg(100);
		pg.VariableLengthProcessBeginning("Collecting files...", kJTrue, kJFalse);
		ok = SearchDirectory(path, fileRegex, pathRegex, fileList, nameList, pg);
		pg.ProcessFinished();

		delete fileRegex;
		delete pathRegex;
		}

	return JI2B( ok && !fileList->IsEmpty() );
}

/******************************************************************************
 SearchDirectory (private)

 ******************************************************************************/

JBoolean
CBSearchTextDialog::SearchDirectory
	(
	const JString&		path,
	const JRegex*		fileRegex,
	const JRegex*		pathRegex,
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList,
	JProgressDisplay&	pg
	)
	const
{
	JDirInfo* info;
	if (!JDirInfo::Create(path, &info))
		{
		return kJTrue;	// user didn't cancel
		}
	info->SetWildcardFilter(const_cast<JRegex*>(fileRegex), kJFalse,
							itsInvertFileFilterCB->IsChecked());

	JBoolean keepGoing = kJTrue;

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);
		if (entry.IsFile())
			{
			if (!pg.IncrementProgress())
				{
				keepGoing = kJFalse;
				break;
				}
			SaveFileForSearch(entry.GetFullName(), fileList, nameList);
			}
		else if (itsRecurseDirCB->IsChecked() &&
				 entry.IsDirectory() && !entry.IsLink() &&
				 !JIsVCSDirectory(entry.GetName()))
			{
			JBoolean match = kJTrue;
			if (pathRegex != NULL)
				{
				match = ! pathRegex->Match(entry.GetName());
				}

			if (match &&
				!SearchDirectory(entry.GetFullName(), fileRegex, pathRegex,
				 				 fileList, nameList, pg))
				{
				keepGoing = kJFalse;
				break;
				}
			}
		}

	delete info;
	return keepGoing;
}

/******************************************************************************
 SaveFileForSearch (private)

	Save the file in /tmp if necessary so the current text will be searched.

 ******************************************************************************/

void
CBSearchTextDialog::SaveFileForSearch
	(
	const JString&		fullName,
	JPtrArray<JString>*	fileList,
	JPtrArray<JString>*	nameList
	)
	const
{
	JBoolean exists;
	const JIndex index =
		nameList->GetInsertionSortIndex(const_cast<JString*>(&fullName), &exists);
	if (!exists)
		{
		JString* file = new JString(fullName);
		assert( file != NULL );

		JXFileDocument* doc;
		if ((CBGetDocumentManager())->FileDocumentIsOpen(*file, &doc) &&
			doc->NeedsSave())
			{
			CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
			if (textDoc != NULL &&
				(JCreateTempFile(file)).OK())
				{
				ofstream output(*file);
				((textDoc->GetTextEditor())->GetText()).Print(output);
				}
			}

		if (!file->IsEmpty())
			{
			JString* name = new JString(fullName);
			assert( name != NULL );
			nameList->InsertAtIndex(index, name);

			fileList->InsertAtIndex(index, file);
			}
		else
			{
			delete file;
			}
		}
}

/******************************************************************************
 UpdateBasePath (private)

 ******************************************************************************/

void
CBSearchTextDialog::UpdateBasePath()
{
	CBProjectDocument* doc;
	if ((CBGetDocumentManager())->GetActiveProjectDocument(&doc))
		{
		itsDirInput->SetBasePath(doc->GetFilePath());
		}
	else
		{
		itsDirInput->ClearBasePath();
		}
}

/******************************************************************************
 GetSearchDirectory (private)

 ******************************************************************************/

JBoolean
CBSearchTextDialog::GetSearchDirectory
	(
	JString* path,
	JString* fileFilter,
	JString* pathFilter
	)
	const
{
	if (!itsSearchDirCB->IsChecked())
		{
		path->Clear();
		fileFilter->Clear();
		pathFilter->Clear();
		return kJTrue;
		}

	*fileFilter = itsFileFilterInput->GetText();
	*pathFilter = itsPathFilterInput->GetText();

	itsDirInput->ShouldAllowInvalidPath(kJFalse);
	const JBoolean ok = itsDirInput->GetPath(path);
	if (ok)
		{
		itsDirHistory->AddPath(itsDirInput->GetText(), itsRecurseDirCB->IsChecked());
		itsFileFilterHistory->AddFilter(itsFileFilterInput->GetText(), itsInvertFileFilterCB->IsChecked());
		itsPathFilterHistory->AddFilter(itsPathFilterInput->GetText(), kJFalse);
		}
	else
		{
		itsDirInput->Focus();
		itsDirInput->InputValid();	// report error
		}
	itsDirInput->ShouldAllowInvalidPath(kJTrue);

	return ok;
}

/******************************************************************************
 LoadFileSet (private)

 ******************************************************************************/

void
CBSearchTextDialog::LoadFileSet()
{
	JString fullName;
	if (itsCSF->ChooseFile("", NULL, &fullName))
		{
		if (itsCSF->ReplaceExisting())
			{
			itsFileList->RemoveAllFiles();
			}

		ifstream input(fullName);
		JString file;
		while (!input.eof() && !input.fail())
			{
			file = JReadLine(input);
			if (JFileReadable(file))
				{
				itsFileList->AddFile(file);
				}
			}
		}
}

/******************************************************************************
 SaveFileSet (private)

 ******************************************************************************/

void
CBSearchTextDialog::SaveFileSet()
	const
{
	JString newName;
	if ((JGetChooseSaveFile())->SaveFile("Save file set as:", "",
										 itsFileSetName, &newName))
		{
		itsFileSetName = newName;

		ofstream output(itsFileSetName);
		const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();
		const JSize fileCount = fullNameList.GetElementCount();
		for (JIndex i=1; i<=fileCount; i++)
			{
			(fullNameList.NthElement(i))->Print(output);
			output << '\n';
			}
		}
}

/******************************************************************************
 AddSearchFiles (private)

 ******************************************************************************/

void
CBSearchTextDialog::AddSearchFiles()
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if ((JXGetChooseSaveFile())->ChooseFiles("", NULL, &fileList))
		{
		const JSize count = fileList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsFileList->AddFile(*(fileList.NthElement(i)));
			}
		}
}

/******************************************************************************
 AddFileToSearch

 ******************************************************************************/

void
CBSearchTextDialog::AddFileToSearch
	(
	const JCharacter* fileName
	)
	const
{
	itsFileList->AddFile(fileName);
}

/******************************************************************************
 ShouldSearchFiles

 ******************************************************************************/

void
CBSearchTextDialog::ShouldSearchFiles
	(
	const JBoolean search
	)
{
	itsMultifileCB->SetState(search);
}

/******************************************************************************
 ClearFileList

 ******************************************************************************/

void
CBSearchTextDialog::ClearFileList()
{
	itsFileList->RemoveAllFiles();
}

/******************************************************************************
 OpenSelectedFiles (private)

 ******************************************************************************/

void
CBSearchTextDialog::OpenSelectedFiles()
	const
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if (itsFileList->GetSelection(&fileList))
		{
		(CBGetDocumentManager())->OpenSomething(fileList);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBSearchTextDialog::ReadPrefs
	(
	istream& input
	)
{
	input >> ws;
	const JCharacter versChar = input.peek();

	JFileVersion vers;
	if (versChar <= '6')
		{
		// leave version number in stream
		vers = versChar - '0';
		}
	else
		{
		input >> vers;
		}

	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXSearchTextDialog::ReadSetup(input);

	// file list

	if (vers >= 4)
		{
		input >> itsOnlyListFilesFlag;
		if (vers >= 5)
			{
			input >> itsListFilesWithoutMatchFlag >> itsFileSetName;
			}

		JSize fileCount;
		input >> fileCount;

		JString fullName;
		for (JIndex i=1; i<=fileCount; i++)
			{
			input >> fullName;
			if (JFileReadable(fullName))
				{
				itsFileList->AddFile(fullName);
				}
			}
		}

	// directory

	if (vers >= 71)
		{
		JBoolean b;
		input >> b;
		itsRecurseDirCB->SetState(b);

		if (vers >= 72)
			{
			input >> b;
			itsInvertFileFilterCB->SetState(b);
			}

		JString s;
		input >> s;
		itsDirInput->SetText(s);
		input >> s;
		itsFileFilterInput->SetText(s);

		itsDirHistory->ReadSetup(input);
		itsFileFilterHistory->ReadSetup(input);

		if (vers >= 73)
			{
			input >> s;
			itsPathFilterInput->SetText(s);

			input >> b;

			itsPathFilterHistory->ReadSetup(input);
			}
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBSearchTextDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	JXSearchTextDialog::WriteSetup(output);

	// file list

	output << ' ' << itsOnlyListFilesFlag;
	output << ' ' << itsListFilesWithoutMatchFlag;
	output << ' ' << itsFileSetName;

	const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();

	const JSize fileCount = fullNameList.GetElementCount();
	output << ' ' << fileCount;

	for (JIndex i=1; i<=fileCount; i++)
		{
		output << ' ' << *(fullNameList.NthElement(i));
		}

	// directory

	output << ' ' << itsRecurseDirCB->IsChecked();
	output << ' ' << itsInvertFileFilterCB->IsChecked();
	output << ' ' << itsDirInput->GetText();
	output << ' ' << itsFileFilterInput->GetText();

	output << ' ';
	itsDirHistory->WriteSetup(output);

	output << ' ';
	itsFileFilterHistory->WriteSetup(output);

	output << ' ' << itsPathFilterInput->GetText();
	output << ' ' << kJFalse;	// placeholder for invert, if we ever find a use
	output << ' ';
	itsPathFilterHistory->WriteSetup(output);

	output << ' ';
}
