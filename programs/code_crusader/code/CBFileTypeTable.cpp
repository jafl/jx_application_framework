/******************************************************************************
 CBFileTypeTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBFileTypeTable.h"
#include "cbGlobals.h"
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXInputField.h>
#include <JXColHeaderWidget.h>
#include <JXGetNewDirDialog.h>
#include <JDirInfo.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kSuffixColumn = 1,
	kTypeColumn,
	kMacroColumn,
	kScriptColumn,
	kCRMColumn,
	kWrapColumn,
	kEditCmdColumn
};

const JCoordinate kInitColWidth[] =
{
	100, 120, 75, 80, 70, 70, 150
};

static const JCharacter* kColTitle[] =
{
	"Suffix / Regex",
	"Type",
	"Macro set",
	"Script path",
	"CPM rules",
	"Word wrap",
	"Open command"
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

static const JCharacter* kWordWrapOnStr  = "Yes";
static const JCharacter* kWordWrapOffStr = "No";

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 3;
const JCharacter kGeometryDataEndDelimiter     = '\1';

	// version  3 stores script path column width
	// version  2 stores edit cmd column width
	// version  1 stores CRM column width

// Type menu

static const JCharacter* kTypeMenuStr =
	"  Adobe Flash %r"
	"| Ant %r"
	"| Assembly %r"
	"| ASP %r"
	"| AWK %r"
	"| Basic %r"
	"| Beta %r"
	"| Bison %r"
	"| Bourne shell (sh) %r"
	"| C shell (csh) %r"
	"| C/C++ source %r"
	"| C/C++ header %r"
	"| C# %r"
	"| Cobol %r"
	"| Eiffel %r"
	"| Erlang %r"
	"| FORTRAN %r"
	"| HTML %r"
	"| INI %r"
	"| Java archive %r"
	"| Java properties %r"
	"| Java source %r"
	"| JavaScript %r"
	"| JSP %r"
	"| Lex %r"
	"| Lisp %r"
	"| Lua %r"
	"| Make %r"
	"| Matlab %r"
	"| Modula-2 module %r"
	"| Modula-2 interface %r"
	"| Modula-3 module %r"
	"| Modula-3 interface %r"
	"| Pascal %r"
	"| Perl %r"
	"| PHP %r"
	"| Python %r"
	"| Ratfor %r"
	"| REXX %r"
	"| Ruby %r"
	"| Scheme %r"
	"| SLang %r"
	"| SML %r"
	"| SQL %r"
	"| TCL %r"
	"| Vera source %r"
	"| Vera header %r"
	"| Verilog %r"
	"| Vim %r"
	"| XML %r"
	"| Other source %r"
	"| Static library %r"
	"| Shared library %r"
	"| Documentation %r"
	"| Other text %r"
	"| Binary %r"
	"| External %r";

enum
{
	kAdobeFlexTypeCmd = 1,
	kAntTypeCmd,
	kAssemblyTypeCmd,
	kASPTypeCmd,
	kAWKTypeCmd,
	kBasicTypeCmd,
	kBetaTypeCmd,
	kBisonTypeCmd,
	kBourneShellTypeCmd,
	kCShellTypeCmd,
	kCSourceTypeCmd,
	kCHeaderTypeCmd,
	kCSharpTypeCmd,
	kCobolTypeCmd,
	kEiffelTypeCmd,
	kErlangTypeCmd,
	kFortranTypeCmd,
	kHTMLTypeCmd,
	kINITypeCmd,
	kJavaArchiveTypeCmd,
	kPropertiesTypeCmd,
	kJavaSourceTypeCmd,
	kJavaScriptTypeCmd,
	kJSPTypeCmd,
	kLexTypeCmd,
	kLispTypeCmd,
	kLuaTypeCmd,
	kMakeTypeCmd,
	kMatlabTypeCmd,
	kModula2ModuleTypeCmd,
	kModula2InterfaceTypeCmd,
	kModula3ModuleTypeCmd,
	kModula3InterfaceTypeCmd,
	kPascalTypeCmd,
	kPerlTypeCmd,
	kPHPTypeCmd,
	kPythonTypeCmd,
	kRatforTypeCmd,
	kREXXTypeCmd,
	kRubyTypeCmd,
	kSchemeTypeCmd,
	kSLangTypeCmd,
	kSMLTypeCmd,
	kSQLTypeCmd,
	kTCLTypeCmd,
	kVeraSourceTypeCmd,
	kVeraHeaderTypeCmd,
	kVerilogTypeCmd,
	kVimTypeCmd,
	kXMLTypeCmd,
	kOtherSourceTypeCmd,
	kStaticLibTypeCmd,
	kSharedLibTypeCmd,
	kDocumentationTypeCmd,
	kOtherTextTypeCmd,
	kBinaryTypeCmd,
	kExternalTypeCmd
};

static const CBTextFileType kMenuIndexToFileType[] =
{
	kCBAdobeFlexFT,
	kCBAntFT,
	kCBAssemblyFT,
	kCBASPFT,
	kCBAWKFT,
	kCBBasicFT,
	kCBBetaFT,
	kCBBisonFT,
	kCBBourneShellFT,
	kCBCShellFT,
	kCBCSourceFT,
	kCBCHeaderFT,
	kCBCSharpFT,
	kCBCobolFT,
	kCBEiffelFT,
	kCBErlangFT,
	kCBFortranFT,
	kCBHTMLFT,
	kCBINIFT,
	kCBJavaArchiveFT,
	kCBPropertiesFT,
	kCBJavaSourceFT,
	kCBJavaScriptFT,
	kCBJSPFT,
	kCBLexFT,
	kCBLispFT,
	kCBLuaFT,
	kCBMakeFT,
	kCBMatlabFT,
	kCBModula2ModuleFT,
	kCBModula2InterfaceFT,
	kCBModula3ModuleFT,
	kCBModula3InterfaceFT,
	kCBPascalFT,
	kCBPerlFT,
	kCBPHPFT,
	kCBPythonFT,
	kCBRatforFT,
	kCBREXXFT,
	kCBRubyFT,
	kCBSchemeFT,
	kCBSLangFT,
	kCBSMLFT,
	kCBSQLFT,
	kCBTCLFT,
	kCBVeraSourceFT,
	kCBVeraHeaderFT,
	kCBVerilogFT,
	kCBVimFT,
	kCBXMLFT,
	kCBOtherSourceFT,
	kCBStaticLibraryFT,
	kCBSharedLibraryFT,
	kCBDocumentationFT,
	kCBUnknownFT,
	kCBBinaryFT,
	kCBExternalFT
};

static const JIndex kFileTypeToMenuIndex[] =
{
	kOtherTextTypeCmd,
	kCSourceTypeCmd,
	kCHeaderTypeCmd,
	kOtherSourceTypeCmd,
	kDocumentationTypeCmd,
	kHTMLTypeCmd,
	kEiffelTypeCmd,
	kFortranTypeCmd,
	kJavaSourceTypeCmd,
	kStaticLibTypeCmd,
	kSharedLibTypeCmd,
	0,					// kCBExecOutputFT
	0,					// kCBManPageFT
	0,					// kCBDiffOutputFT
	kAssemblyTypeCmd,
	kPascalTypeCmd,
	kRatforTypeCmd,
	kExternalTypeCmd,
	kBinaryTypeCmd,
	kModula2ModuleTypeCmd,
	kModula2InterfaceTypeCmd,
	kModula3ModuleTypeCmd,
	kModula3InterfaceTypeCmd,
	kAWKTypeCmd,
	kCobolTypeCmd,
	kLispTypeCmd,
	kPerlTypeCmd,
	kPythonTypeCmd,
	kSchemeTypeCmd,
	kBourneShellTypeCmd,
	kTCLTypeCmd,
	kVimTypeCmd,
	kJavaArchiveTypeCmd,
	kPHPTypeCmd,
	kASPTypeCmd,
	0,					// kCBSearchOutputFT
	kMakeTypeCmd,
	kREXXTypeCmd,
	kRubyTypeCmd,
	kLexTypeCmd,
	kCShellTypeCmd,
	kBisonTypeCmd,
	kBetaTypeCmd,
	kLuaTypeCmd,
	kSLangTypeCmd,
	kSQLTypeCmd,
	kVeraSourceTypeCmd,
	kVeraHeaderTypeCmd,
	kVerilogTypeCmd,
	kCSharpTypeCmd,
	kErlangTypeCmd,
	kSMLTypeCmd,
	kJavaScriptTypeCmd,
	kAntTypeCmd,
	kJSPTypeCmd,
	kXMLTypeCmd,
	kBasicTypeCmd,
	0,					// kCBShellOutputFT
	kMatlabTypeCmd,
	kAdobeFlexTypeCmd,
	kINITypeCmd,
	kPropertiesTypeCmd
};

// Script menu

static const JCharacter* kScriptMenuStr    = "None %r %l";
static const JCharacter* kScriptMenuEndStr = "New directory";

enum
{
	kNoScriptPathCmd = 1,

	kNewScriptPathCmd = 0	// offset from end of menu
};

static const JCharacter* kUserScriptMarker = " (personal)";
static const JCharacter* kSysScriptMarker  = " (global)";

// string ID's

static const JCharacter* kNewDirTitleID       = "NewDirTitle::CBFileTypeTable";
static const JCharacter* kNewDirPromptID      = "NewDirPrompt::CBFileTypeTable";
static const JCharacter* kNewDirNoHomeID      = "NewDirNoHome::CBFileTypeTable";
static const JCharacter* kUnableToCreateDirID = "UnableToCreateDir::CBFileTypeTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileTypeTable::CBFileTypeTable
	(
	const JArray<CBPrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<CBPrefsManager::MacroSetInfo>&		macroList,
	const JArray<CBPrefsManager::CRMRuleListInfo>&	crmList,

	JXTextButton*		addTypeButton,
	JXTextButton*		removeTypeButton,
	JXTextButton*		duplicateTypeButton,
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
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsMacroList(macroList),
	itsCRMList(crmList),
	itsFont(GetFontManager()->GetDefaultFont())
{
	itsTextInput    = NULL;
	itsNewDirDialog = NULL;

	// font

	JString fontName;
	JSize fontSize;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont.Set(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		GetFontManager()->GetDefaultFont().GetLineHeight(),
		itsFont.GetLineHeight());
	SetDefaultRowHeight(rowHeight);

	// buttons

	itsAddTypeButton       = addTypeButton;
	itsRemoveTypeButton    = removeTypeButton;
	itsDuplicateTypeButton = duplicateTypeButton;

	ListenTo(itsAddTypeButton);
	ListenTo(itsRemoveTypeButton);
	ListenTo(itsDuplicateTypeButton);

	// type menu

	itsTypeMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsTypeMenu != NULL );
	itsTypeMenu->SetToHiddenPopupMenu();
	itsTypeMenu->SetMenuItems(kTypeMenuStr);
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTypeMenu);

	// macro menu

	itsMacroMenu = CreateMacroMenu();
	ListenTo(itsMacroMenu);

	// script menu

	itsScriptMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsScriptMenu != NULL );
	itsScriptMenu->SetToHiddenPopupMenu();
	itsScriptMenu->SetMenuItems(kScriptMenuStr);	// ensure non-empty
	itsScriptMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsScriptMenu);

	// CRM menu

	itsCRMMenu = CreateCRMMenu();
	ListenTo(itsCRMMenu);

	// regex for testing

	itsTestRegex = jnew JRegex(".");
	assert( itsTestRegex != NULL );

	// data

	itsFileTypeList = jnew JArray<CBPrefsManager::FileTypeInfo>(fileTypeList);
	assert( itsFileTypeList != NULL );
	FinishFileTypeListCopy(itsFileTypeList);
	itsFileTypeList->SetCompareFunction(CBPrefsManager::CompareFileTypeSpec);
	itsFileTypeList->Sort();

	for (JIndex i=1; i<=kColCount; i++)
		{
		AppendCols(1, kInitColWidth[i-1]);
		}

	AppendRows(itsFileTypeList->GetElementCount());

	UpdateButtons();
	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileTypeTable::~CBFileTypeTable()
{
	CleanOutFileTypeList(itsFileTypeList);
	jdelete itsFileTypeList;

	jdelete itsTestRegex;
}

/******************************************************************************
 CleanOutFileTypeList (private)

 ******************************************************************************/

