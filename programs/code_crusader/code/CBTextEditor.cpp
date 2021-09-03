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

	Copyright © 1996-2001 by John Lindal.

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
#include <JStringIterator.h>
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
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

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

static const JString kExecScriptAction("ExecScript::CBTextEditor", JString::kNoCopy);

// Search menu

#include "jcc_balance_braces.xpm"
#include "jcc_view_man_page.xpm"

static const JUtf8Byte* kSearchMenuStr =
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

static const JUtf8Byte* kContextMenuStr =
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

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTextEditor::CBTextEditor
	(
	CBTextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	const bool		pasteStyledText,
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
	JXTEBase(kFullEditor, jnew StyledText(document, enclosure->GetFontManager(), pasteStyledText), true,
			 false, scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsLastModifiers(GetDisplay())
{
	itsDoc = document;
	ListenTo(itsDoc);

	UpdateWritable(fileName);

	itsLineInput = lineInput;
	itsLineInput->SetTE(this, colInput);

	itsColInput = colInput;
	itsColInput->SetTE(this);

	itsBalanceWhileTypingFlag     = true;
	itsScrollToBalanceFlag        = true;
	itsBeepWhenTypeUnbalancedFlag = true;

	itsSmartTabFlag = true;

	// create edit menu

	JXTextMenu* editMenu = AppendEditMenu(menuBar, true, true, true, true, true, true, false, false);
	const JSize editCount = editMenu->GetItemCount();
	editMenu->ShowSeparatorAfter(editCount);

	itsExecScriptCmdIndex = editCount + 1;
	editMenu->AppendItem(JGetString("RunScript::CBTextEditor"), JXMenu::kPlainType, JString::empty,
						 JGetString("RunScriptShortcut::CBTextEditor"),
						 kExecScriptAction);
	editMenu->SetItemImage(itsExecScriptCmdIndex, jx_run_script);

	editMenu->AppendItem(JGetString("ScriptSubmenuTitle::CBTextEditor"));
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

	GetText()->SetDefaultFontStyle(prefsMgr->GetColor(CBPrefsManager::kTextColorIndex));

	SetBackColor(prefsMgr->GetColor(CBPrefsManager::kBackColorIndex));
	SetFocusColor(GetBackColor());

	SetCaretColor(prefsMgr->GetColor(CBPrefsManager::kCaretColorIndex));
	SetSelectionColor(prefsMgr->GetColor(CBPrefsManager::kSelColorIndex));
	SetSelectionOutlineColor(prefsMgr->GetColor(CBPrefsManager::kSelLineColorIndex));

	// right margin

	itsDrawRightMarginFlag = false;
	itsRightMarginWidth    = 80;
	itsRightMarginColor    = prefsMgr->GetColor(CBPrefsManager::kRightMarginColorIndex);
	SetWhitespaceColor(itsRightMarginColor);

	// misc setup

	UpdateTabHandling();
	ListenTo(this);

	scrollbarSet->GetVScrollbar()->SetScrollDelay(0);

	GetText()->SetLastSaveLocation();
	GetText()->ShouldAutoIndent(true);
	CBShouldAllowDragAndDrop(true);	// new users expect it
	ShouldMoveToFrontOfText(true);

	itsLineInput->ShareEditMenu(this);
	itsColInput->ShareEditMenu(this);

	GetText()->SetCharacterInWordFunction([this] (const JUtf8Character& c)
		{
		return CBMIsCharacterInWord(c) ||
					CBIsCharacterInWord(this->itsDoc->GetFileType(), c);
		});

	JTEKeyHandler* handler;
	CBInstallEmulator(CBGetPrefsManager()->GetEmulator(), this, &handler);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTextEditor::~CBTextEditor()
{
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
		bool autoIndent;
		input >> tabCharCount >> JBoolFromString(autoIndent);
		SetTabCharCount(tabCharCount);
		GetText()->ShouldAutoIndent(autoIndent);

		if (vers >= 2)
			{
			bool allowDND, moveFrontOfText;
			JSize undoDepth;
			input >> JBoolFromString(allowDND) >> JBoolFromString(moveFrontOfText) >> undoDepth;

			if (vers == 2)
				{
				// It actually read first digit of font name,
				// so we reset it.
				undoDepth = 100;
				}

			CBShouldAllowDragAndDrop(allowDND);
			ShouldMoveToFrontOfText(moveFrontOfText);
			GetText()->SetUndoDepth(undoDepth);
			}

		if (vers >= 4)
			{
			JSize lineWidth;
			input >> lineWidth;
			GetText()->SetCRMLineWidth(lineWidth);

			input >> JBoolFromString(itsBalanceWhileTypingFlag);
			input >> JBoolFromString(itsScrollToBalanceFlag);
			input >> JBoolFromString(itsBeepWhenTypeUnbalancedFlag);
			}

		if (vers == 5)
			{
			bool newLineAfterSemiFlag;
			input >> JBoolFromString(newLineAfterSemiFlag);
			}

		if (vers >= 7)
			{
			input >> JBoolFromString(itsSmartTabFlag);
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
			bool tabToSpaces;
			input >> JBoolFromString(tabToSpaces);
			GetText()->TabShouldInsertSpaces(tabToSpaces);
			}

		if (vers >= 11)
			{
			input >> JBoolFromString(itsDrawRightMarginFlag) >> itsRightMarginWidth;
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
	output << ' ' << JBoolToString(GetText().WillAutoIndent());
	output << ' ' << JBoolToString(AllowsDragAndDrop());
	output << ' ' << JBoolToString(WillMoveToFrontOfText());
	output << ' ' << GetText().GetUndoDepth();
	output << ' ' << GetText().GetCRMLineWidth();
	output << ' ' << JBoolToString(itsBalanceWhileTypingFlag);
	output << ' ' << JBoolToString(itsScrollToBalanceFlag);
	output << ' ' << JBoolToString(itsBeepWhenTypeUnbalancedFlag);
	output << ' ' << JBoolToString(itsSmartTabFlag);
	output << ' ' << JBoolToString(GetText().TabInsertsSpaces());
	output << ' ' << JBoolToString(itsDrawRightMarginFlag);
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
		const auto* info =
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
		}

	else if (message.Is(JXFSDirMenu::kFileSelected))
		{
		const auto* info =
			dynamic_cast<const JXFSDirMenu::FileSelected*>(&message);
		assert( info != nullptr );
		bool onDisk;
		const JString fullName = itsDoc->GetFullName(&onDisk);
		if (onDisk)
			{
			(CBGetRunTEScriptDialog())->RunSimpleScript(info->GetFileName(), this, fullName);
			}
		}

	else
		{
		JXTextMenu* editMenu;
		bool ok = GetEditMenu(&editMenu);
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
			const auto* selection =
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
			const auto* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			if (HandleCustomSearchMenuItems(selection->GetIndex()))
				{
				return;
				}
			}

		else if (sender == this &&
				 (message.Is(JStyledText::kTextSet) ||
				  message.Is(JStyledText::kTextChanged)))
			{
			if (itsFnMenu != nullptr)
				{
				itsFnMenu->TextChanged(itsDoc->GetFileType(), JString::empty);
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
	const bool ok = GetEditMenu(&editMenu);
	assert( ok );

	if (GetType() == kFullEditor)
		{
		editMenu->EnableItem(itsExecScriptCmdIndex);
		editMenu->EnableItem(itsExecScriptCmdIndex+1);	// script menu
		}

	editMenu->SetItemText(itsExecScriptCmdIndex,
		JGetString(HasSelection() ? "RunScriptOnSelection::CBTextEditor" : "RunScript::CBTextEditor"));
}

/******************************************************************************
 HandleCustomEditMenuItems (private)

	Returns true if it is one of our items.

 ******************************************************************************/

bool
CBTextEditor::HandleCustomEditMenuItems
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	if (index == itsExecScriptCmdIndex)
		{
		(CBGetRunTEScriptDialog())->Activate();
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 UpdateCustomSearchMenuItems (private)

 ******************************************************************************/

void
CBTextEditor::UpdateCustomSearchMenuItems()
{
	JXTextMenu* searchMenu;
	const bool ok = GetSearchReplaceMenu(&searchMenu);
	assert( ok );

	searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToLineCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kGoToColCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kPlaceBookmarkCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kFindSourceFileCmd);
	searchMenu->EnableItem(itsFirstSearchMenuItem + kViewManPageCmd);

	if (!GetText()->IsEmpty())
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
		const JUtf8Byte* map[] =
			{
			"p", projDoc->GetName().GetBytes()
			};
		searchMenu->SetItemText(itsFirstSearchMenuItem + kFindSelectionAsSymbolCmd,
			JGetString("FindSymbolGlobal::CBTextEditor", map, sizeof(map)));
		}
	else
		{
		searchMenu->SetItemText(itsFirstSearchMenuItem + kFindSelectionAsSymbolCmd, JGetString("FindSymbol::CBTextEditor"));
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

bool
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
		return true;
		}

	else if (index == kGoToLineCmd)
		{
		itsLineInput->Focus();
		return true;
		}
	else if (index == kGoToColCmd)
		{
		itsColInput->Focus();
		return true;
		}
	else if (index == kPlaceBookmarkCmd)
		{
		PlaceBookmark();
		return true;
		}

	else if (index == kOpenSelectionAsFileCmd)
		{
		OpenSelection();
		return true;
		}
	else if (index == kFindSourceFileCmd)
		{
		CBGetFindFileDialog()->Activate();
		return true;
		}

	else if (index == kFindSelectionAsSymbolCmd)
		{
		FindSelectedSymbol(kJXLeftButton, true);
		return true;
		}
	else if (index == kFindSelectionAsSymbolNoContextCmd)
		{
		FindSelectedSymbol(kJXLeftButton, false);
		return true;
		}
	else if (index == kFindSelectionInManPageCmd)
		{
		DisplayManPage();
		return true;
		}
	else if (index == kViewManPageCmd)
		{
		CBGetViewManPageDialog()->Activate();
		return true;
		}

	else if (index == kOpenPrevListItemCmd)
		{
		CBExecOutputDocument* listDoc;
		if (CBGetDocumentManager()->GetActiveListDocument(&listDoc))
			{
			listDoc->OpenPrevListItem();
			}
		return true;
		}
	else if (index == kOpenNextListItemCmd)
		{
		CBExecOutputDocument* listDoc;
		if (CBGetDocumentManager()->GetActiveListDocument(&listDoc))
			{
			listDoc->OpenNextListItem();
			}
		return true;
		}

	else
		{
		return false;
		}
}

/******************************************************************************
 PlaceBookmark (private)

 ******************************************************************************/

void
CBTextEditor::PlaceBookmark()
{
	JXScrollbar *hScrollbar, *vScrollbar;
	const bool hasScroll = GetScrollbars(&hScrollbar, &vScrollbar);
	assert( hasScroll );
	vScrollbar->PlaceScrolltab();
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
CBTextEditor::UpdateContextMenu()
{
	const bool hasSelection = HasSelection();
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
		JStyledText::TextRange r;
		if (GetSelection(&r))
			{
			SetCaretLocation(r.GetAfter());
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
		JStyledText::TextRange r;
		if (GetSelection(&r))
			{
			SetCaretLocation(r.GetAfter());
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
		FindSelectedSymbol(kJXLeftButton, true);
		}
	else if (index == kContextFindSymbolNoContextCmd)
		{
		SelectWordIfNoSelection();
		FindSelectedSymbol(kJXLeftButton, false);
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
		bool onDisk;
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
		JStyledText::TextRange r;
		TEGetDoubleClickSelection(GetInsertionIndex(), false, false, &r);
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
			(itsRightMarginWidth * GetText()->GetDefaultFont().GetCharWidth(GetFontManager(), JUtf8Character(' ')));

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
	const bool selectionSpecial =
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
	itsColInput->ShouldOptimizeUpdate(true);

	const bool selectionSpecial =
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
		itsColInput->ShouldOptimizeUpdate(false);
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
		itsContextMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
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
	auto* menu =
		jnew CBTEScriptMenu(this, itsScriptPath, parent, index, parent->GetEnclosure());
	assert( menu != nullptr );
	ListenTo(menu);

	JXTextMenu* editMenu;
	bool ok = GetEditMenu(&editMenu);
	assert( ok );
	const JXImage* image;
	ok = editMenu->GetItemImage(itsExecScriptCmdIndex, &image);
	assert( ok );
	menu->SetExecIcon(*image);

	menu->SetEmptyMessage(JGetString("EmptyScriptMenu::CBTextEditor"));
	return menu;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBTextEditor::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	CBGetDocumentManager()->SetActiveTextDocument(itsDoc);

	const bool controlOn = modifiers.control();
	const bool metaOn    = modifiers.meta();
	const bool shiftOn   = modifiers.shift();

	CBStringCompleter* completer = nullptr;
	bool clearRequestCount   = true;

	CBMacroManager* macroMgr = nullptr;
	CaretLocation caretIndex;

	const Type type = GetType();

	if (c == '\t' && controlOn && !metaOn && !shiftOn)
		{
		// We have to do this here because JXWindow can only be persuaded
		// to send it to the widget with focus, not as a shortcut.

		itsDoc->HandleActionButton();
		}

	else if (keySym == XK_F12)
		{
		if (!HasSelection())
			{
			JStyledText::TextRange r;
			TEGetDoubleClickSelection(GetInsertionIndex(), false, false, &r);
			SetSelection(r);
			}
		FindSelectedSymbol(shiftOn ? kJXMiddleButton : kJXLeftButton, true);
		}

	else if (controlOn && (c == kJUpArrow || c == kJDownArrow) && !metaOn)
		{
		MoveCaretToEdge(c);
		}

	else if (type == kFullEditor &&
			 itsSmartTabFlag && c == '\t' && !shiftOn && !metaOn && !controlOn &&
			 GetCaretLocation(&caretIndex) &&
			 itsDoc->GetMacroManager(&macroMgr) &&
			 macroMgr->Perform(caretIndex.location, itsDoc))
		{
		// Perform() does the work
		}

	else if (type == kFullEditor &&
			 c == ' ' && controlOn && !shiftOn && !metaOn &&
			 GetCaretLocation(&caretIndex) &&
			 itsDoc->GetMacroManager(&macroMgr))
		{
		macroMgr->Perform(caretIndex.location, itsDoc);	// prevent other Ctrl-space behavior
		}

	else if (type == kFullEditor &&
			 itsSmartTabFlag && c == '\t' && !shiftOn && !metaOn && !controlOn &&
			 itsDoc->GetStringCompleter(&completer) &&
			 completer->Complete(this, itsCompletionMenu))
		{
		// Complete() does the work
		clearRequestCount = false;
		}

	else if (type == kFullEditor &&
			 c == ' ' && controlOn && !metaOn &&		// Ctrl-space & Ctrl-Shift-space
			 itsDoc->GetStringCompleter(&completer))
		{
		completer->Complete(this, itsCompletionMenu);	// prevent other Ctrl-space behavior
		clearRequestCount = false;
		}

	else
		{
		const JStyledText::CRMRuleList* ruleList = nullptr;
		if ((c == '\r' || c == '\n') && shiftOn &&
			GetText()->GetCRMRuleList(&ruleList))
			{
			GetText()->ClearCRMRuleList();
			}

		if (!(shiftOn && (c == kJUpArrow   || c == kJDownArrow ||
						  c == kJLeftArrow || c == kJRightArrow)))
			{
			itsColInput->ShouldOptimizeUpdate(true);
			}
		JXTEBase::HandleKeyPress(c, keySym, modifiers);
		itsColInput->ShouldOptimizeUpdate(false);

		if (ruleList != nullptr)
			{
			GetText()->SetCRMRuleList(const_cast<JStyledText::CRMRuleList*>(ruleList), false);
			}

		if (itsBalanceWhileTypingFlag &&
			!metaOn && !controlOn &&
			CBMIsCloseGroup(CBGetLanguage(itsDoc->GetFileType()), c) &&
			!HasSelection())
			{
			ShowBalancingOpenGroup();
			}

		CBCharActionManager* mgr;
		if (!metaOn && !controlOn && itsDoc->GetCharActionManager(&mgr))
			{
			mgr->Perform(c, itsDoc);
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
	if (CBGetFnMenuUpdater()->CanCreateMenu(type))
		{
		if (itsFnMenu == nullptr)
			{
			JXMenuBar* menuBar = itsDoc->GetMenuBar();

			itsFnMenu = jnew CBFunctionMenu(itsDoc, type, this, menuBar,
											kFixedLeft, kFixedTop, 0,0, 10,10);
			assert( itsFnMenu != nullptr );

			JXTextMenu* searchMenu;
			const bool ok = GetSearchReplaceMenu(&searchMenu);
			assert( ok );

			menuBar->InsertMenuAfter(searchMenu, itsFnMenu);
			}
		else
			{
			itsFnMenu->TextChanged(type, JString::empty);
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
		itsCompletionMenu = jnew JXStringCompletionMenu(this, true);
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
	const JString& name
	)
{
	SetWritable( !JFileExists(name) || JFileWritable(name) );
}

/******************************************************************************
 SetWritable

	Choose whether or not the buffer is editable.

 ******************************************************************************/

void
CBTextEditor::SetWritable
	(
	const bool writable
	)
{
	SetType(writable ? kFullEditor : kSelectableText);
}

/******************************************************************************
 UpdateTabHandling (private)

	If we are not editable, Tab might as well switch focus.

 ******************************************************************************/

void
CBTextEditor::UpdateTabHandling()
{
	WantInput(true, GetType() == kFullEditor, true);
}

/******************************************************************************
 FindSelectedSymbol (private)

	First search the symbol list, then the man pages.

 ******************************************************************************/

void
CBTextEditor::FindSelectedSymbol
	(
	const JXMouseButton button,
	const bool		useContext
	)
{
	JString str;
	CBProjectDocument* projDoc = nullptr;

	bool onDisk;
	JString fullName = itsDoc->GetFullName(&onDisk);
	if (!onDisk || !useContext)
		{
		fullName.Clear();
		}

	if (!GetSelection(&str) || str.Contains("\n") || str.GetCharacterCount() < 2)
		{
		return;
		}

	if (!CBGetDocumentManager()->GetActiveProjectDocument(&projDoc) ||
		!projDoc->GetSymbolDirector()->FindSymbol(str, fullName, button))
		{
		// If we can't find it anywhere else, check the man pages.
		DisplayManPage();
		}
}

/******************************************************************************
 DisplayManPage (private)

	If the selection is followed by "(c)", then run "man c <selection>".

 ******************************************************************************/

static const JRegex manRegex(
	"^([-_:a-zA-Z0-9]+)"								// name
	"(?:[[:space:]]*,[[:space:]]*[-_:a-zA-Z0-9]+)*"		// optional intervening names, e.g., A,B,C(3)
	"(?:[[:space:]]*\\(([a-zA-Z0-9]+)\\))?");			// optional section

void
CBTextEditor::DisplayManPage()
{
	JStyledText::TextRange sel;
	if (GetSelection(&sel))
		{
		const JString& text  = GetText()->GetText();
		const JStringMatch m = manRegex.Match(JString(text.GetBytes() + sel.byteRange.first-1, JString::kNoCopy), JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
			{
			CBManPageDocument::Create(nullptr, m.GetSubstring(1), m.GetSubstring(2));
			}
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

	JStyledText::TextRange sel;
	if (!GetSelection(&sel))
		{
		return;
		}

	JStringIterator* iter = GetText()->GetConstIterator(kJIteratorStartAtBeginning, JStyledText::TextIndex(1,1));

	JString str;
	JIndex lineIndex;
	if (!IsNonstdError(&str, &sel, &lineIndex))
		{
		lineIndex = 0;
		JUtf8Character c;

		// extend selection to entire file name

		JStyledText::TextIndex start = GetText()->GetWordStart(sel.GetLast(*GetText()));
		JStyledText::TextIndex end   = GetText()->GetWordEnd(start);

		iter->UnsafeMoveTo(kJIteratorStartBefore, start.charIndex, start.byteIndex);
		if (!iter->AtBeginning())
			{
			while (iter->Prev(&c))
				{
				// catch "#include </usr/junk.h>", "#include <../junk.h>"

				while ((c == ACE_DIRECTORY_SEPARATOR_CHAR || c == '.') && !iter->AtBeginning())
					{
					iter->Prev(&c);
					}

				// stop correctly for
				// <a href=mailto:jafl>
				// (MyClass.java:35)
				// <url>http://yahoo.com</url>

				if (c.IsSpace() || c == '"' || c == '\'' || c == '<' || c == '>' ||
					c == '=' || c == '(' || c == '[' || c == '{')
					{
					break;
					}
				}

			iter->SkipNext();
			}

		start =
			JStyledText::TextIndex(
				iter->GetNextCharacterIndex(),
				iter->GetNextByteIndex());

		iter->UnsafeMoveTo(kJIteratorStartAfter, end.charIndex, end.byteIndex);
		while (iter->Next(&c))
			{
			// catch "http://junk/>"

			while (c == ACE_DIRECTORY_SEPARATOR_CHAR && !iter->AtEnd())
				{
				iter->Next(&c);
				}

			// don't include line number for file:line
			// stop correctly for
			// (MyClass.java)
			// <url>http://yahoo.com</url>

			if (c.IsSpace() || c == '"' || c == '\'' || c == '>' || c == '<' ||
				c == ')' || c == ']' || c == '}')
				{
				break;
				}
			else if (c == ':' && iter->Next(&c, kJIteratorStay) && c.IsDigit())
				{
				lineIndex = GetLineIndex(
					JStyledText::TextIndex(
						iter->GetNextCharacterIndex(),
						iter->GetNextByteIndex()));
				break;
				}
			}

		iter->SkipPrev();

		sel =
			JStyledText::TextRange(
				start,
				JStyledText::TextIndex(
					iter->GetNextCharacterIndex(),
					iter->GetNextByteIndex()));

		iter->UnsafeMoveTo(kJIteratorStartBefore, sel.charRange.first, sel.byteRange.first);
		iter->BeginMatch();
		iter->UnsafeMoveTo(kJIteratorStartAfter, sel.charRange.last, sel.byteRange.last);
		str = iter->FinishMatch().GetString();
		}

	GetText()->DisposeConstIterator(iter);
	iter = nullptr;

	SetSelection(sel);
	GetWindow()->Update();				// show selection while we work

	if (str.Contains("\n"))
		{
		return;
		}

	if (urlPrefixRegex.Match(str))
		{
		JXGetWebBrowser()->ShowURL(str);
		}
	else
		{
		itsDoc->ConvertSelectionToFullPath(&str);
		CBGetApplication()->FindAndViewFile(str, JIndexRange(lineIndex, lineIndex));
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
	const JStyledText::TextIndex& startIndex
	)
	const
{
	JStringIterator* iter = GetText().GetConstIterator(kJIteratorStartBefore, startIndex);
	JUtf8Character c;

	iter->BeginMatch();

	while (iter->Next(&c))
		{
		if (!c.IsDigit())
			{
			iter->SkipPrev();
			break;
			}
		}

	const JString str = iter->FinishMatch().GetString();
	GetText().DisposeConstIterator(iter);
	iter = nullptr;

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

static const JRegex flexErrorRegex   = "\"(.+)\", line ([0-9]+)(\\.[0-9]+)?: ";
static const JRegex bisonErrorRegex  = "\\(\"(.+)\", line ([0-9]+)\\) error: ";
static const JRegex absoftErrorRegex = " error on line ([0-9]+) of ([^:]+): ";
static const JRegex mavenErrorRegex  = "(?:\\[[^]]+\\]\\s+)?([^:]+):\\[([0-9]+),";

bool
cbExtractFileAndLine
	(
	const JStyledText::TextIndex&	caretIndex,
	const JStyledText::TextIndex&	startIndex,
	const JString&					line,
	const JRegex&					pattern,
	const JIndex					fileSubindex,
	const JIndex					lineSubindex,
	JString*						fileName,
	JStyledText::TextRange*			fileNameRange,
	JIndex*							lineIndex
	)
{
	const JStringMatch match = pattern.Match(line, JRegex::kIncludeSubmatches);
	if (!match.IsEmpty() &&
		match.GetCharacterRange(fileSubindex).Contains(caretIndex.charIndex - startIndex.charIndex + 1))
		{
		*fileName = match.GetSubstring(1);

		*fileNameRange =
			JStyledText::TextRange(
				JCharacterRange(
					match.GetCharacterRange(fileSubindex) + (startIndex.charIndex - 1)),
				JUtf8ByteRange(
					match.GetUtf8ByteRange(fileSubindex)  + (startIndex.byteIndex - 1)));

		const bool ok = match.GetSubstring(lineSubindex).ConvertToUInt(lineIndex);
		assert( ok );

		return true;
		}
	else
		{
		return false;
		}
}

bool
CBTextEditor::IsNonstdError
	(
	JString*				fileName,
	JStyledText::TextRange*	fileNameRange,
	JIndex*					lineIndex
	)
	const
{
	const JStyledText::TextIndex caretIndex = GetInsertionIndex();
	const JStyledText::TextIndex startIndex = GetText().GetParagraphStart(caretIndex);
	const JStyledText::TextIndex endIndex   = GetText().GetParagraphEnd(caretIndex);

	JStringIterator* iter = GetText().GetConstIterator(kJIteratorStartBefore, startIndex);
	iter->BeginMatch();
	iter->UnsafeMoveTo(kJIteratorStartAfter, endIndex.charIndex, endIndex.byteIndex);
	const JString line = iter->FinishMatch().GetString();
	GetText().DisposeConstIterator(iter);
	iter = nullptr;

	return cbExtractFileAndLine(caretIndex, startIndex, line, flexErrorRegex, 1, 2,
							 fileName, fileNameRange, lineIndex) ||
		cbExtractFileAndLine(caretIndex, startIndex, line, bisonErrorRegex, 1, 2,
							 fileName, fileNameRange, lineIndex) ||
		cbExtractFileAndLine(caretIndex, startIndex, line, absoftErrorRegex, 2, 1,
							 fileName, fileNameRange, lineIndex) ||
		cbExtractFileAndLine(caretIndex, startIndex, line, mavenErrorRegex, 1, 2,
							 fileName, fileNameRange, lineIndex);
}

/******************************************************************************
 SetFont

	We do not check whether the font is different because CBEditTextPrefsDialog
	does this once, instead of each CBTextEditor doing it.

 ******************************************************************************/

void
CBTextEditor::SetFont
	(
	const JString&	name,
	const JSize		size,
	const JSize		tabCharCount,
	const bool	breakCROnly
	)
{
	PrivateSetTabCharCount(tabCharCount);
	SetAllFontNameAndSize(name, size,
		CalcTabWidth(JFontManager::GetFont(name, size), tabCharCount),
		breakCROnly, false);
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
			CalcTabWidth(GetText()->GetDefaultFont(), charCount));
		}
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

 ******************************************************************************/

CBTextEditor::StyledText::StyledText
	(
	CBTextDocument*	doc,
	JFontManager*	fontManager,
	const bool		pasteStyledText
	)
	:
	JXStyledText(true, pasteStyledText, fontManager),
	itsDoc(doc)
{
	itsTokenStartList = JSTStyler::NewTokenStartList();
}

CBTextEditor::StyledText::~StyledText()
{
	jdelete itsTokenStartList;
}

void
CBTextEditor::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&				text,
	JRunArray<JFont>*			styles,
	JStyledText::TextRange*		recalcRange,
	JStyledText::TextRange*		redrawRange,
	const bool					deletion
	)
{
	CBStylerBase* styler = nullptr;
	if (text.GetCharacterCount() < CBDocumentManager::kMinWarnFileSize &&
		itsDoc->GetStyler(&styler))
		{
		styles->SetBlockSize(2048);
		styler->UpdateStyles(this, text, styles,
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
	const JSize length = GetText()->GetText().GetCharacterCount();
	if (length == 0)
		{
		return;
		}

	CBStylerBase* styler;
	if (length < CBDocumentManager::kMinWarnFileSize &&
		itsDoc->GetStyler(&styler))
		{
		const JIndex saved = GetLineForChar(GetInsertionCharIndex());
		GetText()->RestyleAll();
		GoToLine(saved);
		}
	else
		{
		JFont font = GetText()->GetFont(1);
		font.ClearStyle();
		GetText()->SetFont(GetText()->SelectAll(), font, false);
		}
}

/******************************************************************************
 ShowBalancingOpenGroup (private)

 ******************************************************************************/

void
CBTextEditor::ShowBalancingOpenGroup()
{
	const CBLanguage lang = CBGetLanguage(itsDoc->GetFileType());

	const JStyledText::TextIndex origCaretIndex = GetInsertionIndex();

	JStringIterator* iter = GetText()->GetConstIterator(kJIteratorStartBefore, origCaretIndex);

	JUtf8Character openChar, closeChar;
	if (!iter->Prev(&closeChar) ||	// paranoia: must be the case unless something fails
		!CBMIsCloseGroup(lang, closeChar))
		{
		return;
		}

	if (CBMBalanceBackward(lang, iter, &openChar) &&
		CBMIsMatchingPair(lang, openChar, closeChar))
		{
		const JPoint savePt = GetAperture().topLeft();

		const JStyledText::TextIndex i(iter->GetNextCharacterIndex(), iter->GetNextByteIndex());
		SetSelection(JStyledText::TextRange(i, GetText()->AdjustTextIndex(i, +1)));

		if (!TEScrollToSelection(false) || itsScrollToBalanceFlag)
			{
			GetWindow()->Update();
			JWait(kBalanceWhileTypingDelay);
			}

		ScrollTo(savePt);
		}
	else if (itsBeepWhenTypeUnbalancedFlag)
		{
		GetDisplay()->Beep();
		}

	SetCaretLocation(origCaretIndex);
	GetText()->DisposeConstIterator(iter);
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
		SetFont(JString("Courier", JString::kNoCopy), fontSize, itsTabCharCount, false);
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
