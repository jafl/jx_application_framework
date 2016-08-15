/******************************************************************************
 CBRubyStyler.cpp

	Helper object for CBTextEditor that displays Ruby with styles to hilight
	keywords, comments, etc.

	BASE CLASS = CBStylerBase, CBRubyScanner

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBRubyStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBRubyStyler* CBRubyStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 1;

// version 1 inserts kWordList after kSymbol (11)

static const JCharacter* kTypeNames[] =
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit Ruby Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBRubyStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBRubyStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBRubyStyler::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBRubyStyler::CBRubyStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBRubyStyleID, kCBRubyFT),
	CBRubyScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap   = GetColormap();
	const JColorIndex red = colormap->GetRedColor();

	SetTypeStyle(kReservedKeyword    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kSingleQuoteString  - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString  - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kHereDocString      - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kExecString         - kWhitespace, JFontStyle(colormap->GetPinkColor()));

	SetTypeStyle(kRegex              - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kEmbeddedDoc        - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse, colormap->GetGrayColor(50)));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRubyStyler::~CBRubyStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBRubyStyler::Scan
	(
	istream&			input,
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
			SaveTokenStart(TokenExtra());
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
			if (!GetWordStyle(text.GetSubstring(token.range), &style))
				{
				style = GetDefaultFont().GetStyle();
				}
			}
		else
			{
			style = GetStyle(typeIndex, text.GetSubstring(token.range));
			}

		keepGoing = SetStyle(token.range, style);

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
	const JIndexRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.last+1);
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

static JRegex variablePattern =      "^#\\{[^}]+\\}";
static JRegex emptyVariablePattern = "^#\\{\\}?";

void
CBRubyStyler::StyleEmbeddedVariables
	(
	const Token& token
	)
{
	variablePattern.SetSingleLine();
	emptyVariablePattern.SetSingleLine();

	const JString& text = GetText();

	JFontStyle varStyle = GetTypeStyle(token.type - kWhitespace);
	varStyle.underlineCount++;

	JFontStyle errStyle = GetTypeStyle(kError - kWhitespace);
	errStyle.underlineCount++;

	JIndexRange r = token.range, r1, r2;
	while (!r.IsEmpty())
		{
		const JCharacter c = text.GetCharacter(r.first);
		if (c == '\\')
			{
			r.first++;
			}
		else if (c == '#')
			{
			r1 = r - (r.first-1);
			if (variablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				AdjustStyle(r2, varStyle);
				r.first = r2.last;
				}
			else if (emptyVariablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				AdjustStyle(r2, errStyle);
				r.first = r2.last;
				}
			}

		r.first++;
		}
}

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
