/******************************************************************************
 CBINIStyler.cpp

	Helper object for CBTextEditor that displays .ini with styles to hilight
	sections, comments, etc.

	BASE CLASS = CBStylerBase, CBINIScanner

	Copyright © 2013 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBINIStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBINIStyler* CBINIStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 1;

	// version 1 inserts kArrayIndex after kAssignment (2)

static const JCharacter* kTypeNames[] =
{
	"Section Name",

	"Key or Value",
	"Assignment",
	"Array index",

	"Single quoted string",
	"Double quoted string",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit INI Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBINIStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBINIStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBINIStyler::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBINIStyler::CBINIStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBINIStyleID, kCBINIFT),
	CBINIScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap = GetColormap();

	SetTypeStyle(kSectionName       - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse));

	SetTypeStyle(kSingleQuoteString - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));

	SetTypeStyle(kComment           - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));

	SetTypeStyle(kError             - kWhitespace, JFontStyle(colormap->GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBINIStyler::~CBINIStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBINIStyler::Scan
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

		// save token starts

		if (token.type == kAssignment)
			{
			SaveTokenStart(TokenExtra());
			}

		// handle special cases

		if (token.type == kDoubleQuoteString)
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
CBINIStyler::ExtendCheckRangeForString
	(
	const JIndexRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.last+1);
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBINIStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(3, JFontStyle());
		}
}
