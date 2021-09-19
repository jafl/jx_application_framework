/******************************************************************************
 CBINIStyler.cpp

	Helper object for CBTextEditor that displays .ini with styles to hilight
	sections, comments, etc.

	BASE CLASS = CBStylerBase, CB::Text::INI::Scanner

	Copyright © 2013 by John Lindal.

 ******************************************************************************/

#include "CBINIStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBINIStyler* CBINIStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 1;

	// version 1 inserts kArrayIndex after kAssignment (2)

static const JUtf8Byte* kTypeNames[] =
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBINIStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBINIStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBINIStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBINIStyler::CBINIStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBINIStyler"),
				 kCBINIStyleID, kCBINIFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	SetTypeStyle(kSectionName       - kWhitespace, JFontStyle(true, false, 0, false));

	SetTypeStyle(kSingleQuoteString - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment           - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));

	SetTypeStyle(kError             - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBINIStyler::~CBINIStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBINIStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(GetStyledText(), startIndex, input);

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

		if (token.type == kAssignment)
			{
			SaveTokenStart(token.range.GetFirst());
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
			if (!GetWordStyle(JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy), &style))
				{
				style = GetDefaultFont().GetStyle();
				}
			}
		else
			{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
			}
		}
		while (SetStyle(token.range.charRange, style));
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
	const JStyledText::TextRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.charRange.last+1);
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