void
CBFileTypeTable::CleanOutFileTypeList
	(
	JArray<CBPrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	const JSize count = fileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBPrefsManager::FileTypeInfo info = fileTypeList->GetElement(i);
		info.Free();
		}
}

/******************************************************************************
 GetFileTypeList

 ******************************************************************************/

void
CBFileTypeTable::GetFileTypeList
	(
	JArray<CBPrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	assert( !IsEditing() );

	CleanOutFileTypeList(fileTypeList);
	*fileTypeList = *itsFileTypeList;
	FinishFileTypeListCopy(fileTypeList);
}

/******************************************************************************
 FinishFileTypeListCopy (private)

 ******************************************************************************/

void
CBFileTypeTable::FinishFileTypeListCopy
	(
	JArray<CBPrefsManager::FileTypeInfo>* fileTypeList
	)
	const
{
	const JSize count = fileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBPrefsManager::FileTypeInfo info = fileTypeList->GetElement(i);

		info.suffix = jnew JString(*(info.suffix));
		assert( info.suffix != NULL );

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != NULL );

		if (info.scriptPath != NULL)
			{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != NULL );
			}

		if (info.editCmd != NULL)
			{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != NULL );
			}

		// create regex when it is not our own private list

		info.nameRegex    = NULL;
		info.contentRegex = NULL;

		if (fileTypeList != itsFileTypeList)
			{
			info.CreateRegex();
			}

		(info.literalRange).SetToNothing();		// assume text changed

		fileTypeList->SetElement(i, info);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CBFileTypeTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
		{
		return;
		}

	HilightIfSelected(p, cell, rect);

	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	if (cell.x == kSuffixColumn)
		{
		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *(info.suffix), JPainter::kHAlignLeft, JPainter::kVAlignCenter);

		p.SetFont(GetFontManager()->GetDefaultFont());
		}
	else if (cell.x == kTypeColumn)
		{
		const JString& str = itsTypeMenu->GetItemText(kFileTypeToMenuIndex[ info.type ]);
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kMacroColumn &&
			 info.type != kCBBinaryFT && info.type != kCBExternalFT)
		{
		const JString& str = itsMacroMenu->GetItemText(MacroIDToMenuIndex(info.macroID));
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kScriptColumn &&
			 info.type != kCBBinaryFT && info.type != kCBExternalFT)
		{
		if (info.scriptPath == NULL)
			{
			p.String(rect, "None", JPainter::kHAlignCenter, JPainter::kVAlignCenter);
			}
		else
			{
			p.String(rect, *(info.scriptPath), JPainter::kHAlignCenter, JPainter::kVAlignCenter);
			}
		}
	else if (cell.x == kCRMColumn &&
			 info.type != kCBBinaryFT && info.type != kCBExternalFT)
		{
		const JString& str = itsCRMMenu->GetItemText(CRMIDToMenuIndex(info.crmID));
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kWrapColumn &&
			 info.type != kCBBinaryFT && info.type != kCBExternalFT)
		{
		const JCharacter* str = info.wordWrap ? kWordWrapOnStr : kWordWrapOffStr;
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kEditCmdColumn && info.editCmd != NULL)
		{
		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *(info.editCmd), JPainter::kHAlignLeft, JPainter::kVAlignCenter);

		p.SetFont(GetFontManager()->GetDefaultFont());
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBFileTypeTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
		{
		s.SelectCell(cell);
		TableScrollToCell(cell);

		CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

		if (cell.x == kSuffixColumn)
			{
			BeginEditing(cell);
			}
		else if (cell.x == kTypeColumn)
			{
			itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else if (cell.x == kMacroColumn &&
				 info.type != kCBBinaryFT && info.type != kCBExternalFT)
			{
			itsMacroMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else if (cell.x == kScriptColumn &&
				 info.type != kCBBinaryFT && info.type != kCBExternalFT)
			{
			itsScriptMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else if (cell.x == kCRMColumn &&
				 info.type != kCBBinaryFT && info.type != kCBExternalFT)
			{
			itsCRMMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		else if (cell.x == kWrapColumn &&
				 info.type != kCBBinaryFT && info.type != kCBExternalFT)
			{
			info.wordWrap = !info.wordWrap;
			if (modifiers.meta())
				{
				const JBoolean wrap = info.wordWrap;
				const JSize count   = itsFileTypeList->GetElementCount();
				for (JIndex i=1; i<=count; i++)
					{
					info          = itsFileTypeList->GetElement(i);
					info.wordWrap = wrap;
					itsFileTypeList->SetElement(i, info);
					}
				TableRefreshCol(kWrapColumn);
				}
			else
				{
				itsFileTypeList->SetElement(cell.y, info);
				TableRefreshCell(cell);
				}
			}
		else if (cell.x == kEditCmdColumn && info.type == kCBExternalFT)
			{
			BeginEditing(cell);
			}
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

JBoolean
CBFileTypeTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	return JI2B( cell.x == kSuffixColumn ||
				(cell.x == kEditCmdColumn && info.editCmd != NULL));
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CBFileTypeTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == NULL );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != NULL );

	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	if (cell.x == kSuffixColumn)
		{
		itsTextInput->SetText(*(info.suffix));
		itsTextInput->SetIsRequired();
		}
	else if (cell.x == kEditCmdColumn)
		{
		assert( info.editCmd != NULL );
		itsTextInput->SetText(*(info.editCmd));
		}
	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
CBFileTypeTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != NULL );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	const JString& text               = itsTextInput->GetText();

	JBoolean ok = itsTextInput->InputValid();
	if (ok && cell.x == kSuffixColumn)
		{
		const JBoolean isRegex = JI2B(text.GetFirstCharacter() == kCBContentRegexMarker);
		if (isRegex && text.GetLength() == 1)
			{
			ok = kJFalse;
			(JGetUserNotification())->ReportError(
				"You must specify some text to match.");
			}
		else if (isRegex)
			{
			const JError err = itsTestRegex->SetPattern(text);
			err.ReportIfError();
			ok = err.OK();
			}

		if (ok)
			{
			*(info.suffix) = text;
			}
		}
	else if (ok && cell.x == kEditCmdColumn)
		{
		assert( info.editCmd != NULL );
		*(info.editCmd) = text;
		(info.editCmd)->TrimWhitespace();
		}

	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CBFileTypeTable::PrepareDeleteXInputField()
{
	itsTextInput = NULL;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBFileTypeTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddTypeButton && message.Is(JXButton::kPushed))
		{
		AddType();
		}
	else if (sender == itsRemoveTypeButton && message.Is(JXButton::kPushed))
		{
		RemoveType();
		}
	else if (sender == itsDuplicateTypeButton && message.Is(JXButton::kPushed))
		{
		DuplicateType();
		}

	else if (sender == itsTypeMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTypeMenu();
		}
	else if (sender == itsTypeMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleTypeMenu(selection->GetIndex());
		}

	else if (sender == itsMacroMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMacroMenu();
		}
	else if (sender == itsMacroMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleMacroMenu(selection->GetIndex());
		}

	else if (sender == itsScriptMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateScriptMenu();
		}
	else if (sender == itsScriptMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleScriptMenu(selection->GetIndex());
		}

	else if (sender == itsCRMMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCRMMenu();
		}
	else if (sender == itsCRMMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleCRMMenu(selection->GetIndex());
		}

	else if (sender == itsNewDirDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CreateNewScriptDirectory();
			}
		itsNewDirDialog = NULL;
		}

	else
		{
		if (sender == &(GetTableSelection()))
			{
			UpdateButtons();
			}

		JXEditTable::Receive(sender, message);
		}
}

