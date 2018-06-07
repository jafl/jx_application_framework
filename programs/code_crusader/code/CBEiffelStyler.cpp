/******************************************************************************
 CBEiffelStyler.cpp

	Helper object for CBTextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = CBStylerBase, CBEiffelScanner

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBEiffelStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JXColorManager.h>
#include <jAssert.h>

CBEiffelStyler* CBEiffelStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in cbmUtil.cc

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JCharacter* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",

	"Operator",
	"Free operator",
	"Delimiter",

	"String",

	"Floating point constant",
	"Decimal constant",
	"Binary constant",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit Eiffel Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBEiffelStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBEiffelStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBEiffelStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBEiffelStyler::CBEiffelStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBEiffelStyleID, kCBEiffelFT),
	CBEiffelScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JXColorManager* colormap   = GetColormap();
	const JColorID red = colormap->GetRedColor();

	SetTypeStyle(kReservedKeyword - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kString          - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kComment         - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kError           - kWhitespace, JFontStyle(red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEiffelStyler::~CBEiffelStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBEiffelStyler::Scan
	(
	std::istream&		input,
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
			token.type == kBuiltInDataType ||
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
CBEiffelStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
