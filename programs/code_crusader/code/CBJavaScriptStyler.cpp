/******************************************************************************
 CBJavaScriptStyler.cpp

	Helper object for CBTextEditor that displays JavaScript with styles to
	hilight keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBJavaScriptScanner

	Copyright (C) 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBJavaScriptStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBJavaScriptStyler* CBJavaScriptStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 1;

	// version 1 adds kRegexSearch after kHexInt (8)

static const JCharacter* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",

	"Operator",
	"Delimiter",

	"String",

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
	"abstract", "debugger", "enum", "goto", "implements", "interface",
	"native", "protected", "synchronized", "throws", "transient", "volatile"
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

		itsSelf = new CBJavaScriptStyler;
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
	delete itsSelf;
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
	istream&			input,
	const TokenExtra&	initData
	)
{
	BeginScan(input);

	const JString& text = GetText();

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
		}
		while (SetStyle(token.range, style));
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

	// set new values after all new slots have been created
}
