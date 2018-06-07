/******************************************************************************
 CBSearchTextDialog.cpp

	BASE CLASS = JXSearchTextDialog, JPrefObject

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "CBSearchTextDialog.h"
#include "CBSearchDocument.h"
#include "CBTextEditor.h"
#include "CBSearchPathHistoryMenu.h"
#include "CBSearchFilterHistoryMenu.h"
#include "CBListCSF.h"
#include "CBProjectDocument.h"
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
#include <JXChooseSaveFile.h>
#include <JXFontManager.h>
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
	CBSearchTextDialog* dlog = jnew CBSearchTextDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow();
	dlog->JXSearchTextDialogX();
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBSearchTextDialog::CBSearchTextDialog()
	:
	JXSearchTextDialog("CBSearchTextHelp"),
	JPrefObject(CBGetPrefsManager(), kCBSearchTextID),
	itsFileSetName("Untitled")
{
	itsCSF = jnew CBListCSF(kReplaceFileListStr, kAppendToFileListStr);
	assert( itsCSF != nullptr );

	itsOnlyListFilesFlag         = kJFalse;
	itsListFilesWithoutMatchFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchTextDialog::~CBSearchTextDialog()
{
	// prefs written by CBDeleteGlobals()

	jdelete itsCSF;
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

	JXWindow* window = jnew JXWindow(this, 450,470, "");
	assert( window != nullptr );

	JXStaticText* replaceLabel =
		jnew JXStaticText(JGetString("replaceLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,101, 220,20);
	assert( replaceLabel != nullptr );
	replaceLabel->SetToLabel();

	JXStaticText* searchLabel =
		jnew JXStaticText(JGetString("searchLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,31, 220,20);
	assert( searchLabel != nullptr );
	searchLabel->SetToLabel();

	JXInputField* searchInput =
		jnew JXInputField(kJTrue, kJFalse, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,50, 220,40);
	assert( searchInput != nullptr );

	JXInputField* replaceInput =
		jnew JXInputField(kJTrue, kJFalse, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,120, 220,40);
	assert( replaceInput != nullptr );

	JXTextButton* closeButton =
		jnew JXTextButton(JGetString("closeButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,440, 80,20);
	assert( closeButton != nullptr );

	JXTextCheckbox* ignoreCaseCB =
		jnew JXTextCheckbox(JGetString("ignoreCaseCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 130,20);
	assert( ignoreCaseCB != nullptr );

	JXTextCheckbox* wrapSearchCB =
		jnew JXTextCheckbox(JGetString("wrapSearchCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,240, 130,20);
	assert( wrapSearchCB != nullptr );

	JXTextButton* replaceButton =
		jnew JXTextButton(JGetString("replaceButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,50, 140,20);
	assert( replaceButton != nullptr );

	JXTextCheckbox* entireWordCB =
		jnew JXTextCheckbox(JGetString("entireWordCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 130,20);
	assert( entireWordCB != nullptr );

	JXStringHistoryMenu* prevReplaceMenu =
		jnew JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,130, 30,20);
	assert( prevReplaceMenu != nullptr );

	JXStringHistoryMenu* prevSearchMenu =
		jnew JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,60, 30,20);
	assert( prevSearchMenu != nullptr );

	JXTextButton* helpButton =
		jnew JXTextButton(JGetString("helpButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,440, 80,20);
	assert( helpButton != nullptr );
	helpButton->SetShortcuts(JGetString("helpButton::CBSearchTextDialog::shortcuts::JXLayout"));

	JXTextCheckbox* searchIsRegexCB =
		jnew JXTextCheckbox(JGetString("searchIsRegexCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,180, 150,20);
	assert( searchIsRegexCB != nullptr );

	JXTextCheckbox* replaceIsRegexCB =
		jnew JXTextCheckbox(JGetString("replaceIsRegexCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,180, 110,20);
	assert( replaceIsRegexCB != nullptr );

	JXTextCheckbox* singleLineCB =
		jnew JXTextCheckbox(JGetString("singleLineCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,210, 150,20);
	assert( singleLineCB != nullptr );

	JXTextCheckbox* preserveCaseCB =
		jnew JXTextCheckbox(JGetString("preserveCaseCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,210, 110,20);
	assert( preserveCaseCB != nullptr );

	JXTextButton* qRefButton =
		jnew JXTextButton(JGetString("qRefButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,440, 80,20);
	assert( qRefButton != nullptr );

	itsMultifileCB =
		jnew JXTextCheckbox(JGetString("itsMultifileCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,360, 100,20);
	assert( itsMultifileCB != nullptr );
	itsMultifileCB->SetShortcuts(JGetString("itsMultifileCB::CBSearchTextDialog::shortcuts::JXLayout"));

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,360, 300,60);
	assert( scrollbarSet != nullptr );

	JXStaticText* findLabel =
		jnew JXStaticText(JGetString("findLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,20, 100,20);
	assert( findLabel != nullptr );
	findLabel->SetToLabel(kJTrue);

	JXSearchTextButton* findBackButton =
		jnew JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 20,20);
	assert( findBackButton != nullptr );

	JXSearchTextButton* findFwdButton =
		jnew JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,20, 20,20);
	assert( findFwdButton != nullptr );

	JXStaticText* replaceFindLabel =
		jnew JXStaticText(JGetString("replaceFindLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,80, 100,20);
	assert( replaceFindLabel != nullptr );
	replaceFindLabel->SetToLabel(kJTrue);

	JXSearchTextButton* replaceFindBackButton =
		jnew JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 20,20);
	assert( replaceFindBackButton != nullptr );

	JXSearchTextButton* replaceFindFwdButton =
		jnew JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,80, 20,20);
	assert( replaceFindFwdButton != nullptr );

	JXStaticText* replaceAllLabel =
		jnew JXStaticText(JGetString("replaceAllLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,110, 100,20);
	assert( replaceAllLabel != nullptr );
	replaceAllLabel->SetToLabel(kJTrue);

	JXSearchTextButton* replaceAllBackButton =
		jnew JXSearchTextButton(kJFalse, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,110, 20,20);
	assert( replaceAllBackButton != nullptr );

	JXSearchTextButton* replaceAllFwdButton =
		jnew JXSearchTextButton(kJTrue, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,110, 20,20);
	assert( replaceAllFwdButton != nullptr );

	JXTextButton* replaceAllInSelButton =
		jnew JXTextButton(JGetString("replaceAllInSelButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,140, 140,20);
	assert( replaceAllInSelButton != nullptr );

	JXTextCheckbox* stayOpenCB =
		jnew JXTextCheckbox(JGetString("stayOpenCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,240, 150,20);
	assert( stayOpenCB != nullptr );

	JXTextCheckbox* retainFocusCB =
		jnew JXTextCheckbox(JGetString("retainFocusCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,240, 110,20);
	assert( retainFocusCB != nullptr );

	itsFileListMenu =
		jnew JXTextMenu(JGetString("itsFileListMenu::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,390, 90,30);
	assert( itsFileListMenu != nullptr );

	itsSearchDirCB =
		jnew JXTextCheckbox(JGetString("itsSearchDirCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,270, 130,20);
	assert( itsSearchDirCB != nullptr );
	itsSearchDirCB->SetShortcuts(JGetString("itsSearchDirCB::CBSearchTextDialog::shortcuts::JXLayout"));

	itsDirInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,270, 190,20);
	assert( itsDirInput != nullptr );

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,300, 100,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	itsDirHistory =
		jnew CBSearchPathHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,270, 30,20);
	assert( itsDirHistory != nullptr );

	itsFileFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,300, 190,20);
	assert( itsFileFilterInput != nullptr );

	itsFileFilterHistory =
		jnew CBSearchFilterHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,300, 30,20);
	assert( itsFileFilterHistory != nullptr );

	itsRecurseDirCB =
		jnew JXTextCheckbox(JGetString("itsRecurseDirCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,330, 215,20);
	assert( itsRecurseDirCB != nullptr );

	itsChooseDirButton =
		jnew JXTextButton(JGetString("itsChooseDirButton::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,270, 60,20);
	assert( itsChooseDirButton != nullptr );

	itsInvertFileFilterCB =
		jnew JXTextCheckbox(JGetString("itsInvertFileFilterCB::CBSearchTextDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,300, 60,20);
	assert( itsInvertFileFilterCB != nullptr );

	itsPathFilterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 255,330, 155,20);
	assert( itsPathFilterInput != nullptr );

	itsPathFilterHistory =
		jnew CBSearchFilterHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,330, 30,20);
	assert( itsPathFilterHistory != nullptr );

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
		jnew JXFileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);
	assert( itsFileList != nullptr );
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

	const JFont& font = window->GetFontManager()->GetDefaultMonospaceFont();

	itsDirInput->ShouldAllowInvalidPath();
	itsDirHistory->SetDefaultFont(font, kJTrue);
	itsFileFilterInput->SetDefaultFont(font);
	itsFileFilterHistory->SetDefaultFont(font, kJTrue);
	itsPathFilterInput->SetDefaultFont(font);
	itsPathFilterHistory->SetDefaultFont(font, kJTrue);
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
		jnew JXDocumentMenu("", window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

	// decor

	const JRect wFrame  = window->GetFrame();
	const JRect cbFrame = itsSearchDirCB->GetFrame();

	JXDownRect* line =
		jnew JXDownRect(window, JXWidget::kFixedLeft, JXWidget::kFixedTop,
					   cbFrame.left, cbFrame.top-6,
					   wFrame.right-10-cbFrame.left, 2);
	assert( line != nullptr );
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
		GetFindBackButton()->Deactivate();
		GetReplaceButton()->Deactivate();
		GetReplaceFindFwdButton()->Deactivate();
		GetReplaceFindBackButton()->Deactivate();
		GetReplaceAllBackButton()->Deactivate();
		GetReplaceAllInSelButton()->Deactivate();

		// These two buttons will already be deactivated if !HasSearchText().

		if (HasSearchText())
			{
			GetFindFwdButton()->Activate();

			if ((CBGetDocumentManager())->WillEditTextFilesLocally())
				{
				GetReplaceAllFwdButton()->Activate();
				}
			}
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
		assert( selection != nullptr );
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
									 searchStr, replaceStr);
		err.ReportIfError();
		}
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
			SaveFileForSearch(*(fullNameList.GetElement(i)), fileList, nameList);
			}
		}

	JBoolean ok = kJTrue;

	JString path, fileFilter, pathFilter;
	if (GetSearchDirectory(&path, &fileFilter, &pathFilter))
		{
		JRegex* fileRegex = nullptr;
		JString regexStr;
		if (JDirInfo::BuildRegexFromWildcardFilter(fileFilter, &regexStr))
			{
			fileRegex = jnew JRegex(regexStr);
			assert( fileRegex != nullptr );
			fileRegex->SetCaseSensitive(kJFalse);
			}

		JRegex* pathRegex = nullptr;
		if (JDirInfo::BuildRegexFromWildcardFilter(pathFilter, &regexStr))
			{
			pathRegex = jnew JRegex(regexStr);
			assert( pathRegex != nullptr );
			pathRegex->SetCaseSensitive(kJFalse);
			}

		JLatentPG pg(100);
		pg.VariableLengthProcessBeginning("Collecting files...", kJTrue, kJFalse);
		ok = SearchDirectory(path, fileRegex, pathRegex, fileList, nameList, pg);
		pg.ProcessFinished();

		jdelete fileRegex;
		jdelete pathRegex;
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
			if (pathRegex != nullptr)
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

	jdelete info;
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
		JString* file = jnew JString(fullName);
		assert( file != nullptr );

		JXFileDocument* doc;
		if ((CBGetDocumentManager())->FileDocumentIsOpen(*file, &doc) &&
			doc->NeedsSave())
			{
			CBTextDocument* textDoc = dynamic_cast<CBTextDocument*>(doc);
			if (textDoc != nullptr &&
				(JCreateTempFile(file)).OK())
				{
				std::ofstream output(*file);
				((textDoc->GetTextEditor())->GetText()).Print(output);
				}
			}

		if (!file->IsEmpty())
			{
			JString* name = jnew JString(fullName);
			assert( name != nullptr );
			nameList->InsertAtIndex(index, name);

			fileList->InsertAtIndex(index, file);
			}
		else
			{
			jdelete file;
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
	if (itsCSF->ChooseFile("", nullptr, &fullName))
		{
		if (itsCSF->ReplaceExisting())
			{
			itsFileList->RemoveAllFiles();
			}

		std::ifstream input(fullName);
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

		std::ofstream output(itsFileSetName);
		const JPtrArray<JString>& fullNameList = itsFileList->GetFullNameList();
		const JSize fileCount = fullNameList.GetElementCount();
		for (JIndex i=1; i<=fileCount; i++)
			{
			(fullNameList.GetElement(i))->Print(output);
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
	if ((JXGetChooseSaveFile())->ChooseFiles("", nullptr, &fileList))
		{
		const JSize count = fileList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsFileList->AddFile(*(fileList.GetElement(i)));
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
	std::istream& input
	)
{
	input >> std::ws;
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
	std::ostream& output
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
		output << ' ' << *(fullNameList.GetElement(i));
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
