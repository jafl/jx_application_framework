/******************************************************************************
 CBJavaStyler.cpp

	Helper object for CBTextEditor that displays Java with styles to
	hilight keywords, strings, etc.

	BASE CLASS = CBStylerBase, CBJavaScanner

	Copyright (C) 2001 by John Lindal.

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
	"operator", "outer", "rest", "var"
};

const JSize kUnusedKeywordCount = sizeof(kUnusedKeyword)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBJavaStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBJavaStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
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
				 kCBJavaStyleID, kCBJavaSourceFT),
	CBJavaScanner()
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
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(kJFalse, kJFalse, 1, kJFalse));

	SetTypeStyle(kError                - kWhitespace, JFontStyle(red));

	SetWordStyle(JString("goto", kJFalse), JFontStyle(kJTrue, kJFalse, 0, kJFalse, red));

	for (JUnsignedOffset i=0; i<kUnusedKeywordCount; i++)
		{
		SetWordStyle(JString(kUnusedKeyword[i], kJFalse), JFontStyle(red));
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
					SetStyle(token.docCommentRange, GetTypeStyle(kComment - kWhitespace));
					}
				ExtendCheckRange(token.range.last+1);
				}

			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range, kJFalse));
			}
		}
		while (SetStyle(token.range, style));
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

#ifdef CODE_CRUSADER

	if (message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CBMWriteSharedPrefs(kJTrue);
			}
		}

#endif
}
