/******************************************************************************
 CBJavaScriptStyler.cpp

	Helper object for CBTextEditor that displays JavaScript with styles to
	hilight keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBJavaScriptScanner

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#include "CBJavaScriptStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JColormap.h>
#include <JRegex.h>
#include <ctype.h>
#include <jAssert.h>

CBJavaScriptStyler* CBJavaScriptStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 2;

	// version 2 adds kTemplateString after kString (5)
	// version 1 adds kRegexSearch after kHexInt (8)

static const JCharacter* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",

	"Operator",
	"Delimiter",

	"String",
	"Template String",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",

	"Regular expression",

	"Comment",
	"HTML tag in javadoc comment",
	"@ tag in javadoc comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kUnusedKeyword[] =
{
	"debugger", "goto"
};

const JSize kUnusedKeywordCount = sizeof(kUnusedKeyword)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit JavaScript Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBJavaScriptStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBJavaScriptStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJavaScriptStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJavaScriptStyler::CBJavaScriptStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBJavaScriptStyleID, kCBJavaScriptFT),
	CBJavaScriptScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap   = GetColormap();
	const JColorIndex red = colormap->GetRedColor();

	SetTypeStyle(kReservedKeyword      - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kString               - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kTemplateString       - kWhitespace, JFontStyle(colormap->GetPinkColor()));
	SetTypeStyle(kRegexSearch          - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kComment              - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kDocCommentHTMLTag    - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(kJFalse, kJFalse, 1, kJFalse));

	SetTypeStyle(kError                - kWhitespace, JFontStyle(red));

	for (JIndex i=0; i<kUnusedKeywordCount; i++)
		{
		SetWordStyle(kUnusedKeyword[i], JFontStyle(red));
		}

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaScriptStyler::~CBJavaScriptStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBJavaScriptStyler::Scan
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

		if (token.type == kID              ||
			token.type == kReservedKeyword ||
			token.type == kString)
			{
			SaveTokenStart(TokenExtra());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kComment ||
				 token.type == kString  ||
				 token.type == kRegexSearch)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else
			{
			if (token.type == kDocCommentHTMLTag ||
				token.type == kDocCommentSpecialTag)
				{
				if (!(token.docCommentRange).IsEmpty())
					{
					SetStyle(token.docCommentRange, GetTypeStyle(kComment - kWhitespace));
					}
				ExtendCheckRange(token.range.last+1);
				}

			style = GetStyle(typeIndex, text.GetSubstring(token.range));
			}

		keepGoing = SetStyle(token.range, style);

		if (token.type == kTemplateString)
			{
			StyleEmbeddedVariables(token);
			}
		}
		while (keepGoing);
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

static JRegex variablePattern =      "^\\$\\{.+?\\}";
static JRegex emptyVariablePattern = "^\\$\\{\\}?";

void
CBJavaScriptStyler::StyleEmbeddedVariables
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
CBJavaScriptStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	JColormap* colormap = GetColormap();

	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(9, JFontStyle(colormap->GetDarkGreenColor()));
		}

	if (vers < 2)
		{
		typeStyles->InsertElementAtIndex(6, JFontStyle(colormap->GetPinkColor()));
		}

	// set new values after all new slots have been created
}