/******************************************************************************
 AddType (private)

 ******************************************************************************/

void
CBFileTypeTable::AddType()
{
	if (EndEditing())
		{
		CBPrefsManager::FileTypeInfo info(jnew JString, NULL, NULL, kCBUnknownFT,
										  kCBEmptyMacroID, kCBEmptyCRMRuleListID,
										  kJTrue, NULL, kJTrue, jnew JString, NULL);
		assert( info.suffix != NULL && info.complSuffix != NULL );
		itsFileTypeList->AppendElement(info);
		AppendRows(1);
		BeginEditing(JPoint(kSuffixColumn, itsFileTypeList->GetElementCount()));
		}
}

/******************************************************************************
 RemoveType (private)

 ******************************************************************************/

void
CBFileTypeTable::RemoveType()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		CancelEditing();

		CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
		info.Free();

		itsFileTypeList->RemoveElement(cell.y);
		RemoveRow(cell.y);
		}
}

/******************************************************************************
 DuplicateType (private)

 ******************************************************************************/

void
CBFileTypeTable::DuplicateType()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell) && EndEditing())
		{
		CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

		info.suffix = jnew JString(*(info.suffix));
		assert( info.suffix != NULL );

		info.nameRegex    = NULL;
		info.contentRegex = NULL;

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != NULL );

		if (info.scriptPath != NULL)
			{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != NULL );
			}

		if (info.editCmd != NULL)
			{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != NULL );
			}

		itsFileTypeList->AppendElement(info);
		AppendRows(1);
		BeginEditing(JPoint(kSuffixColumn, itsFileTypeList->GetElementCount()));
		}
}

