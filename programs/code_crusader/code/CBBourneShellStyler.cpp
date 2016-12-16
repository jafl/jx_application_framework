/******************************************************************************
 CBBourneShellStyler.cpp

	Helper object for CBTextEditor that displays bash with styles to hilight
	keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBBourneShellScanner

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBBourneShellStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBBourneShellStyler* CBBourneShellStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 0;

static const JCharacter* kTypeNames[] =
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit Bourne Shell Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBBourneShellStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBBourneShellStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
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
				 kEditDialogTitle, kCBBourneShellStyleID, kCBBourneShellFT),
	CBBourneShellScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap = GetColormap();

	SetTypeStyle(kVariable          - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kReservedWord      - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kBuiltInCommand    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kExecString        - kWhitespace, JFontStyle(colormap->GetPinkColor()));
	SetTypeStyle(kComment           - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kError             - kWhitespace, JFontStyle(colormap->GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBBourneShellStyler::~CBBourneShellStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBBourneShellStyler::Scan
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
			token.type == kVariable          ||
			token.type == kReservedWord      ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString        ||
			token.type == kComment)
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

	"x"$x"
	  ^
	Inserting the marked character tricks JTEStyler into not continuing
	because the style didn't change and it was at a style run boundary.

 ******************************************************************************/

void
CBBourneShellStyler::ExtendCheckRangeForString
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

#define CBBourneShellStringID "([[:alpha:]_][[:alnum:]_]*|[0-9]+)"

static JRegex variablePattern =
	"^\\$(" CBBourneShellStringID "|\\{[#!]?" CBBourneShellStringID "(\\[[^]\n]+\\])?([}:]|#{1,2}|%{1,2}|/{1,2})|[-0-9*@#?$!_])";
static JRegex emptyVariablePattern =
	"^\\$\\{\\}?";

#undef CBBourneShellStringID

void
CBBourneShellStyler::StyleEmbeddedVariables
	(
	const Token& token
	)
{
	variablePattern.SetSingleLine();
	emptyVariablePattern.SetSingleLine();

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
CBBourneShellStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
