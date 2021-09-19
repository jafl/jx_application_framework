/******************************************************************************
 CBDStyler.cpp

	Helper object for CBTextEditor that displays D with styles to hilight
	keywords, preprocessor directives, etc.

	Not to be confused with the drug.

	BASE CLASS = CBStylerBase, CB::Text::D::Scanner

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "CBDStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBDStyler* CBDStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in cbmUtil.cpp

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",

	"Operator",
	"Delimiter",

	"String",
	"Wysiwyg string",
	"Hexadecimal string",
	"Token string",
	"Character constant",

	"Floating point constant",
	"Hexadecimal floating point constant",
	"Decimal constant",
	"Binary constant",
	"Hexadecimal constant",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

static const JUtf8Byte* kDeprecatedKeyword[] =
{
	"delete", "macro"
};

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBDStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBDStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBDStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBDStyler::CBDStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBDStyler"),
				 kCBDStyleID, kCBDFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kReservedKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kWysiwygString   - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kHexString       - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kTokenString     - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kCharConst       - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment         - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));

	SetTypeStyle(kError           - kWhitespace, JFontStyle(red));

	SetWordStyle(JString("goto", JString::kNoCopy), JFontStyle(true, false, 0, false, red));

	for (const auto* kw : kDeprecatedKeyword)
		{
		SetWordStyle(JString(kw, JString::kNoCopy), JFontStyle(red));
		}

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDStyler::~CBDStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBDStyler::Scan
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

		if (token.type == kID              ||
			token.type == kReservedKeyword ||
			token.type == kBuiltInDataType ||
			token.type == kString          ||
			token.type == kWysiwygString   ||
			token.type == kHexString       ||
			token.type == kTokenString     ||
			token.type == kComment)
			{
			SaveTokenStart(token.range.GetFirst());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kComment       ||
				 token.type == kString        ||
				 token.type == kWysiwygString ||
				 token.type == kHexString     ||
				 token.type == kTokenString)
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
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBDStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}

/******************************************************************************
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
CBDStyler::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CBStylerBase::Receive(sender, message);

#if defined CODE_CRUSADER && ! defined CODE_CRUSADER_UNIT_TEST

	if (message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CBMWriteSharedPrefs(true);
			}
		}

#endif
}
