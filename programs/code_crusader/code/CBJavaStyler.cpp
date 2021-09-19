/******************************************************************************
 CBJavaStyler.cpp

	Helper object for CBTextEditor that displays Java with styles to
	hilight keywords, strings, etc.

	BASE CLASS = CBStylerBase, CB::Text::Java::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBJavaStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBJavaStyler* CBJavaStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 1;

	// Remember to increment kCurrentSharedPrefsVersion in cbmUtil.cpp

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",
	"Annotation",

	"Operator",
	"Delimiter",

	"String",
	"Character constant",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",

	"Comment",
	"HTML tag in javadoc comment",
	"@ tag in javadoc comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

static const JUtf8Byte* kUnusedKeyword[] =
{
	"byvalue", "cast", "const", "future", "generic", "inner",
	"operator", "outer", "rest"
};

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBJavaStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBJavaStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJavaStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJavaStyler::CBJavaStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBJavaStyler"),
				 kCBJavaStyleID, kCBJavaSourceFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kReservedKeyword      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString               - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kCharConst            - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment              - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kDocCommentHTMLTag    - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(false, false, 1, false));

	SetTypeStyle(kError                - kWhitespace, JFontStyle(red));

	SetWordStyle(JString("goto", JString::kNoCopy), JFontStyle(true, false, 0, false, red));

	for (const auto* kw : kUnusedKeyword)
		{
		SetWordStyle(JString(kw, JString::kNoCopy), JFontStyle(red));
		}

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaStyler::~CBJavaStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBJavaStyler::Scan
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
			token.type == kString)
			{
			SaveTokenStart(token.range.GetFirst());
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
		else
			{
			if (token.type == kDocCommentHTMLTag ||
				token.type == kDocCommentSpecialTag)
				{
				if (!(token.docCommentRange).IsEmpty())
					{
					SetStyle(token.docCommentRange.charRange, GetTypeStyle(kComment - kWhitespace));
					}
				ExtendCheckRange(token.range.charRange.last+1);
				}

			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
			}
		}
		while (SetStyle(token.range.charRange, style));
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBJavaStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(4, JFontStyle(JColorManager::GetBlueColor()));
		}

	// set new values after all new slots have been created
}

/******************************************************************************
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
CBJavaStyler::Receive
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