/******************************************************************************
 UpdateTypeMenu (private)

 ******************************************************************************/

void
CBFileTypeTable::UpdateTypeMenu()
{
	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	itsTypeMenu->CheckItem(kFileTypeToMenuIndex[ info.type ]);
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
CBFileTypeTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const CBTextFileType newType = kMenuIndexToFileType [ index-1 ];

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	if (newType != info.type)
		{
		info.type = newType;
		if (info.type == kCBExternalFT)
			{
			info.editCmd = jnew JString;
			assert( info.editCmd != NULL );
			BeginEditing(JPoint(kEditCmdColumn, cell.y));
			}
		else
			{
			jdelete info.editCmd;
			info.editCmd = NULL;
			}
		itsFileTypeList->SetElement(cell.y, info);

		TableRefreshRow(cell.y);

		// info.editCmd must not be NULL

		if (info.type == kCBExternalFT)
			{
			BeginEditing(JPoint(kEditCmdColumn, cell.y));
			}
		}
}

/******************************************************************************
 UpdateScriptMenu (private)

 ******************************************************************************/

void
CBFileTypeTable::UpdateScriptMenu()
{
	itsScriptMenu->SetMenuItems(kScriptMenuStr);

	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );
	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	if (info.scriptPath == NULL)
		{
		itsScriptMenu->CheckItem(kNoScriptPathCmd);
		}

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (CBPrefsManager::GetScriptPaths(&sysDir, &userDir))
		{
		BuildScriptMenuItems(sysDir,  kJFalse, &menuText);
		BuildScriptMenuItems(userDir, kJTrue,  &menuText);

		const JSize count = menuText.GetElementCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
			{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *(menuText.GetElement(i));

			JIndex j;
			const JBoolean found = itemText.LocateLastSubstring(" (", &j);
			assert( found );

			const JIndexRange r(j, itemText.GetLength());
			nmShortcut = itemText.GetSubstring(r);
			itemText.RemoveSubstring(r);

			itsScriptMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsScriptMenu->SetItemNMShortcut(itsScriptMenu->GetItemCount(), nmShortcut);

			// By not stripping off the leading whitespace on nmShorcut, we
			// can do a direct string comparison with the original strings.

			if (info.scriptPath != NULL && *(info.scriptPath) == itemText &&
				(( info.isUserScript && nmShortcut == kUserScriptMarker) ||
				 (!info.isUserScript && nmShortcut == kSysScriptMarker)))
				{
				itsScriptMenu->CheckItem(itsScriptMenu->GetItemCount());
				}
			}
		}

	itsScriptMenu->ShowSeparatorAfter(itsScriptMenu->GetItemCount());
	itsScriptMenu->AppendMenuItems(kScriptMenuEndStr);
}

