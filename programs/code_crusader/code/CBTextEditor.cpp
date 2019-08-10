/******************************************************************************
 CBTextEditor.cpp

	1) Search & replace
	2) Search for function name, class name, man page when a word is Meta-double-clicked
	3) Search for file or view URL when it is Ctrl-double-clicked
	4) Open complement file (source<->header) via Ctrl-Tab
	5) View HTML file in browser via Ctrl-Tab
	6) Context-sensitive hilighting
	7) Keyword completion
	8) Character actions (e.g. semicolor triggers newline)
	9) Macros (e.g. "<a\t" triggers " href=\"\">\l\l")

	BASE CLASS = JXTEBase

	Copyright (C) 1996-2001 by John Lindal.

 ******************************************************************************/

#include "CBTextEditor.h"
#include "CBTextDocument.h"
#include "CBSearchTextDialog.h"
#include "CBRunTEScriptDialog.h"
#include "CBTELineIndexInput.h"
#include "CBTEColIndexInput.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBManPageDocument.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBStylerBase.h"
#include "CBStringCompleter.h"
#include "CBCharActionManager.h"
#include "CBMacroManager.h"
#include "CBFnMenuUpdater.h"
#include "CBFunctionMenu.h"
#include "CBCommandMenu.h"
#include "CBTEScriptMenu.h"
#include "CBEmulator.h"
#include "CBPTPrinter.h"
#include "CBPSPrinter.h"
#include "cbGlobals.h"
#include "cbmUtil.h"
#include "cbActionDefs.h"

#include <JXDisplay.h>
#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXWebBrowser.h>
#include <JXSharedPrefsManager.h>
#include <JXStringCompletionMenu.h>
#include <JXWindowPainter.h>
#include <JXFontManager.h>
#include <JXColorManager.h>

#include <JRegex.h>
#include <JStack.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jASCIIConstants.h>
#include <X11/keysym.h>
#include <jAssert.h>

const JSize kDefTabCharCount          = 4;
const JFloat kBalanceWhileTypingDelay = 0.5;

// setup information

const JFileVersion kCurrentSetupVersion = 11;
const JCharacter kSetupDataEndDelimiter = '\1';

	// version 11:  added right margin info
	// version 10:  PartialWordModifier moved to JX shared prefs.
	// version  9:  added tab to spaces flag
	// version  8:  added partial word modifier
	// version  7:  added itsSmartTabFlag
	// version  6:  removed itsNewLineAfterSemiFlag (generalized to CharAction)
	// version  5:  added itsNewLineAfterSemiFlag
	// version  4:  added CRM line width, itsBalanceWhileTypingFlag,
	//				itsScrollToBalanceFlag, itsBeepWhenTypeUnbalancedFlag
	// version  3:  removed font
	// version  2:  dnd, Meta-left-arrow, font, undo depth
	// version  1:  merged in parameters from JXCodeEditor

// Edit menu additions -- can't use AppendMenuItems() because of bar in NMShortcut

#include <jx_run_script.xpm>

static const JCharacter* kExecScriptStr           = "Run script...";
static const JCharacter* kExecScriptSelStr        = "Run script on selection...";
static const JCharacter* kExecScriptNMShortcutStr = "Meta-|";
static const JCharacter* kExecScriptAction        = "ExecScript::CBTextEditor";

static const JCharacter* kExecScriptSubmenuStr = "Scripts";

// Search menu

#include "jcc_balance_braces.xpm"
#include "jcc_view_man_page.xpm"

static const JCharacter* kSearchMenuStr =
	"    Balance closest grouping symbols   %k Meta-B               %i" kCBBalanceGroupingSymbolAction
	"%l| Go to line...                      %k Meta-comma           %i" kCBGoToLineAction
	"  | Go to column...                    %k Meta-<               %i" kCBGoToColumnAction
	"  | Place bookmark                     %k Meta-Shift-B         %i" kCBPlaceBookmarkAction
	"%l| Open selection as file name or URL %k Ctrl-left-dbl-click  %i" kCBOpenSelectionAsFileAction
	"  | Find file...                       %k Meta-D               %i" kCBFindFileAction
	"%l| Find selection as symbol           %k Meta-dbl-click, F12  %i" kCBFindSelectionInProjectAction
	"  | Find selection (no context)        %k Meta-Shift-dbl-click %i" kCBFindSelInProjNoContextAction
	"  | Find selection (man page only)     %k Meta-Shift-I         %i" kCBFindSelectionInManPageAction
	"  | Look up man page...                %k Meta-I               %i" kCBOpenManPageAction
	"%l| Previous search/compile result     %k Ctrl-minus           %i" kCBOpenPrevListItem
	"  | Next search/compile result         %k Ctrl-plus            %i" kCBOpenNextListItem;

static const JCharacter* kFindSelAsSymText   = "Find selection as symbol";
static const JCharacter* kFindSelAsSymInText = "Find selection as symbol in project ";

// offsets from itsFirstSearchMenuItem

enum
{
	kBalanceCmd = 0,
	kGoToLineCmd, kGoToColCmd, kPlaceBookmarkCmd,
	kOpenSelectionAsFileCmd, kFindSourceFileCmd,
	kFindSelectionAsSymbolCmd, kFindSelectionAsSymbolNoContextCmd,
	kFindSelectionInManPageCmd, kViewManPageCmd,
	kOpenPrevListItemCmd, kOpenNextListItemCmd
};

// Context menu

static const JCharacter* kContextMenuStr =
	"    Cut                              %k Meta-X."
	"  | Copy                             %k Meta-C."
	"  | Paste                            %k Meta-V."
	"%l| Shift left                       %k Meta-[."
	"  | Shift right                      %k Meta-]."
	"%l| Complete                         %k Tab or Ctrl-space."
	"  | Run macro                        %k Tab or Ctrl-space."
	"  | Scripts"
	"%l| Find backwards                   %k Meta-Shift-H."
	"  | Find forward                     %k Meta-H."
	"%l| Find as symbol                   %k Meta-dbl-click."
	"  | Find as symbol (no context)      %k Meta-Shift-dbl-click."
	"  | Find as symbol (man page only)   %k Meta-Shift-I."
	"%l| Open as file name or URL         %k Ctrl-left-dbl-click."
	"  | Show in file manager             %k Dbl-click icon in footer"
	"%l| Balance closest grouping symbols %k Meta-B."
	"  | Place bookmark                   %k Meta-Shift-B.";

