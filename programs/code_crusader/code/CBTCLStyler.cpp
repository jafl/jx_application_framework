/******************************************************************************
 CBTCLStyler.cpp

	Helper object for CBTextEditor that displays TCL with styles to hilight
	keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBTCLScanner

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBTCLStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBTCLStyler* CBTCLStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 0;

static const JCharacter* kTypeNames[] =
{
	"Predefined word",
	"Variable",
	"String",

	"Comment",

	"Brace: { }",
	"Square bracket: [ ]",
	"Parenthesis: ( )",
	"Semicolon: ;",

	"Other word",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit TCL Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBTCLStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBTCLStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBTCLStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBTCLStyler::CBTCLStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBTCLStyleID, kCBTCLFT),
	CBTCLScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap = GetColormap();

	SetTypeStyle(kPredefinedWord - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kVariable       - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kString         - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kComment        - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kError          - kWhitespace, JFontStyle(colormap->GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTCLStyler::~CBTCLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBTCLStyler::Scan
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

		if (token.type == kPredefinedWord ||
			token.type == kOtherWord ||
			token.type == kVariable ||
			token.type == kString ||
			token.type == kComment)
			{
			SaveTokenStart(TokenExtra());
			}

		// handle special cases

		if (token.type == kString ||
			token.type == kUnterminatedString)
			{
			ExtendCheckRangeForString(token.range);
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kComment       ||
				 token.type == kString        ||
				 token.type == kBrace         ||
				 token.type == kSquareBracket ||
				 token.type == kParenthesis)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else
			{
			style = GetStyle(typeIndex, text.GetSubstring(token.range));
			}

		keepGoing = SetStyle(token.range, style);

		if (token.type == kString)
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
CBTCLStyler::ExtendCheckRangeForString
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

static JRegex emptyVariablePattern = "^\\$\\{\\}";
static JRegex variablePattern      = "^\\$(\\{[^}]+\\}|[[:alnum:]_]+)";

void
CBTCLStyler::StyleEmbeddedVariables
	(
	const Token& token
	)
{
	emptyVariablePattern.SetSingleLine();
	variablePattern.SetSingleLine();

	const JString& text = GetText();

	JFontStyle varStyle = GetTypeStyle(token.type - kWhitespace);
	varStyle.underlineCount++;
	if (varStyle == GetTypeStyle(kVariable - kWhitespace))
		{
		varStyle.underlineCount++;
		}

	JFontStyle errStyle = GetTypeStyle(kError - kWhitespace);
	errStyle.underlineCount++;
	if (errStyle == GetTypeStyle(kVariable - kWhitespace))
		{
		errStyle.underlineCount++;
		}

	JIndexRange r = token.range, r1, r2;
	while (!r.IsEmpty())
		{
		const JCharacter c = text.GetCharacter(r.first);
		if (c == '\\')
			{
			r.first++;
			}
		else if (c == '$')
			{
			r1 = r - (r.first-1);
			if (emptyVariablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				AdjustStyle(r2, errStyle);
				r.first = r2.last;
				}
			else if (variablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				AdjustStyle(r2, varStyle);
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
CBTCLStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