/******************************************************************************
 BuildScriptMenuItems (private)

	Builds a sorted list of the directories below the given path.

 ******************************************************************************/

void
CBFileTypeTable::BuildScriptMenuItems
	(
	const JCharacter*	path,
	const JBoolean		isUserPath,
	JPtrArray<JString>*	menuText
	)
	const
{
	JDirInfo* info = NULL;
	if (JDirInfo::Create(path, &info))
		{
		info->ShowFiles(kJFalse);

		const JSize count = info->GetEntryCount();
		for (JIndex i=1; i<=count; i++)
			{
			JString* s = jnew JString((info->GetEntry(i)).GetName());
			assert( s != NULL );

			if (isUserPath)
				{
				*s += kUserScriptMarker;
				}
			else
				{
				*s += kSysScriptMarker;
				}

			menuText->InsertSorted(s);
			}
		}
}

/******************************************************************************
 HandleScriptMenu (private)

 ******************************************************************************/

void
CBFileTypeTable::HandleScriptMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	const JSize itemCount = itsScriptMenu->GetItemCount();
	if (index == kNoScriptPathCmd)
		{
		jdelete info.scriptPath;
		info.scriptPath = NULL;
		}
	else if (index == itemCount - kNewScriptPathCmd)
		{
		GetNewScriptDirectory();
		}
	else
		{
		if (info.scriptPath == NULL)
			{
			info.scriptPath = jnew JString(itsScriptMenu->GetItemText(index));
			assert( info.scriptPath != NULL );
			}
		else
			{
			*(info.scriptPath) = itsScriptMenu->GetItemText(index);
			}

		JString nmShortcut;
		const JBoolean ok = itsScriptMenu->GetItemNMShortcut(index, &nmShortcut);
		assert( ok );
		info.isUserScript = JI2B( nmShortcut == kUserScriptMarker );
		}

	itsFileTypeList->SetElement(cell.y, info);
	TableRefreshRow(cell.y);
}

