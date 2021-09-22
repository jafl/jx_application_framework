/******************************************************************************
 CBRubyStyler.cpp

	Helper object for CBTextEditor that displays Ruby with styles to hilight
	keywords, comments, etc.

	BASE CLASS = CBStylerBase, CB::Text::Ruby::Scanner

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "CBRubyStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JStringIterator.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBRubyStyler* CBRubyStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 1;

// version 1 inserts kWordList after kSymbol (11)

static const JUtf8Byte* kTypeNames[] =
{
	"Local variable",
	"Instance variable",
	"Global variable",
	"Symbol",
	"Reserved keyword",

	"Operator",
	"Delimiter",

	"Single quoted string",
	"Double quoted string",
	"Heredoc string",
	"Executed string",
	"Quoted word list",

	"Floating point constant",
	"Decimal constant",
	"Binary constant",
	"Octal constant",
	"Hexadecimal constant",

	"Regex",

	"Comment",
	"Embedded documentation",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBRubyStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBRubyStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBRubyStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRubyStyler::CBRubyStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBRubyStyler"),
				 kCBRubyStyleID, kCBRubyFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	SetTypeStyle(kReservedKeyword    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kSingleQuoteString  - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString  - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kHereDocString      - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kExecString         - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kWordList           - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kRegex              - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kEmbeddedDoc        - kWhitespace, JFontStyle(true, false, 0, false, JColorManager::GetGrayColor(50)));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRubyStyler::~CBRubyStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBRubyStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(GetStyledText(), startIndex, input);

	const JString& text = GetText();

	bool keepGoing;
	Token token;
	JFontStyle style;
	do
		{
		token = NextToken();
		if (token.type == kEOF)
			{
			break;
			}

		// save token starts -- must set itsProbableOperatorFlag

		if (token.type == kLocalVariable     ||
			token.type == kInstanceVariable  ||
			token.type == kGlobalVariable    ||
			token.type == kSymbol            ||
			token.type == kReservedKeyword   ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString)
			{
			SaveTokenStart(token.range.GetFirst());
			}

		// handle special cases

		if (token.type == kDoubleQuoteString ||
			token.type == kExecString)
			{
			ExtendCheckRangeForString(token.range);
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kSingleQuoteString  ||
				 token.type == kDoubleQuoteString  ||
				 token.type == kHereDocString      ||
				 token.type == kExecString         ||
				 token.type == kRegex              ||
				 token.type == kComment            ||
				 token.type == kEmbeddedDoc)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else if (token.type > kError)	// misc
			{
			if (!GetWordStyle(JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy), &style))
				{
				style = GetDefaultFont().GetStyle();
				}
			}
		else
			{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
			}

		keepGoing = SetStyle(token.range.charRange, style);

		if (token.type == kDoubleQuoteString ||
			token.type == kExecString)
			{
			StyleEmbeddedVariables(token);
			}
		}
		while (keepGoing);
}

/******************************************************************************
 ExtendCheckRangeForString (private)

	There is one case where modifying a string doesn't force a restyling:

	"x"#{x}"
	  ^
	Inserting the marked character tricks JTEStyler into not continuing
	because the style didn't change and it was at a style run boundary.

 ******************************************************************************/

void
CBRubyStyler::ExtendCheckRangeForString
	(
	const JStyledText::TextRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.charRange.last+1);
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

static JRegex variablePattern =      "(?<!\\\\)#\\{[^}]+\\}";
static JRegex emptyVariablePattern = "(?<!\\\\)#\\{\\}?";

#define ClassName CBRubyStyler
#define NoVariableIndex
#include "CBSTStylerEmbeddedVariables.th"
#undef NoVariableIndex
#undef ClassName

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBRubyStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(12, typeStyles->GetElement(9));
		}

	// set new values after all new slots have been created
}
