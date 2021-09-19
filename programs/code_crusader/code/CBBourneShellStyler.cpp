/******************************************************************************
 CBBourneShellStyler.cpp

	Helper object for CBTextEditor that displays bash with styles to hilight
	keywords, strings, etc.

	BASE CLASS = CBStylerBase, CB::Text::BourneShell::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBBourneShellStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JStringIterator.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBBourneShellStyler* CBBourneShellStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Variable",
	"Reserved keyword",
	"Built-in command",

	"Control operator",
	"Redirection operator",
	"History operator",

	"Single quoted string",
	"Double quoted string",
	"Executed string",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBBourneShellStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBBourneShellStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBBourneShellStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBBourneShellStyler::CBBourneShellStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBBourneShellStyler"),
				 kCBBourneShellStyleID, kCBBourneShellFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	SetTypeStyle(kVariable          - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kReservedWord      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInCommand    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kExecString        - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kComment           - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError             - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBBourneShellStyler::~CBBourneShellStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBBourneShellStyler::Scan
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

		// save token starts

		if (token.type == kID                ||
			token.type == kVariable          ||
			token.type == kReservedWord      ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString        ||
			token.type == kComment)
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
		else if (token.type == kSingleQuoteString ||
				 token.type == kDoubleQuoteString ||
				 token.type == kExecString        ||
				 token.type == kComment)
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

	"x"$x"
	  ^
	Inserting the marked character tricks JTEStyler into not continuing
	because the style didn't change and it was at a style run boundary.

 ******************************************************************************/

void
CBBourneShellStyler::ExtendCheckRangeForString
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

#define CBBourneShellStringID "([[:alpha:]_][[:alnum:]_]*|[0-9]+)"

static JRegex variablePattern =
	"(?<!\\\\)\\$(" CBBourneShellStringID "|\\{[#!]?" CBBourneShellStringID "(\\[[^]\n]+\\])?([}:]|#{1,2}|%{1,2}|/{1,2})|[-0-9*@#?$!_])";
static JRegex emptyVariablePattern =
	"(?<!\\\\)\\$\\{\\}?";

#undef CBBourneShellStringID

#define ClassName CBBourneShellStyler
#include "CBSTStylerEmbeddedVariables.th"
#undef ClassName

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBBourneShellStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