/******************************************************************************
 GetNewScriptDirectory (private)

 ******************************************************************************/

void
CBFileTypeTable::GetNewScriptDirectory()
{
	assert( itsNewDirDialog == NULL );

	JString sysDir, userDir;
	if (!CBPrefsManager::GetScriptPaths(&sysDir, &userDir))
		{
		(JGetUserNotification())->ReportError(JGetString(kNewDirNoHomeID));
		return;
		}

	if (!CreateDirectory(userDir))
		{
		return;
		}

	itsNewDirDialog =
		jnew JXGetNewDirDialog(JXGetApplication(), JGetString(kNewDirTitleID),
							  JGetString(kNewDirPromptID), "", userDir);
	assert( itsNewDirDialog != NULL );
	itsNewDirDialog->BeginDialog();
	ListenTo(itsNewDirDialog);
}

/******************************************************************************
 CreateNewScriptDirectory (private)

 ******************************************************************************/

void
CBFileTypeTable::CreateNewScriptDirectory()
{
	assert( itsNewDirDialog != NULL );

	const JString fullName = itsNewDirDialog->GetNewDirName();
	CreateDirectory(fullName);

	JPoint cell;
	const JBoolean ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	if (info.scriptPath == NULL)
		{
		info.scriptPath = jnew JString(name);
		assert( info.scriptPath != NULL );
		}
	else
		{
		*(info.scriptPath) = name;
		}

	info.isUserScript = kJTrue;

	itsFileTypeList->SetElement(cell.y, info);
	TableRefreshRow(cell.y);
}

