/******************************************************************************
 CBPythonStyler.cpp

	Helper object for CBTextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = CBStylerBase, CBPythonScanner

	Copyright (C) 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBPythonStyler.h"
#include "CBPrefsManager.h"
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBPythonStyler* CBPythonStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 0;

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
	"Octal constant",
	"Imaginary constant",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit C/C++ Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBPythonStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPythonStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPythonStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPythonStyler::CBPythonStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBPythonStyleID, kCBPythonFT),
	CBPythonScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap   = GetColormap();
	const JColorIndex red = colormap->GetRedColor();

	SetTypeStyle(kReservedKeyword - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kString          - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kComment         - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kError           - kWhitespace, JFontStyle(red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPythonStyler::~CBPythonStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBPythonStyler::Scan
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
			token.type == kString          ||
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
		else if (token.type == kComment ||
				 token.type == kString)
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
		}
		while (SetStyle(token.range, style));
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBPythonStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set jnew values after all jnew slots have been created
}
