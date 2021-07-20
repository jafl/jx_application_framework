/******************************************************************************
 CBFileTypeTable.cpp

	BASE CLASS = JXEditTable

	Copyright © 1998 by John Lindal.

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
#include <JStringIterator.h>
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

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 3;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

	// version  3 stores script path column width
	// version  2 stores edit cmd column width
	// version  1 stores CRM column width

// Type menu

static const JUtf8Byte* kTypeMenuStr =
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
	"| D %d"
	"| Eiffel %r"
	"| Erlang %r"
	"| FORTRAN %r"
	"| Go %r"
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
	kDTypeCmd,
	kEiffelTypeCmd,
	kErlangTypeCmd,
	kFortranTypeCmd,
	kGoTypeCmd,
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
	kCBDFT,
	kCBEiffelFT,
	kCBErlangFT,
	kCBFortranFT,
	kCBGoFT,
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
	kPropertiesTypeCmd,
	kGoTypeCmd,
	kDTypeCmd
};

// Script menu

static const JUtf8Byte* kScriptMenuStr    = "None %r %l";
static const JUtf8Byte* kScriptMenuEndStr = "New directory";

enum
{
	kNoScriptPathCmd = 1,

	kNewScriptPathCmd = 0	// offset from end of menu
};

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
	itsFont(JFontManager::GetDefaultFont())
{
	itsTextInput    = nullptr;
	itsNewDirDialog = nullptr;

	// font

	JString fontName;
	JSize fontSize;
	CBGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont.Set(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()),
		itsFont.GetLineHeight(GetFontManager()));
	SetDefaultRowHeight(rowHeight);

	// buttons

	itsAddTypeButton       = addTypeButton;
	itsRemoveTypeButton    = removeTypeButton;
	itsDuplicateTypeButton = duplicateTypeButton;

	ListenTo(itsAddTypeButton);
	ListenTo(itsRemoveTypeButton);
	ListenTo(itsDuplicateTypeButton);

	// type menu

	itsTypeMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsTypeMenu != nullptr );
	itsTypeMenu->SetToHiddenPopupMenu();
	itsTypeMenu->SetMenuItems(kTypeMenuStr);
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTypeMenu);

	// macro menu

	itsMacroMenu = CreateMacroMenu();
	ListenTo(itsMacroMenu);

	// script menu

	itsScriptMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsScriptMenu != nullptr );
	itsScriptMenu->SetToHiddenPopupMenu();
	itsScriptMenu->SetMenuItems(kScriptMenuStr);	// ensure non-empty
	itsScriptMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsScriptMenu);

	// CRM menu

	itsCRMMenu = CreateCRMMenu();
	ListenTo(itsCRMMenu);

	// regex for testing

	itsTestRegex = jnew JRegex(".");
	assert( itsTestRegex != nullptr );

	// data

	itsFileTypeList = jnew JArray<CBPrefsManager::FileTypeInfo>(fileTypeList);
	assert( itsFileTypeList != nullptr );
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
		assert( info.suffix != nullptr );

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != nullptr );

		if (info.scriptPath != nullptr)
			{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != nullptr );
			}

		if (info.editCmd != nullptr)
			{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != nullptr );
			}

		// create regex when it is not our own private list

		info.nameRegex    = nullptr;
		info.contentRegex = nullptr;

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

		p.SetFont(JFontManager::GetDefaultFont());
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
		if (info.scriptPath == nullptr)
			{
			p.String(rect, JGetString("NoScript::CBFileTypeTable"), JPainter::kHAlignCenter, JPainter::kVAlignCenter);
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
		p.String(rect,
			JGetString(info.wordWrap ? "WordWrapOn::CBFileTypeTable" : "WordWrapOff::CBFileTypeTable"),
			JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kEditCmdColumn && info.editCmd != nullptr)
		{
		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *(info.editCmd), JPainter::kHAlignLeft, JPainter::kVAlignCenter);

		p.SetFont(JFontManager::GetDefaultFont());
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
				const bool wrap = info.wordWrap;
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

bool
CBFileTypeTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	return cell.x == kSuffixColumn ||
				(cell.x == kEditCmdColumn && info.editCmd != nullptr);
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
	assert( itsTextInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != nullptr );

	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	if (cell.x == kSuffixColumn)
		{
		itsTextInput->GetText()->SetText(*info.suffix);
		itsTextInput->SetIsRequired();
		}
	else if (cell.x == kEditCmdColumn)
		{
		assert( info.editCmd != nullptr );
		itsTextInput->GetText()->SetText(*info.editCmd);
		}
	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
CBFileTypeTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	const JString& text               = itsTextInput->GetText()->GetText();

	bool ok = itsTextInput->InputValid();
	if (ok && cell.x == kSuffixColumn)
		{
		const bool isRegex = text.GetFirstCharacter() == kCBContentRegexMarker;
		if (isRegex && text.GetCharacterCount() == 1)
			{
			ok = false;
			JGetUserNotification()->ReportError(
				JGetString("EmptyInput::CBFileTypeTable"));
			}
		else if (isRegex)
			{
			const JError err = itsTestRegex->SetPattern(text);
			err.ReportIfError();
			ok = err.OK();
			}

		if (ok)
			{
			*info.suffix = text;
			}
		}
	else if (ok && cell.x == kEditCmdColumn)
		{
		assert( info.editCmd != nullptr );
		*info.editCmd = text;
		info.editCmd->TrimWhitespace();
		}

	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CBFileTypeTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleTypeMenu(selection->GetIndex());
		}

	else if (sender == itsMacroMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMacroMenu();
		}
	else if (sender == itsMacroMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleMacroMenu(selection->GetIndex());
		}

	else if (sender == itsScriptMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateScriptMenu();
		}
	else if (sender == itsScriptMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleScriptMenu(selection->GetIndex());
		}

	else if (sender == itsCRMMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCRMMenu();
		}
	else if (sender == itsCRMMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCRMMenu(selection->GetIndex());
		}

	else if (sender == itsNewDirDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CreateNewScriptDirectory();
			}
		itsNewDirDialog = nullptr;
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
		CBPrefsManager::FileTypeInfo info(jnew JString, nullptr, nullptr, kCBUnknownFT,
										  kCBEmptyMacroID, kCBEmptyCRMRuleListID,
										  true, nullptr, true, jnew JString, nullptr);
		assert( info.suffix != nullptr && info.complSuffix != nullptr );
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
		assert( info.suffix != nullptr );

		info.nameRegex    = nullptr;
		info.contentRegex = nullptr;

		info.complSuffix = jnew JString(*(info.complSuffix));
		assert( info.complSuffix != nullptr );

		if (info.scriptPath != nullptr)
			{
			info.scriptPath = jnew JString(*(info.scriptPath));
			assert( info.scriptPath != nullptr );
			}

		if (info.editCmd != nullptr)
			{
			info.editCmd = jnew JString(*(info.editCmd));
			assert( info.editCmd != nullptr );
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
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
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
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	const CBTextFileType newType = kMenuIndexToFileType [ index-1 ];

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);
	if (newType != info.type)
		{
		info.type = newType;
		if (info.type == kCBExternalFT)
			{
			info.editCmd = jnew JString;
			assert( info.editCmd != nullptr );
			BeginEditing(JPoint(kEditCmdColumn, cell.y));
			}
		else
			{
			jdelete info.editCmd;
			info.editCmd = nullptr;
			}
		itsFileTypeList->SetElement(cell.y, info);

		TableRefreshRow(cell.y);

		// info.editCmd must not be nullptr

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
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );
	const CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	if (info.scriptPath == nullptr)
		{
		itsScriptMenu->CheckItem(kNoScriptPathCmd);
		}

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (CBPrefsManager::GetScriptPaths(&sysDir, &userDir))
		{
		BuildScriptMenuItems(sysDir,  false, &menuText);
		BuildScriptMenuItems(userDir, true,  &menuText);

		const JSize count = menuText.GetElementCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
			{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *menuText.GetElement(i);

			JStringIterator iter(&itemText, kJIteratorStartAtEnd);
			iter.BeginMatch();
			const bool found = iter.Prev(" (");
			assert( found );
			const JStringMatch& m = iter.FinishMatch(true);

			nmShortcut = m.GetString();
			iter.RemoveAllNext();
			iter.Invalidate();

			itsScriptMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsScriptMenu->SetItemNMShortcut(itsScriptMenu->GetItemCount(), nmShortcut);

			// By not stripping off the leading whitespace on nmShorcut, we
			// can do a direct string comparison with the original strings.

			if (info.scriptPath != nullptr && *info.scriptPath == itemText &&
				(( info.isUserScript && nmShortcut == JGetString("UserScriptMarker::CBFileTypeTable")) ||
				 (!info.isUserScript && nmShortcut == JGetString("SysScriptMarker::CBFileTypeTable"))))
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
	const JString&		path,
	const bool		isUserPath,
	JPtrArray<JString>*	menuText
	)
	const
{
	JDirInfo* info = nullptr;
	if (JDirInfo::Create(path, &info))
		{
		info->ShowFiles(false);

		const JSize count = info->GetEntryCount();
		for (JIndex i=1; i<=count; i++)
			{
			auto* s = jnew JString(info->GetEntry(i).GetName());
			assert( s != nullptr );

			if (isUserPath)
				{
				*s += JGetString("UserScriptMarker::CBFileTypeTable");
				}
			else
				{
				*s += JGetString("SysScriptMarker::CBFileTypeTable");
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
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	const JSize itemCount = itsScriptMenu->GetItemCount();
	if (index == kNoScriptPathCmd)
		{
		jdelete info.scriptPath;
		info.scriptPath = nullptr;
		}
	else if (index == itemCount - kNewScriptPathCmd)
		{
		GetNewScriptDirectory();
		}
	else
		{
		if (info.scriptPath == nullptr)
			{
			info.scriptPath = jnew JString(itsScriptMenu->GetItemText(index));
			assert( info.scriptPath != nullptr );
			}
		else
			{
			*(info.scriptPath) = itsScriptMenu->GetItemText(index);
			}

		JString nmShortcut;
		const bool ok = itsScriptMenu->GetItemNMShortcut(index, &nmShortcut);
		assert( ok );
		info.isUserScript = nmShortcut == JGetString("UserScriptMarker::CBFileTypeTable");
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
	assert( itsNewDirDialog == nullptr );

	JString sysDir, userDir;
	if (!CBPrefsManager::GetScriptPaths(&sysDir, &userDir))
		{
		JGetUserNotification()->ReportError(JGetString("NewDirNoHome::CBFileTypeTable"));
		return;
		}

	if (!CreateDirectory(userDir))
		{
		return;
		}

	itsNewDirDialog =
		jnew JXGetNewDirDialog(JXGetApplication(), JGetString("NewDirTitle::CBFileTypeTable"),
							  JGetString("NewDirPrompt::CBFileTypeTable"), JString::empty, userDir);
	assert( itsNewDirDialog != nullptr );
	itsNewDirDialog->BeginDialog();
	ListenTo(itsNewDirDialog);
}

/******************************************************************************
 CreateNewScriptDirectory (private)

 ******************************************************************************/

void
CBFileTypeTable::CreateNewScriptDirectory()
{
	assert( itsNewDirDialog != nullptr );

	const JString fullName = itsNewDirDialog->GetNewDirName();
	CreateDirectory(fullName);

	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	CBPrefsManager::FileTypeInfo info = itsFileTypeList->GetElement(cell.y);

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	if (info.scriptPath == nullptr)
		{
		info.scriptPath = jnew JString(name);
		assert( info.scriptPath != nullptr );
		}
	else
		{
		*(info.scriptPath) = name;
		}

	info.isUserScript = true;

	itsFileTypeList->SetElement(cell.y, info);
	TableRefreshRow(cell.y);
}

/******************************************************************************
 CreateDirectory (private)

 ******************************************************************************/

bool
CBFileTypeTable::CreateDirectory
	(
	const JString& path
	)
	const
{
	const JError err = JCreateDirectory(path);
	if (!err.OK())
		{
		const JUtf8Byte* map[] =
			{
			"dir", path.GetBytes(),
			"err", err.GetMessage().GetBytes()
			};
		const JString msg = JGetString("UnableToCreateDir::CBFileTypeTable", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
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
	widget->SetColumnTitles("CBFileTypeTable", kColCount);
}