/******************************************************************************
 CreateDirectory (private)

 ******************************************************************************/

JBoolean
CBFileTypeTable::CreateDirectory
	(
	const JCharacter* path
	)
	const
{
	const JError err = JCreateDirectory(path);
	if (!err.OK())
		{
		const JCharacter* map[] =
			{
			"dir", path,
			"err", err.GetMessage()
			};
		const JString msg = JGetString(kUnableToCreateDirID, map, sizeof(map));
		(JGetUserNotification())->ReportError(msg);
		}
	return err.OK();
}

/******************************************************************************
 Macro menu functions (private)

 ******************************************************************************/

#define CreateMenuFn CreateMacroMenu
#define UpdateMenuFn UpdateMacroMenu
#define IDToIndexFn  MacroIDToMenuIndex
#define FindIDFn     FindMacroID
#define HandleMenuFn HandleMacroMenu
#define MenuVar      itsMacroMenu
#define DataList     itsMacroList
#define StructIDVar  macroID
#define EmptyDataID  kCBEmptyMacroID
#include "CBFTTHandleMenu.th"
#undef CreateMenuFn
#undef UpdateMenuFn
#undef IDToIndexFn
#undef FindIDFn
#undef HandleMenuFn
#undef MenuVar
#undef DataList
#undef StructIDVar
#undef EmptyDataID