enum
{
	kContextCutCmd = 1, kContextCopyCmd, kContextPasteCmd,
	kContextShiftLeftCmd, kContextShiftRightCmd,
	kContextCompleteCmd, kContextMacroCmd, kContextScriptSubmenuIndex,
	kContextFindSelBackCmd, kContextFindSelFwdCmd,
	kContextFindSymbolCmd, kContextFindSymbolNoContextCmd, kContextFindSymbolManPageCmd,
	kContextOpenSelAsFileCmd, kShowInFileMgrCmd,
	kContextBalanceCmd, kContextPlaceBookmarkCmd
};

// string ID's

static const JCharacter* kEmptyScriptMenuID = "EmptyScriptMenu::CBTextEditor";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTextEditor::CBTextEditor
	(
	CBTextDocument*		document,
	const JCharacter*	fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, kJTrue, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h),
	itsLastModifiers(GetDisplay())
{
	itsDoc = document;
	ListenTo(itsDoc);

	UpdateWritable(fileName);

	itsLineInput = lineInput;
	itsLineInput->SetTE(this, colInput);

	itsColInput = colInput;
	itsColInput->SetTE(this);

	itsTokenStartList = JTEStyler::NewTokenStartList();

	itsBalanceWhileTypingFlag     = kJTrue;
	itsScrollToBalanceFlag        = kJTrue;
	itsBeepWhenTypeUnbalancedFlag = kJTrue;

	itsSmartTabFlag = kJTrue;

	// create edit menu

	JXTextMenu* editMenu = AppendEditMenu(menuBar, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse);
	const JSize editCount = editMenu->GetItemCount();
	editMenu->ShowSeparatorAfter(editCount);

	itsExecScriptCmdIndex = editCount + 1;
	editMenu->AppendItem(kExecScriptStr, JXMenu::kPlainType, nullptr,
						 kExecScriptNMShortcutStr, kExecScriptAction);
	editMenu->SetItemImage(itsExecScriptCmdIndex, jx_run_script);

	editMenu->AppendItem(kExecScriptSubmenuStr);
	CreateScriptMenu(editMenu, editCount + 2);

	// create search menu

	JXTextMenu* searchMenu = AppendSearchReplaceMenu(menuBar);
	itsFirstSearchMenuItem = searchMenu->GetItemCount() + 1;
	searchMenu->ShowSeparatorAfter(itsFirstSearchMenuItem-1);
	searchMenu->AppendMenuItems(kSearchMenuStr, "CBTextEditor");
	ListenTo(searchMenu);

	searchMenu->SetItemImage(itsFirstSearchMenuItem + kBalanceCmd,     jcc_balance_braces);
	searchMenu->SetItemImage(itsFirstSearchMenuItem + kViewManPageCmd, jcc_view_man_page);

	// create menus when needed

	itsFnMenu         = nullptr;
	itsCompletionMenu = nullptr;
	itsContextMenu    = nullptr;

	// font

	CBPrefsManager* prefsMgr = CBGetPrefsManager();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	SetFont(fontName, fontSize, kDefTabCharCount);

	// colors

	SetDefaultFontStyle(prefsMgr->GetColor(CBPrefsManager::kTextColorIndex));

	SetBackColor(prefsMgr->GetColor(CBPrefsManager::kBackColorIndex));
	SetFocusColor(GetBackColor());

	SetCaretColor(prefsMgr->GetColor(CBPrefsManager::kCaretColorIndex));
	SetSelectionColor(prefsMgr->GetColor(CBPrefsManager::kSelColorIndex));
	SetSelectionOutlineColor(prefsMgr->GetColor(CBPrefsManager::kSelLineColorIndex));

	// right margin

	itsDrawRightMarginFlag = kJFalse;
	itsRightMarginWidth    = 80;
	itsRightMarginColor    = prefsMgr->GetColor(CBPrefsManager::kRightMarginColorIndex);
	SetWhitespaceColor(itsRightMarginColor);

	// misc setup

	ShouldBroadcastCaretLocationChanged(kJTrue);
	UpdateTabHandling();
	ListenTo(this);

	(scrollbarSet->GetVScrollbar())->SetScrollDelay(0);

	UseMultipleUndo();
	SetLastSaveLocation();
	ShouldAutoIndent(kJTrue);
	CBShouldAllowDragAndDrop(kJTrue);	// new users expect it
	ShouldMoveToFrontOfText(kJTrue);

	itsLineInput->ShareEditMenu(this);
	itsColInput->ShareEditMenu(this);

	SetCharacterInWordFunction(CBMIsCharacterInWord);

	JTEKeyHandler* handler;
	CBInstallEmulator(CBGetPrefsManager()->GetEmulator(), this, &handler);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTextEditor::~CBTextEditor()
{
	jdelete itsTokenStartList;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBTextEditor::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
		{
		if (vers == 0)
			{
			JIgnoreUntil(input, '\1');
			input >> vers;
			assert( vers == 0 );
			}

		JSize tabCharCount;
		JBoolean autoIndent;
		input >> tabCharCount >> autoIndent;
		SetTabCharCount(tabCharCount);
		ShouldAutoIndent(autoIndent);

		if (vers >= 2)
			{
			JBoolean allowDND, moveFrontOfText;
			JSize undoDepth;
			input >> allowDND >> moveFrontOfText >> undoDepth;

			if (vers == 2)
				{
				// It actually read first digit of font name,
				// so we reset it.
				undoDepth = 100;
				}

			CBShouldAllowDragAndDrop(allowDND);
			ShouldMoveToFrontOfText(moveFrontOfText);
			SetUndoDepth(undoDepth);
			}

		if (vers >= 4)
			{
			JSize lineWidth;
			input >> lineWidth;
			SetCRMLineWidth(lineWidth);

			input >> itsBalanceWhileTypingFlag;
			input >> itsScrollToBalanceFlag >> itsBeepWhenTypeUnbalancedFlag;
			}

		if (vers == 5)
			{
			JBoolean newLineAfterSemiFlag;
			input >> newLineAfterSemiFlag;
			}

		if (vers >= 7)
			{
			input >> itsSmartTabFlag;
			}

		if (8 <= vers && vers < 10)
			{
			long mod;
			input >> mod;

			if ((JXGetSharedPrefsManager())->WasNew())
				{
				SetPartialWordModifier((PartialWordModifier) mod);
				}
			}

		if (vers >= 9)
			{
			JBoolean tabToSpaces;
			input >> tabToSpaces;
			TabShouldInsertSpaces(tabToSpaces);
			}

		if (vers >= 11)
			{
			input >> itsDrawRightMarginFlag >> itsRightMarginWidth;
			}
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBTextEditor::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsTabCharCount;
	output << ' ' << WillAutoIndent();
	output << ' ' << AllowsDragAndDrop();
	output << ' ' << WillMoveToFrontOfText();
	output << ' ' << GetUndoDepth();
	output << ' ' << GetCRMLineWidth();
	output << ' ' << itsBalanceWhileTypingFlag;
	output << ' ' << itsScrollToBalanceFlag;
	output << ' ' << itsBeepWhenTypeUnbalancedFlag;
	output << ' ' << itsSmartTabFlag;
	output << ' ' << TabInsertsSpaces();
	output << ' ' << itsDrawRightMarginFlag;
	output << ' ' << itsRightMarginWidth;
	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBTextEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDoc && message.Is(JXFileDocument::kNameChanged))
		{
		const JXFileDocument::NameChanged* info =
			dynamic_cast<const JXFileDocument::NameChanged*>(&message);
		assert( info != nullptr );
		UpdateWritable(info->GetFullName());
		}

	else if (sender == itsContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
		}

	else if (message.Is(JXFSDirMenu::kFileSelected))
		{
		const JXFSDirMenu::FileSelected* info =
			dynamic_cast<const JXFSDirMenu::FileSelected*>(&message);
		assert( info != nullptr );
		JBoolean onDisk;
		const JString fullName = itsDoc->GetFullName(&onDisk);
		if (onDisk)
			{
			(CBGetRunTEScriptDialog())->RunSimpleScript(info->GetFileName(), this, fullName);
			}
		}

	else
		{
		JXTextMenu* editMenu;
		JBoolean ok = GetEditMenu(&editMenu);
		assert( ok );

		JXTextMenu* searchMenu;
		ok = GetSearchReplaceMenu(&searchMenu);
		assert( ok );

		if (sender == editMenu && message.Is(JXMenu::kNeedsUpdate))
			{
			UpdateCustomEditMenuItems();
			}
		else if (sender == editMenu && message.Is(JXMenu::kItemSelected))
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			if (HandleCustomEditMenuItems(selection->GetIndex()))
				{
				return;
				}
			}

		else if (sender == searchMenu && message.Is(JXMenu::kNeedsUpdate))
			{
			UpdateCustomSearchMenuItems();
			}
		else if (sender == searchMenu && message.Is(JXMenu::kItemSelected))
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			if (HandleCustomSearchMenuItems(selection->GetIndex()))
				{
				return;
				}
			}

		else if (sender == this &&
				 (message.Is(kTextSet) ||
				  message.Is(kTextChanged)))
			{
			if (itsFnMenu != nullptr)
				{
				itsFnMenu->TextChanged(itsDoc->GetFileType(), "");
				}
			}
		else if (sender == this && message.Is(kTypeChanged))
			{
			UpdateTabHandling();
			}

		JXTEBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateCustomEditMenuItems (private)

 ******************************************************************************/

void
CBTextEditor::UpdateCustomEditMenuItems()
{
	JXTextMenu* editMenu;
	const JBoolean ok = GetEditMenu(&editMenu);
	assert( ok );

	if (GetType() == kFullEditor)
		{
		editMenu->EnableItem(itsExecScriptCmdIndex);
		editMenu->EnableItem(itsExecScriptCmdIndex+1);	// script menu
		}

	if (HasSelection())
		{
		editMenu->SetItemText(itsExecScriptCmdIndex, kExecScriptSelStr);
		}
	else
		{
		editMenu->SetItemText(itsExecScriptCmdIndex, kExecScriptStr);
		}
}

/******************************************************************************
 HandleCustomEditMenuItems (private)

	Returns kJTrue if it is one of our items.

 ******************************************************************************/

JBoolean
CBTextEditor::HandleCustomEditMenuItems
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	if (index == itsExecScriptCmdIndex)
		{
		(CBGetRunTEScriptDialog())->Activate();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UpdateCustomSearchMenuItems (private)

 ******************************************************************************/

void
CBTextEditor::UpdateCustomSearchMenuItems()
{
	JXTextMenu* searchMenu;
	const JBoolean ok = GetSearchReplaceMenu(&searchMenu);
	assert( ok );

	searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToLineCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToColCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kPlaceBookmarkCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kFindSourceFileCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kViewManPageCmd);

	if (!IsEmpty())
		{
		searchMenu->EnableItem(itsFirstSearchMenuItem + kBalanceCmd);
		}

	if (HasSelection())
		{
		searchMenu->EnableItem(itsFirstSearchMenuItem + kOpenSelectionAsFileCmd);
		searchMenu->EnableItem(itsFirstSearchMenuItem + kFindSelectionAsSymbolCmd);
		searchMenu->EnableItem(itsFirstSearchMenuItem + kFindSelectionAsSymbolNoContextCmd);
		searchMenu->EnableItem(itsFirstSearchMenuItem + kFindSelectionInManPageCmd);
		}

	CBProjectDocument* projDoc;
	if (CBGetDocumentManager()->GetActiveProjectDocument(&projDoc))
		{
		JString itemText = projDoc->GetName();
		itemText.Prepend(kFindSelAsSymInText);
		searchMenu->SetItemText(itsFirstSearchMenuItem + kFindSelectionAsSymbolCmd, itemText);
		}
	else
		{
		searchMenu->SetItemText(itsFirstSearchMenuItem + kFindSelectionAsSymbolCmd, kFindSelAsSymText);
		}

	CBExecOutputDocument* listDoc;
	if (CBGetDocumentManager()->GetActiveListDocument(&listDoc))
		{
		searchMenu->EnableItem(itsFirstSearchMenuItem + kOpenPrevListItemCmd);
		searchMenu->EnableItem(itsFirstSearchMenuItem + kOpenNextListItemCmd);
		}
}

/******************************************************************************
 HandleCustomSearchMenuItems (private)

 ******************************************************************************/

JBoolean
CBTextEditor::HandleCustomSearchMenuItems
	(
	const JIndex origIndex
	)
{
	const JInteger index = JInteger(origIndex) - JInteger(itsFirstSearchMenuItem);
	if (index != kGoToLineCmd && index != kGoToColCmd)
		{
		Focus();
		}
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	if (index == kBalanceCmd)
		{
		CBMBalanceFromSelection(this, CBGetLanguage(itsDoc->GetFileType()));
		return kJTrue;
		}

	else if (index == kGoToLineCmd)
		{
		itsLineInput->Focus();
		return kJTrue;
		}
	else if (index == kGoToColCmd)
		{
		itsColInput->Focus();
		return kJTrue;
		}
	else if (index == kPlaceBookmarkCmd)
		{
		PlaceBookmark();
		return kJTrue;
		}

	else if (index == kOpenSelectionAsFileCmd)
		{
		OpenSelection();
		return kJTrue;
		}
	else if (index == kFindSourceFileCmd)
		{
		(CBGetFindFileDialog())->Activate();
		return kJTrue;
		}

	else if (index == kFindSelectionAsSymbolCmd)
		{
		FindSelectedSymbol(kJXLeftButton, kJTrue);
		return kJTrue;
		}
	else if (index == kFindSelectionAsSymbolNoContextCmd)
		{
		FindSelectedSymbol(kJXLeftButton, kJFalse);
		return kJTrue;
		}
	else if (index == kFindSelectionInManPageCmd)
		{
		DisplayManPage();
		return kJTrue;
		}
	else if (index == kViewManPageCmd)
		{
		(CBGetViewManPageDialog())->Activate();
		return kJTrue;
		}

	else if (index == kOpenPrevListItemCmd)
		{
		CBExecOutputDocument* listDoc;
		if (CBGetDocumentManager()->GetActiveListDocument(&listDoc))
			{
			listDoc->OpenPrevListItem();
			}
		return kJTrue;
		}
	else if (index == kOpenNextListItemCmd)
		{
		CBExecOutputDocument* listDoc;
		if (CBGetDocumentManager()->GetActiveListDocument(&listDoc))
			{
			listDoc->OpenNextListItem();
			}
		return kJTrue;
		}

	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PlaceBookmark (private)

 ******************************************************************************/

void
CBTextEditor::PlaceBookmark()
{
	JXScrollbar *hScrollbar, *vScrollbar;
	const JBoolean hasScroll = GetScrollbars(&hScrollbar, &vScrollbar);
	assert( hasScroll );
	vScrollbar->PlaceScrolltab();
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
CBTextEditor::UpdateContextMenu()
{
	const JBoolean hasSelection = HasSelection();
	itsContextMenu->SetItemEnable(kContextCutCmd, hasSelection);
	itsContextMenu->SetItemEnable(kContextCopyCmd, hasSelection);
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
CBTextEditor::HandleContextMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	if (index == kContextCutCmd)
		{
		Cut();
		}
	else if (index == kContextCopyCmd)
		{
		Copy();
		}
	else if (index == kContextPasteCmd)
		{
		Paste();
		}

	else if (index == kContextCompleteCmd)
		{
		JIndexRange r;
		if (GetSelection(&r))
			{
			SetCaretLocation(r.last+1);
			}

		CBStringCompleter* completer = nullptr;
		if (itsDoc->GetStringCompleter(&completer))
			{
			itsCompletionMenu->ClearRequestCount();
			itsCompletionMenu->CompletionRequested(1);
			completer->Complete(this, itsCompletionMenu);
			}
		}
	else if (index == kContextMacroCmd)
		{
		JIndexRange r;
		if (GetSelection(&r))
			{
			SetCaretLocation(r.last+1);
			}

		CBMacroManager* macroMgr = nullptr;
		if (itsDoc->GetMacroManager(&macroMgr))
			{
			macroMgr->Perform(GetInsertionIndex(), itsDoc);
			}
		}

	else if (index == kContextShiftLeftCmd)
		{
		TabSelectionLeft();
		}
	else if (index == kContextShiftRightCmd)
		{
		TabSelectionRight();
		}

	else if (index == kContextFindSelBackCmd)
		{
		SelectWordIfNoSelection();
		SearchSelectionBackward();
		}
	else if (index == kContextFindSelFwdCmd)
		{
		SelectWordIfNoSelection();
		SearchSelectionForward();
		}

	else if (index == kContextFindSymbolCmd)
		{
		SelectWordIfNoSelection();
		FindSelectedSymbol(kJXLeftButton, kJTrue);
		}
	else if (index == kContextFindSymbolNoContextCmd)
		{
		SelectWordIfNoSelection();
		FindSelectedSymbol(kJXLeftButton, kJFalse);
		}
	else if (index == kContextFindSymbolManPageCmd)
		{
		SelectWordIfNoSelection();
		DisplayManPage();
		}

	else if (index == kContextOpenSelAsFileCmd)
		{
		SelectWordIfNoSelection();
		OpenSelection();
		}
	else if (index == kShowInFileMgrCmd)
		{
		JBoolean onDisk;
		const JString fullName = itsDoc->GetFullName(&onDisk);
		if (onDisk)
			{
			(JXGetWebBrowser())->ShowFileLocation(fullName);
			}
		}

	else if (index == kContextBalanceCmd)
		{
		CBMBalanceFromSelection(this, CBGetLanguage(itsDoc->GetFileType()));
		}
	else if (index == kContextPlaceBookmarkCmd)
		{
		PlaceBookmark();
		}
}

/******************************************************************************
 SelectWordIfNoSelection (private)

 ******************************************************************************/

void
CBTextEditor::SelectWordIfNoSelection()
{
	if (!HasSelection())
		{
		JIndexRange r;
		TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
		SetSelection(r);
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CBTextEditor::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsDrawRightMarginFlag && CBDrawRightMargin(itsDoc->GetFileType()))
		{
		const JCoordinate x =
			TEGetLeftMarginWidth() +
			(itsRightMarginWidth * GetDefaultFont().GetCharWidth(' '));

		const JColorID saveColor = p.GetPenColor();
		p.SetPenColor(itsRightMarginColor);
		p.Line(x, rect.top, x, rect.bottom);
		p.SetPenColor(saveColor);
		}

	JXTEBase::Draw(p, rect);
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the default cursor during drag-and-drop.

 ******************************************************************************/

void
CBTextEditor::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean selectionSpecial =
		modifiers.meta() ? !itsAllowDNDFlag : itsAllowDNDFlag;
	ShouldAllowDragAndDrop(selectionSpecial);
	if (selectionSpecial && PointInSelection(pt))
		{
		DisplayCursor(kJXDefaultCursor);
		}
	else
		{
		JXTEBase::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBTextEditor::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);
	if (itsCompletionMenu != nullptr)
		{
		itsCompletionMenu->ClearRequestCount();
		}
	itsColInput->ShouldOptimizeUpdate(kJTrue);

	const JBoolean selectionSpecial =
		modifiers.meta() ? !itsAllowDNDFlag : itsAllowDNDFlag;
	ShouldAllowDragAndDrop(selectionSpecial);

	itsLastClickCount = clickCount;
	itsLastModifiers  = modifiers;

	if (selectionSpecial &&
		button == kJXRightButton && clickCount == 1)
		{
		if (!PointInSelection(pt))
			{
			JXKeyModifiers emptyMod(GetDisplay());
			JXTEBase::HandleMouseDown(pt, kJXLeftButton, 1, buttonStates, emptyMod);
			}
		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (button <= kJXRightButton &&
			 (modifiers.meta() || modifiers.control()))
		{
		JXKeyModifiers emptyMod(GetDisplay());
		JXTEBase::HandleMouseDown(pt, kJXLeftButton, clickCount, buttonStates, emptyMod);
		}
	else
		{
		JXTEBase::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CBTextEditor::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXTEBase::HandleMouseUp(pt, button, buttonStates, modifiers);
	if (buttonStates.AllOff())
		{
		itsColInput->ShouldOptimizeUpdate(kJFalse);
		}

	if (button <= kJXRightButton && itsLastClickCount == 2 &&
		itsLastModifiers.meta() && !itsLastModifiers.control())
		{
		FindSelectedSymbol(button, !itsLastModifiers.shift());
		}
	else if (button == kJXLeftButton && itsLastClickCount == 2 &&
			 !itsLastModifiers.meta() && itsLastModifiers.control() &&
			 !itsLastModifiers.shift())
		{
		OpenSelection();
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

#include <jx_edit_cut.xpm>
#include <jx_edit_copy.xpm>
#include <jx_edit_paste.xpm>
#include <jx_edit_shift_left.xpm>
#include <jx_edit_shift_right.xpm>
#include <jx_find_selection_backwards.xpm>
#include <jx_find_selection_forward.xpm>

void
CBTextEditor::CreateContextMenu()
{
	if (itsContextMenu == nullptr)
		{
		itsContextMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != nullptr );
		itsContextMenu->SetMenuItems(kContextMenuStr, "CBTextEditor");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);

		CreateScriptMenu(itsContextMenu, kContextScriptSubmenuIndex);

		itsContextMenu->SetItemImage(kContextCutCmd,         jx_edit_cut);
		itsContextMenu->SetItemImage(kContextCopyCmd,        jx_edit_copy);
		itsContextMenu->SetItemImage(kContextPasteCmd,       jx_edit_paste);
		itsContextMenu->SetItemImage(kContextShiftLeftCmd,   jx_edit_shift_left);
		itsContextMenu->SetItemImage(kContextShiftRightCmd,  jx_edit_shift_right);
		itsContextMenu->SetItemImage(kContextFindSelBackCmd, jx_find_selection_backwards);
		itsContextMenu->SetItemImage(kContextFindSelFwdCmd,  jx_find_selection_forward);
		itsContextMenu->SetItemImage(kContextBalanceCmd,     jcc_balance_braces);
		}
}

/******************************************************************************
 CreateScriptMenu (private)

 ******************************************************************************/

CBTEScriptMenu*
CBTextEditor::CreateScriptMenu
	(
	JXMenu*			parent,
	const JIndex	index
	)
{
	CBTEScriptMenu* menu =
		jnew CBTEScriptMenu(this, itsScriptPath, parent, index, parent->GetEnclosure());
	assert( menu != nullptr );
	ListenTo(menu);

	JXTextMenu* editMenu;
	JBoolean ok = GetEditMenu(&editMenu);
	assert( ok );
	const JXImage* image;
	ok = editMenu->GetItemImage(itsExecScriptCmdIndex, &image);
	assert( ok );
	menu->SetExecIcon(*image);

	menu->SetEmptyMessage(JGetString(kEmptyScriptMenuID));
	return menu;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBTextEditor::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();

	CBStringCompleter* completer = nullptr;
	JBoolean clearRequestCount   = kJTrue;

	CBMacroManager* macroMgr = nullptr;
	JIndex caretIndex        = 0;

	const Type type = GetType();

	if (key == '\t' && controlOn && !metaOn && !shiftOn)
		{
		// We have to do this here because JXWindow can only be persuaded
		// to send it to the widget with focus, not as a shortcut.

		itsDoc->HandleActionButton();
		}

	else if (key == XK_F12)
		{
		if (!HasSelection())
			{
			JIndexRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), kJFalse, kJFalse, &r);
			SetSelection(r);
			}
		FindSelectedSymbol(shiftOn ? kJXMiddleButton : kJXLeftButton, kJTrue);
		}

	else if (controlOn && (key == kJUpArrow || key == kJDownArrow) && !metaOn)
		{
		MoveCaretToEdge(key);
		}

	else if (type == kFullEditor &&
			 itsSmartTabFlag && key == '\t' && !shiftOn && !metaOn && !controlOn &&
			 GetCaretLocation(&caretIndex) &&
			 itsDoc->GetMacroManager(&macroMgr) &&
			 macroMgr->Perform(caretIndex, itsDoc))
		{
		// Perform() does the work
		}

	else if (type == kFullEditor &&
			 key == ' ' && controlOn && !shiftOn && !metaOn &&
			 GetCaretLocation(&caretIndex) &&
			 itsDoc->GetMacroManager(&macroMgr))
		{
		macroMgr->Perform(caretIndex, itsDoc);	// prevent other Ctrl-space behavior
		}

	else if (type == kFullEditor &&
			 itsSmartTabFlag && key == '\t' && !shiftOn && !metaOn && !controlOn &&
			 itsDoc->GetStringCompleter(&completer) &&
			 completer->Complete(this, itsCompletionMenu))
		{
		// Complete() does the work
		clearRequestCount = kJFalse;
		}

	else if (type == kFullEditor &&
			 key == ' ' && controlOn && !metaOn &&		// Ctrl-space & Ctrl-Shift-space
			 itsDoc->GetStringCompleter(&completer))
		{
		completer->Complete(this, itsCompletionMenu);	// prevent other Ctrl-space behavior
		clearRequestCount = kJFalse;
		}

	else
		{
		const CRMRuleList* ruleList = nullptr;
		if ((key == '\r' || key == '\n') && shiftOn &&
			GetCRMRuleList(&ruleList))
			{
			ClearCRMRuleList();
			}

		if (!(shiftOn && (key == kJUpArrow || key == kJDownArrow ||
						  key == kJLeftArrow || key == kJRightArrow)))
			{
			itsColInput->ShouldOptimizeUpdate(kJTrue);
			}
		JXTEBase::HandleKeyPress(key, modifiers);
		itsColInput->ShouldOptimizeUpdate(kJFalse);

		if (ruleList != nullptr)
			{
			SetCRMRuleList(const_cast<CRMRuleList*>(ruleList), kJFalse);
			}

		if (itsBalanceWhileTypingFlag &&
			!metaOn && !controlOn &&
			CBMIsCloseGroup(CBGetLanguage(itsDoc->GetFileType()), key) &&
			!HasSelection())
			{
			ShowBalancingOpenGroup();
			}

		CBCharActionManager* mgr;
		if (!metaOn && !controlOn && itsDoc->GetCharActionManager(&mgr))
			{
			mgr->Perform(key, itsDoc);
			}
		}

	if (clearRequestCount && itsCompletionMenu != nullptr)
		{
		itsCompletionMenu->ClearRequestCount();
		}
}

/******************************************************************************
 HandleShortcut (virtual)

 ******************************************************************************/

void
CBTextEditor::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);
	if (itsCompletionMenu != nullptr)
		{
		itsCompletionMenu->ClearRequestCount();
		}
	JXTEBase::HandleShortcut(key, modifiers);
}

/******************************************************************************
 FileTypeChanged

 ******************************************************************************/

void
CBTextEditor::FileTypeChanged
	(
	const CBTextFileType type
	)
{
	if ((CBGetFnMenuUpdater())->CanCreateMenu(type))
		{
		if (itsFnMenu == nullptr)
			{
			JXMenuBar* menuBar = itsDoc->GetMenuBar();

			itsFnMenu = jnew CBFunctionMenu(itsDoc, type, this, menuBar,
										   kFixedLeft, kFixedTop, 0,0, 10,10);
			assert( itsFnMenu != nullptr );

			JXTextMenu* searchMenu;
			const JBoolean ok = GetSearchReplaceMenu(&searchMenu);
			assert( ok );

			menuBar->InsertMenuAfter(searchMenu, itsFnMenu);
			}
		else
			{
			itsFnMenu->TextChanged(type, "");
			}
		}
	else
		{
		jdelete itsFnMenu;
		itsFnMenu = nullptr;
		}

	CBStringCompleter* completer = nullptr;
	if (itsDoc->GetStringCompleter(&completer))
		{
		itsCompletionMenu = jnew JXStringCompletionMenu(this, kJTrue);
		assert( itsCompletionMenu != nullptr );
		}
	else
		{
		jdelete itsCompletionMenu;
		itsCompletionMenu = nullptr;
		}
}

/******************************************************************************
 UpdateWritable

 ******************************************************************************/

void
CBTextEditor::UpdateWritable
	(
	const JCharacter* name
	)
{
	SetWritable( JI2B( !JFileExists(name) || JFileWritable(name) ) );
}

/******************************************************************************
 SetWritable

	Choose whether or not the buffer is editable.

 ******************************************************************************/

void
CBTextEditor::SetWritable
	(
	const JBoolean writable
	)
{
	if (writable)
		{
		SetType(kFullEditor);
		}
	else
		{
		SetType(kSelectableText);
		}
}

/******************************************************************************
 UpdateTabHandling (private)

	If we are not editable, Tab might as well switch focus.

 ******************************************************************************/

void
CBTextEditor::UpdateTabHandling()
{
	WantInput(kJTrue, JI2B(GetType() == kFullEditor), kJTrue);
}

/******************************************************************************
 VIsCharacterInWord (virtual protected)

 ******************************************************************************/

JBoolean
CBTextEditor::VIsCharacterInWord
	(
	const JString&	text,
	const JIndex	charIndex
	)
	const
{
	return JI2B(JXTEBase::VIsCharacterInWord(text, charIndex) ||
				CBIsCharacterInWord(itsDoc->GetFileType(), text, charIndex));
}

/******************************************************************************
 FindSelectedSymbol (private)

	First search the symbol list, then the man pages.

 ******************************************************************************/

void
CBTextEditor::FindSelectedSymbol
	(
	const JXMouseButton button,
	const JBoolean		useContext
	)
{
	JString str;
	CBProjectDocument* projDoc = nullptr;

	JBoolean onDisk;
	JString fullName = itsDoc->GetFullName(&onDisk);
	if (!onDisk || !useContext)
		{
		fullName.Clear();
		}

	if (!GetSelection(&str) || str.Contains("\n") || str.GetLength() < 2)
		{
		return;
		}

	if (!CBGetDocumentManager()->GetActiveProjectDocument(&projDoc) ||
		!(projDoc->GetSymbolDirector())->FindSymbol(str, fullName, button))
		{
		// If we can't find it anywhere else, check the man pages.
		DisplayManPage();
		}
}

/******************************************************************************
 DisplayManPage (private)

	If the selection is followed by "(c)", then run "man c <selection>".

 ******************************************************************************/

static const JRegex sectionRegex(
	"([[:space:]]*,[[:space:]]*[^(,[:space:]]+)*[[:space:]]*\\([^)]\\)");

void
CBTextEditor::DisplayManPage()
{
	JIndex startIndex, endIndex;
	if (GetSelection(&startIndex, &endIndex))
		{
		const JString& text  = GetText();
		const JSize length   = text.GetLength();
		const JString fnName = text.GetSubstring(startIndex, endIndex);
		JCharacter manIndex  = ' ';

		const JIndexRange textRange(endIndex+1, length);
		JIndexRange matchRange;
		if (sectionRegex.MatchWithin(text, textRange, &matchRange) &&
			matchRange.first == textRange.first)
			{
			manIndex = text.GetCharacter(endIndex + matchRange.GetLength() - 1);
			}

		CBManPageDocument::Create(nullptr, fnName, manIndex);
		}
}

/******************************************************************************
 OpenSelection

	Select the rest of the file or URL name and then try to open in.

 ******************************************************************************/

static const JRegex urlPrefixRegex = "^[A-Za-z0-9]+://";

void
CBTextEditor::OpenSelection()
{
	// get selected text

	JIndex startIndex, endIndex;
	if (!GetSelection(&startIndex, &endIndex))
		{
		return;
		}

	JIndex lineIndex;
	if (!IsNonstdError(&startIndex, &endIndex, &lineIndex))
		{
		// extend selection to entire file name

		startIndex = GetWordStart(endIndex);
		endIndex   = GetWordEnd(startIndex);
		lineIndex  = 0;

		const JString& text    = GetText();
		const JSize textLength = text.GetLength();

		while (startIndex > 1)
			{
			JCharacter c = text.GetCharacter(startIndex-1);

			// catch "#include </usr/junk.h>", "#include <../junk.h>"

			while ((c == ACE_DIRECTORY_SEPARATOR_CHAR || c == '.') && startIndex > 2)
				{
				startIndex--;
				c = text.GetCharacter(startIndex-1);
				}

			// stop correctly for
			// <a href=mailto:jafl>
			// (MyClass.java:35)
			// <url>http://yahoo.com</url>

			if (isspace(c) || c == '"' || c == '\'' || c == '<' || c == '>' ||
				c == '=' || c == '(' || c == '[' || c == '{')
				{
				break;
				}
			startIndex = GetWordStart(startIndex-1);
			}

		while (endIndex < textLength)
			{
			JCharacter c = text.GetCharacter(endIndex+1);

			// catch "http://junk/>"

			while (c == ACE_DIRECTORY_SEPARATOR_CHAR && endIndex < textLength-1)
				{
				endIndex++;
				c = text.GetCharacter(endIndex+1);
				}

			// don't include line number for file:line
			// stop correctly for
			// (MyClass.java)
			// <url>http://yahoo.com</url>

			if (isspace(c) || c == '"' || c == '\'' || c == '>' || c == '<' ||
				c == ')' || c == ']' || c == '}')
				{
				break;
				}
			else if (c == ':' && endIndex < textLength-1 &&
					 isdigit(text.GetCharacter(endIndex+2)))
				{
				lineIndex = GetLineIndex(endIndex+2);
				break;
				}
			endIndex = GetWordEnd(endIndex+1);
			}
		}

	SetSelection(startIndex, endIndex);
	GetWindow()->Update();				// show selection while we work

	JString str = (GetText()).GetSubstring(startIndex, endIndex);
	if (str.Contains("\n"))
		{
		return;
		}

	if (urlPrefixRegex.Match(str))
		{
		(JXGetWebBrowser())->ShowURL(str);
		}
	else
		{
		itsDoc->ConvertSelectionToFullPath(&str);
		(CBGetApplication())->FindAndViewFile(str, JIndexRange(lineIndex, lineIndex));
		}
}

/******************************************************************************
 GetLineIndex (private)

	Returns the value of the integer starting at startIndex.
	Returns zero if there is nothing there.

 ******************************************************************************/

JIndex
CBTextEditor::GetLineIndex
	(
	const JIndex startIndex
	)
	const
{
	const JString& text    = GetText();
	const JSize textLength = text.GetLength();

	JString str;
	JIndex i = startIndex;
	while (i <= textLength)
		{
		const JCharacter c = text.GetCharacter(i);
		if (isdigit(c))
			{
			str.AppendCharacter(c);
			}
		else
			{
			break;
			}
		i++;
		}

	JIndex lineIndex;
	if (str.ConvertToUInt(&lineIndex))
		{
		return lineIndex;
		}
	else
		{
		return 0;
		}
}

/******************************************************************************
 IsNonstdError (private)

	flex:       "$", line #:
	bison:      ("$", line #) error:
	Absoft F77:  error on line # of $:
	Maven:      $:[#,

 ******************************************************************************/

static const JRegex flexErrorRegex   = "..\", line ([0-9]+)(\\.[0-9]+)?: ";
static const JRegex bisonErrorRegex  = "...\", line ([0-9]+)\\) error: ";
static const JRegex absoftErrorRegex = " error on line ([0-9]+) of ([^:]+): ";
static const JRegex mavenErrorRegex  = "(?:\\[[^]]+\\]\\s+)?([^:]+):\\[([0-9]+),";

JBoolean
CBTextEditor::IsNonstdError
	(
	JIndex* fileNameStart,
	JIndex* fileNameEnd,
	JIndex* lineIndex
	)
	const
{
	const JIndex caretIndex = GetInsertionIndex();
	const JIndex startIndex = GetParagraphStart(caretIndex);
	const JIndex endIndex   = GetParagraphEnd(caretIndex);
	const JString line      = (GetText()).GetSubstring(startIndex, endIndex);

	JArray<JIndexRange> matchList;
	if (flexErrorRegex.Match(line, &matchList) &&
		line.GetFirstCharacter() == '"' &&
		caretIndex - startIndex + 1 < (matchList.GetFirstElement()).last - 1)
		{
		*fileNameStart = startIndex+1;
		*fileNameEnd   = (matchList.GetFirstElement()).first + startIndex;

		const JString lineStr = line.GetSubstring(matchList.GetElement(2));
		const JBoolean ok     = lineStr.ConvertToUInt(lineIndex);
		assert( ok );

		return kJTrue;
		}
	else if (bisonErrorRegex.Match(line, &matchList) &&
			 line.BeginsWith("(\"") &&
			 caretIndex - startIndex + 1 < (matchList.GetFirstElement()).last - 8)
		{
		*fileNameStart = startIndex+2;
		*fileNameEnd   = (matchList.GetFirstElement()).first + startIndex+1;

		const JString lineStr = line.GetSubstring(matchList.GetElement(2));
		const JBoolean ok     = lineStr.ConvertToUInt(lineIndex);
		assert( ok );

		return kJTrue;
		}
	else if (absoftErrorRegex.Match(line, &matchList) &&
			 (matchList.GetElement(3)).Contains(caretIndex - startIndex + 1))
		{
		const JIndexRange fileNameRange = matchList.GetElement(3);
		*fileNameStart = startIndex + fileNameRange.first-1;
		*fileNameEnd   = startIndex + fileNameRange.last -1;

		const JString lineStr = line.GetSubstring(matchList.GetElement(2));
		const JBoolean ok     = lineStr.ConvertToUInt(lineIndex);
		assert( ok );

		return kJTrue;
		}
	else if (mavenErrorRegex.Match(line, &matchList) &&
			 (matchList.GetElement(2)).Contains(caretIndex - startIndex + 1))
		{
		const JIndexRange fileNameRange = matchList.GetElement(2);
		*fileNameStart = startIndex + fileNameRange.first-1;
		*fileNameEnd   = startIndex + fileNameRange.last -1;

		const JString lineStr = line.GetSubstring(matchList.GetElement(3));
		const JBoolean ok     = lineStr.ConvertToUInt(lineIndex);
		assert( ok );

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetFont

	We do not check whether the font is different because CBEditTextPrefsDialog
	does this once, instead of each CBTextEditor doing it.

 ******************************************************************************/

void
CBTextEditor::SetFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JSize			tabCharCount,
	const JBoolean		breakCROnly
	)
{
	PrivateSetTabCharCount(tabCharCount);
	SetAllFontNameAndSize(name, size,
		CalcTabWidth(JFontManager::GetFont(name, size), tabCharCount),
		breakCROnly, kJFalse);
}

/******************************************************************************
 SetTabCharCount

 ******************************************************************************/

void
CBTextEditor::SetTabCharCount
	(
	const JSize charCount
	)
{
	if (charCount != itsTabCharCount)
		{
		PrivateSetTabCharCount(charCount);
		SetDefaultTabWidth(
			CalcTabWidth(GetDefaultFont(), charCount));
		}
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

 ******************************************************************************/

void
CBTextEditor::xAdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	CBStylerBase* styler = nullptr;
	if (GetTextLength() < CBDocumentManager::kMinWarnFileSize &&
		itsDoc->GetStyler(&styler))
		{
		styler->UpdateStyles(this, buffer, styles,
							 recalcRange, redrawRange,
							 deletion, itsTokenStartList);
		}
	else
		{
		itsTokenStartList->RemoveAll();
		}
}

/******************************************************************************
 RecalcStyles

	Redo all the styles, usually because the styler changed.

 ******************************************************************************/

void
CBTextEditor::RecalcStyles()
{
	const JSize length = GetTextLength();
	if (length == 0)
		{
		return;
		}

	CBStylerBase* styler;
	if (length < CBDocumentManager::kMinWarnFileSize &&
		itsDoc->GetStyler(&styler))
		{
		RecalcAll(kJTrue);
		}
	else
		{
		JFont font = GetFont(1);
		font.ClearStyle();
		JTextEditor::SetFont(1, GetTextLength(), font, kJFalse);
		}
}

/******************************************************************************
 ShowBalancingOpenGroup (private)

 ******************************************************************************/

void
CBTextEditor::ShowBalancingOpenGroup()
{
	const CBLanguage lang = CBGetLanguage(itsDoc->GetFileType());

	const JIndex origCaretIndex = GetInsertionIndex();
	const JIndex closeIndex     = origCaretIndex - 1;
	if (closeIndex > 0)		// must be the case unless something fails
		{
		const JString& text        = GetText();
		const JCharacter closeChar = text.GetCharacter(closeIndex);
		if (CBMIsCloseGroup(lang, closeChar))
			{
			JIndex openIndex     = closeIndex;
			const JBoolean found = CBMBalanceBackward(lang, text, &openIndex);
			if (found && CBMIsMatchingPair(lang, text.GetCharacter(openIndex), closeChar))
				{
				const JPoint savePt = (GetAperture()).topLeft();
				SetSelection(openIndex, openIndex);
				if (!TEScrollToSelection(kJFalse) || itsScrollToBalanceFlag)
					{
					GetWindow()->Update();
					JWait(kBalanceWhileTypingDelay);
					}
				SetCaretLocation(origCaretIndex);
				ScrollTo(savePt);
				}
			else if (itsBeepWhenTypeUnbalancedFlag)
				{
				GetDisplay()->Beep();
				}
			}
		}
}

/******************************************************************************
 ScrollForDefinition

	Assumes that the line of the function's definition is selected.

 ******************************************************************************/

void
CBTextEditor::ScrollForDefinition
	(
	const CBLanguage lang
	)
{
	CBMScrollForDefinition(this, lang);
}

/******************************************************************************
 Set font for PrintPS

 ******************************************************************************/

void
CBTextEditor::SetFontBeforePrintPS
	(
	const JSize fontSize
	)
{
	JString fontName;
	JSize size;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &size);
	if (fontName != "Courier")
		{
		itsSavedBreakCROnlyFlag = WillBreakCROnly();
		SetFont("Courier", fontSize, itsTabCharCount, kJFalse);
		}
}

void
CBTextEditor::ResetFontAfterPrintPS()
{
	JString fontName;
	JSize fontSize;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	if (fontName != "Courier")
		{
		SetFont(fontName, fontSize, itsTabCharCount, itsSavedBreakCROnlyFlag);
		}
}

/******************************************************************************
 Print header & footer (virtual protected)

 ******************************************************************************/

JCoordinate
CBTextEditor::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return ((CBGetPTTextPrinter())->WillPrintHeader() ? 4 * p.GetLineHeight() : 0);
}

void
CBTextEditor::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	if ((CBGetPTTextPrinter())->WillPrintHeader())
		{
		JRect pageRect = p.GetPageRect();
		p.String(pageRect, (CBGetPSTextPrinter())->GetHeaderName());

		pageRect.top += p.GetLineHeight();
		const JString dateStr = JGetTimeStamp();
		p.String(pageRect, dateStr);

		JString pageStr(p.GetPageIndex(), 0);
		pageStr.Prepend("Page ");
		p.String(pageRect, pageStr, JPainter::kHAlignRight);
		}
}
