/******************************************************************************
 CBSQLStyler.cpp

	Helper object for CBTextEditor that displays bash with styles to hilight
	keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBSQLScanner

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSQLStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBSQLStyler* CBSQLStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 0;

static const JCharacter* kTypeNames[] =
{
	"Identifier",
	"Variable",
	"Keyword",
	"Reserved word",
	"Built-in function",
	"Extension: MySQL keyword",
	"Extension: PostgreSQL keyword",
	"Extension: Oracle keyword",

	"Operator",
	"Delimiter",
	"Extension: MySQL operator",

	"Single quoted string",
	"Double quoted string",
	"Backtick quoted string",

	"Floating point value",
	"Decimal integer value",
	"Hex integer value",

	"Comment",
	"Extension: MySQL comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit SQL Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBSQLStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBSQLStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBSQLStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBSQLStyler::CBSQLStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBSQLStyleID, kCBSQLFT),
	CBSQLScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap = GetColormap();

	SetTypeStyle(kVariable            - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kKeyword             - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kReservedWord        - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kBuiltInFunction     - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString   - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString   - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kBackQuoteString     - kWhitespace, JFontStyle(colormap->GetPinkColor()));
	SetTypeStyle(kComment             - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));

	SetTypeStyle(kExtensionMySQL      - kWhitespace, JFontStyle(colormap->GetRedColor()));
	SetTypeStyle(kExtensionPostgreSQL - kWhitespace, JFontStyle(colormap->GetRedColor()));
	SetTypeStyle(kExtensionOracle     - kWhitespace, JFontStyle(colormap->GetRedColor()));
	SetTypeStyle(kMySQLOperator       - kWhitespace, JFontStyle(colormap->GetRedColor()));
	SetTypeStyle(kMySQLComment        - kWhitespace, JFontStyle(colormap->GetRedColor()));
	SetTypeStyle(kError               - kWhitespace, JFontStyle(colormap->GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSQLStyler::~CBSQLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBSQLStyler::Scan
	(
	std::istream&			input,
	const TokenExtra&	initData
	)
{
	BeginScan(input);

	const JString& text = GetText();

	JBoolean keepGoing;
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
			token.type == kKeyword           ||
			token.type == kReservedWord      ||
			token.type == kBuiltInFunction   ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kBackQuoteString   ||
			token.type == kComment)
			{
			SaveTokenStart(TokenExtra());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kSingleQuoteString ||
				 token.type == kDoubleQuoteString ||
				 token.type == kBackQuoteString   ||
				 token.type == kComment)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else
			{
			JString word = text.GetSubstring(token.range);
			word.ToLower();
			style = GetStyle(typeIndex, word);
			}

		keepGoing = SetStyle(token.range, style);
		}
		while (keepGoing);
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBSQLStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