/******************************************************************************
 CRM menu functions (private)

 ******************************************************************************/

#define CreateMenuFn CreateCRMMenu
#define UpdateMenuFn UpdateCRMMenu
#define IDToIndexFn  CRMIDToMenuIndex
#define FindIDFn     FindCRMRuleListID
#define HandleMenuFn HandleCRMMenu
#define MenuVar      itsCRMMenu
#define DataList     itsCRMList
#define StructIDVar  crmID
#define EmptyDataID  kCBEmptyCRMRuleListID
#include "CBFTTHandleMenu.th"
#undef CreateMenuFn
#undef UpdateMenuFn
#undef IDToIndexFn
#undef FindIDFn
#undef HandleMenuFn
#undef MenuVar
#undef DataList
#undef StructIDVar
#undef EmptyDataID

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CBFileTypeTable::UpdateButtons()
{
	if ((GetTableSelection()).HasSelection())
		{
		itsRemoveTypeButton->Activate();
		itsDuplicateTypeButton->Activate();
		}
	else
		{
		itsRemoveTypeButton->Deactivate();
		itsDuplicateTypeButton->Deactivate();
		}
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
CBFileTypeTable::ReadGeometry
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
		{
		JCoordinate w;
		input >> w;
		SetColWidth(kSuffixColumn, w);
		input >> w;
		SetColWidth(kTypeColumn, w);
		input >> w;
		SetColWidth(kMacroColumn, w);
		if (vers >= 3)
			{
			input >> w;
			SetColWidth(kScriptColumn, w);
			}
		if (vers >= 1)
			{
			input >> w;
			SetColWidth(kCRMColumn, w);
			}
		input >> w;
		SetColWidth(kWrapColumn, w);
		if (vers >= 2)
			{
			input >> w;
			SetColWidth(kEditCmdColumn, w);
			}
		}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
CBFileTypeTable::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kSuffixColumn);
	output << ' ' << GetColWidth(kTypeColumn);
	output << ' ' << GetColWidth(kMacroColumn);
	output << ' ' << GetColWidth(kScriptColumn);
	output << ' ' << GetColWidth(kCRMColumn);
	output << ' ' << GetColWidth(kWrapColumn);
	output << ' ' << GetColWidth(kEditCmdColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
CBFileTypeTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		widget->SetColTitle(i, kColTitle[i-1]);
		}
}
