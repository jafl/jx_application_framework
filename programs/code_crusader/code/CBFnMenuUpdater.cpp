/******************************************************************************
 CBFnMenuUpdater.cpp

	Uses ctags to extract function names from a source file and build
	a menu to allow the user to jump to their definitions.

	BASE CLASS = JPrefObject, CBCtagsUser

	Copyright © 1999-2001 by John Lindal.

 ******************************************************************************/

#include "CBFnMenuUpdater.h"
#include "cbmUtil.h"
#include "cbCtagsRegex.h"
#include <JXTextMenu.h>
#include <jXConstants.h>
#include <JPtrArray-JString.h>
#include <JListUtil.h>
#include <JFontManager.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// sort=no requires so qualified tag comes after unqualified version

static const JUtf8Byte* kCtagsArgs =
	"--format=1 --excmd=number --sort=no --extras=q "
	"--ant-kinds=t "
	"--asm-kinds=l "
	"--asp-kinds=fs "
	"--awk-kinds=f "
	"--beta-kinds=f "
	CBCtagsBisonDef
	CBCtagsBisonNonterminalDef
	"--c-kinds=f "
	"--cobol-kinds=p "
	"--eiffel-kinds=f "
	"--fortran-kinds=psfl "
	CBCtagsHTMLDef
	CBCtagsHTMLID
	"--java-kinds=m "
	CBCtagsLexDef
	CBCtagsLexState
	"--lisp-kinds=f "
	"--lua-kinds=f "
	CBCtagsMakeDef
	CBCtagsMakeTarget
	"--pascal-kinds=fp "
	"--perl-kinds=s "
	"--php-kinds=fj "
	"--python-kinds=f "
	"--rexx-kinds=s "
	"--ruby-kinds=f "
	"--scheme-kinds=f "
	"--sh-kinds=f "
	"--slang-kinds=f "
	"--sql-kinds=fptvT "
	"--tcl-kinds=p "
	"--vera-kinds=f "
	"--verilog-kinds=f "
	"--vim-kinds=f "
	"--c#-kinds=m "
	"--erlang-kinds=f "
	"--sml-kinds=fc "
	"--javascript-kinds=fm "
	"--basic-kinds=f "
	"--matlab-kinds=f "
	"--flex-kinds=fm ";

// setup information

const JFileVersion kCurrentSetupVersion = 2;

// version  2 adds itsIncludeNSFlag
// version  1 adds itsPackFlag

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFnMenuUpdater::CBFnMenuUpdater()
	:
	JPrefObject(CBMGetPrefsManager(), kCBFnMenuUpdaterID),
	CBCtagsUser(kCtagsArgs)
{
	itsSortFlag      = true;
	itsIncludeNSFlag = false;
	itsPackFlag      = false;
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFnMenuUpdater::~CBFnMenuUpdater()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 UpdateMenu

 ******************************************************************************/

void
CBFnMenuUpdater::UpdateMenu
	(
	const JString&			fileName,
	const CBTextFileType	origFileType,
	const bool			sort,
	const bool			includeNS,
	const bool			pack,
	JXTextMenu*				menu,
	JArray<JIndex>*			lineIndexList,
	JArray<CBLanguage>*		lineLangList
	)
{
	menu->RemoveAllItems();

	JPtrArray<JString> fnNameList(JPtrArrayT::kDeleteAll);
	fnNameList.SetCompareFunction(JCompareStringsCaseInsensitive);
	fnNameList.SetSortOrder(JListT::kSortAscending);

	lineIndexList->RemoveAll();
	lineIndexList->SetCompareFunction(JCompareIndices);
	lineIndexList->SetSortOrder(JListT::kSortAscending);

	lineLangList->RemoveAll();

	if (pack)
		{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize()-2, false);
		menu->CompressHeight(true);
		}
	else
		{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize(), false);
		menu->CompressHeight(false);
		}

	CBTextFileType fileType = origFileType == kCBPHPFT ? kCBJavaScriptFT : origFileType;

	JString data;
	CBLanguage lang;
	if (ProcessFile(fileName, fileType, &data, &lang))
		{
		icharbufstream input(data.GetRawBytes(), data.GetByteCount());
		ReadFunctionList(input, CBGetLanguage(fileType), sort, includeNS,
						 &fnNameList, lineIndexList, lineLangList);

		if (lang == kCBHTMLLang)
			{
			for (auto* s : fnNameList)
				{
				s->Prepend("#");
				}
			}
		}

	if (lang == kCBHTMLLang && ProcessFile(fileName, kCBJavaScriptFT, &data, &lang))
		{
		icharbufstream input(data.GetRawBytes(), data.GetByteCount());
		ReadFunctionList(input, kCBJavaScriptLang, sort, includeNS,
						 &fnNameList, lineIndexList, lineLangList);
		}

	// build menu

	for (auto* s : fnNameList)
		{
		menu->AppendItem(*s);
		}
}

/******************************************************************************
 ReadFunctionList (private)

 ******************************************************************************/

void
CBFnMenuUpdater::ReadFunctionList
	(
	std::istream&		input,
	const CBLanguage	lang,
	const bool		sort,
	const bool		includeNS,
	JPtrArray<JString>*	fnNameList,
	JArray<JIndex>*		lineIndexList,
	JArray<CBLanguage>*	lineLangList
	)
{
	// build symbol list

	const bool hasNS = CBHasNamespace(lang);
	JString fnName;
	while (true)
		{
		input >> std::ws;
		fnName = JReadUntil(input, '\t');	// fn name
		if (input.eof() || input.fail())
			{
			break;
			}

		JIgnoreUntil(input, '\t');			// file name

		JIndex lineIndex;
		input >> lineIndex;					// line index

		if (IgnoreSymbol(fnName))
			{
			continue;
			}

		// toss qualified or unqualified version

		if (hasNS && !includeNS && CBNameIsQualified(fnName))
			{
			continue;
			}

		// save symbol

		JIndex i;
		if (sort)
			{
			i = fnNameList->GetInsertionSortIndex(&fnName);
			}
		else
			{
			i = lineIndexList->GetInsertionSortIndex(lineIndex);
			}
		fnNameList->InsertAtIndex(i, fnName);
		lineIndexList->InsertElementAtIndex(i, lineIndex);
		lineLangList->InsertElementAtIndex(i, lang);
		}

	// filter

	if (hasNS && includeNS)
		{
		JSize count = fnNameList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString* fnName = fnNameList->GetElement(i);
			if (CBNameIsQualified(*fnName))
				{
				const JIndex lineIndex = lineIndexList->GetElement(i);
				for (JIndex j=1; j<=count; j++)
					{
					if (j != i && lineIndexList->GetElement(j) == lineIndex)
						{
						fnNameList->DeleteElement(j);
						lineIndexList->RemoveElement(j);
						lineLangList->RemoveElement(j);
						count--;
						if (j < i)
							{
							i--;
							}
						break;
						}
					}
				}
			}
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBFnMenuUpdater::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		input >> JBoolFromString(itsSortFlag);

		if (vers >= 2)
			{
			input >> JBoolFromString(itsIncludeNSFlag);
			}

		if (vers >= 1)
			{
			input >> JBoolFromString(itsPackFlag);
			}
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBFnMenuUpdater::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsSortFlag);
	output << ' ' << JBoolToString(itsIncludeNSFlag);
	output << ' ' << JBoolToString(itsPackFlag);
}
