/******************************************************************************
 CBFnMenuUpdater.cpp

	Uses ctags to extract function names from a source file and build
	a menu to allow the user to jump to their definitions.

	BASE CLASS = JPrefObject, CBCtagsUser

	Copyright (C) 1999-2001 by John Lindal.

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
#include <strstream>
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
	itsSortFlag      = kJTrue;
	itsIncludeNSFlag = kJFalse;
	itsPackFlag      = kJFalse;
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

CBLanguage
CBFnMenuUpdater::UpdateMenu
	(
	const JString&			fileName,
	const CBTextFileType	fileType,
	const JBoolean			sort,
	const JBoolean			includeNS,
	const JBoolean			pack,
	JXTextMenu*				menu,
	JArray<JIndex>*			lineIndexList
	)
{
	menu->RemoveAllItems();
	lineIndexList->RemoveAll();

	if (pack)
		{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize()-2, kJFalse);
		menu->CompressHeight(kJTrue);
		}
	else
		{
		menu->SetDefaultFontSize(JFontManager::GetDefaultFontSize(), kJFalse);
		menu->CompressHeight(kJFalse);
		}

	JString data;
	CBLanguage lang;
	if (ProcessFile(fileName, fileType, &data, &lang))
		{
		std::istrstream input(data.GetRawBytes(), data.GetByteCount());
		ReadFunctionList(input, CBGetLanguage(fileType),
						 sort, includeNS, menu, lineIndexList);
		}
	return lang;
}

/******************************************************************************
 ReadFunctionList (private)

 ******************************************************************************/

inline JBoolean
cbIsQualified
	(
	const JString& s
	)
{
	return JI2B(s.Contains(":") || s.Contains("."));
}

void
CBFnMenuUpdater::ReadFunctionList
	(
	std::istream&	input,
	CBLanguage		lang,
	const JBoolean	sort,
	const JBoolean	includeNS,
	JXTextMenu*		menu,
	JArray<JIndex>*	lineIndexList
	)
{
	JPtrArray<JString> fnNameList(JPtrArrayT::kDeleteAll);
	fnNameList.SetCompareFunction(JCompareStringsCaseInsensitive);
	fnNameList.SetSortOrder(JListT::kSortAscending);

	lineIndexList->SetCompareFunction(JCompareIndices);
	lineIndexList->SetSortOrder(JListT::kSortAscending);

	// build symbol list

	const JBoolean hasNS = CBHasNamespace(lang);
	JString fnName;
	while (1)
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

		if (hasNS && !includeNS && cbIsQualified(fnName))
			{
			continue;
			}

		// save symbol

		JIndex i;
		if (sort)
			{
			i = fnNameList.GetInsertionSortIndex(&fnName);
			}
		else
			{
			i = lineIndexList->GetInsertionSortIndex(lineIndex);
			}
		fnNameList.InsertAtIndex(i, fnName);
		lineIndexList->InsertElementAtIndex(i, lineIndex);
		}

	if (hasNS && includeNS)
		{
		JSize count = fnNameList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString* fnName  = fnNameList.GetElement(i);
			const JIndex lineIndex = lineIndexList->GetElement(i);
			if (cbIsQualified(*fnName))
				{
				for (JIndex j=1; j<=count; j++)
					{
					if (j != i && lineIndexList->GetElement(j) == lineIndex)
						{
						fnNameList.DeleteElement(j);
						lineIndexList->RemoveElement(j);
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

	// build menu

	const JSize count = fnNameList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		menu->AppendItem(*(fnNameList.GetElement(i)));
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
