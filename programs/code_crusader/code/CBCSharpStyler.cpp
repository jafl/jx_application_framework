/******************************************************************************
 CBCSharpStyler.cpp

	Helper object for CBTextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = CBStylerBase, CBCSharpScanner

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#include "CBCSharpStyler.h"
#include "CBPrefsManager.h"
#include <JRegex.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBCSharpStyler* CBCSharpStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",

	"Operator",
	"Delimiter",

	"String",
	"Character constant",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",

	"Comment",
	"Documentation comment",
	"Preprocessor directive",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBCSharpStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBCSharpStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBCSharpStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCSharpStyler::CBCSharpStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBCSharpStyler"),
				 kCBCSharpStyleID, kCBCSharpFT),
	CBCSharpScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	SetTypeStyle(kReservedCKeyword   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString             - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kCharConst          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kDocComment         - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	SetWordStyle(JString("goto", kJFalse), JFontStyle(kJTrue, kJFalse, 0, kJFalse, JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCSharpStyler::~CBCSharpStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBCSharpStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(startIndex, input);

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

		if (token.type == kID                 ||
			token.type == kReservedCKeyword   ||
			token.type == kBuiltInDataType    ||
			token.type == kString             ||
			token.type == kComment            ||
			token.type == kDocComment)
			{
			SaveTokenStart(token.range.GetFirst());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kComment    ||
				 token.type == kDocComment ||
				 token.type == kString)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type == kPPDirective && SlurpPPComment(&token.range))
			{
			token.type = kComment;
			style      = GetTypeStyle(kComment - kWhitespace);
			}
		else if (token.type == kPPDirective)
			{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), GetPPNameRange().byteRange, kJFalse));
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else if (token.type > kError)	// misc
			{
			if (!GetWordStyle(JString(text.GetRawBytes(), token.range.byteRange, kJFalse), &style))
				{
				style = GetDefaultFont().GetStyle();
				}
			}
		else
			{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, kJFalse));
			}
		}
		while (SetStyle(token.range.charRange, style));
}

/******************************************************************************
 SlurpPPComment (private)

	Treat "#if false ... #(elif|else|endif)" as a comment.

 ******************************************************************************/

static const JRegex ppCommentPattern =
	"^[[:space:]]*#[[:space:]]*(el)?if[[:space:]]+false[[:space:]]*(//.*|/\\*[^*]*\\*/)?\n";

static const JRegex ppIfPattern   = "^[[:space:]]*#[[:space:]]*if";
static const JRegex ppElsePattern = "^[[:space:]]*#[[:space:]]*(else|elif)";
static const JRegex ppEndPattern  = "^[[:space:]]*#[[:space:]]*endif";

#if 0	// comment
abc
#endif

#if 1
// nothing
 #elif 0 /* comment */
abc
	# if abc
	123
	# elif defined xyz
	456
	# endif
# endif

JBoolean
CBCSharpStyler::SlurpPPComment
	(
	JStyledText::TextRange* totalRange
	)
{
	const JString& text = GetText();
	const JString s(text.GetRawBytes(), JUtf8ByteRange(GetPPNameRange().byteRange.first, totalRange->byteRange.last), kJFalse);
	if (!ppCommentPattern.Match(s))
		{
		return kJFalse;
		}

	Token token;
	JSize nestCount = 1;
	while (1)
		{
		token = NextToken();
		if (token.type == kEOF)
			{
			break;
			}
		else if (token.type == kPPDirective)
			{
			const JString ppCmd(text.GetRawBytes(), GetPPNameRange().byteRange, kJFalse);
			if (ppIfPattern.Match(ppCmd))
				{
				nestCount++;
				}
			else if (ppEndPattern.Match(ppCmd))
				{
				nestCount--;
				if (nestCount == 0)
					{
					break;
					}
				}
			else if (ppElsePattern.Match(ppCmd) && nestCount == 1)
				{
				JSize prevCharByteCount;
				const JBoolean ok =
					JUtf8Character::GetPrevCharacterByteCount(
						text.GetRawBytes() + token.range.byteRange.first-2,
						&prevCharByteCount);
				assert( ok );

				Undo(token.range, prevCharByteCount,
					 JString(text.GetRawBytes(), token.range.byteRange, kJFalse));	// rescan
				token.range.charRange.SetToEmptyAt(token.range.charRange.first);
				token.range.byteRange.SetToEmptyAt(token.range.byteRange.first);
				break;
				}
			}
		}

	totalRange->charRange.last = token.range.charRange.last;
	totalRange->byteRange.last = token.range.byteRange.last;
	return kJTrue;
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBCSharpStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
