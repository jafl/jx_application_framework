/******************************************************************************
 CBCStyler.cpp

	Helper object for CBTextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = CBStylerBase, CB::Text::C::Scanner

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBCStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBCStyler* CBCStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in cbmUtil.cpp

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Identifier containing $",
	"Reserved C keyword",
	"Reserved C++ keyword",
	"Built-in data type",

	"Operator",
	"Delimiter",

	"String",
	"Character constant",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",
	"Octal constant",

	"Comment",
	"Preprocessor directive",

	"Trigraph",
	"Respelling",
	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBCStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBCStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBCStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCStyler::CBCStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBCStyler"),
				 kCBCStyleID, kCBCSourceFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kDollarID           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kReservedCKeyword   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kReservedCPPKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString             - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kCharConst          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));

	SetTypeStyle(kTrigraph           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kRespelling         - kWhitespace, JFontStyle(red));
	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	SetWordStyle(JString("#pragma", JString::kNoCopy),       JFontStyle(red));
	SetWordStyle(JString("#include_next", JString::kNoCopy), JFontStyle(red));
	SetWordStyle(JString("goto", JString::kNoCopy),          JFontStyle(true, false, 0, false, red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCStyler::~CBCStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBCStyler::Scan
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

		if (token.type == kID                 ||
			token.type == kReservedCKeyword   ||
			token.type == kReservedCPPKeyword ||
			token.type == kBuiltInDataType    ||
			token.type == kString             ||
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
		else if (token.type == kComment ||
				 token.type == kString)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type == kPPDirective && SlurpPPComment(&(token.range)))
			{
			token.type = kComment;
			style      = GetTypeStyle(kComment - kWhitespace);
			}
		else if (token.type == kPPDirective)
			{
			style = GetStyle(typeIndex, GetPPCommand(text));
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
 PreexpandCheckRange (virtual protected)

	Expand checkRange if the preceding text is #[[:space:]]*
	This catches "#includ" when typing 'e' and the token start happens
	to be after the #.

	modifiedRange is the range of text that was changed.
	deletion is true if the modification was that text was deleted.

 ******************************************************************************/

void
CBCStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JCharacterRange&	modifiedRange,
	const bool			deletion,
	JStyledText::TextRange*	checkRange
	)
{
	JStringIterator iter(text);
	iter.UnsafeMoveTo(kJIteratorStartBefore, checkRange->charRange.first, checkRange->byteRange.first);

	JUtf8Character c;
	while (iter.Prev(&c, kJIteratorStay) && c.IsSpace())
		{
		iter.SkipPrev();
		}
	if (iter.Prev(&c) && c == '#')
		{
		checkRange->charRange.first = iter.GetNextCharacterIndex();
		checkRange->byteRange.first = iter.GetNextByteIndex();
		}
}

/******************************************************************************
 SlurpPPComment (private)

	Treat "#if 0 ... #(elif|else|endif)" as a comment.

 ******************************************************************************/

static const JRegex ppCommentPattern =
	"^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*(el)?if[[:space:]]+0[[:space:]]*(//.*|/\\*[^*]*\\*/)?\n";

static const JRegex ppIfPattern   = "^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*if";
static const JRegex ppElsePattern = "^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*(else|elif)";
static const JRegex ppEndPattern  = "^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*endif";

bool
CBCStyler::SlurpPPComment
	(
	JStyledText::TextRange* totalRange
	)
{
	const JString& text = GetText();
	const JString s(text.GetRawBytes(), JUtf8ByteRange(GetPPNameRange().byteRange.first, totalRange->byteRange.last), JString::kNoCopy);
	if (!ppCommentPattern.Match(s))
		{
		return false;
		}

	Token token;
	JSize nestCount = 1;
	while (true)
		{
		token = NextToken();
		if (token.type == kEOF)
			{
			break;
			}
		else if (token.type == kPPDirective)
			{
			const JString ppCmd(text.GetRawBytes(), GetPPNameRange().byteRange, JString::kNoCopy);
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
				const bool ok =
					JUtf8Character::GetPrevCharacterByteCount(
						text.GetRawBytes() + token.range.byteRange.first-2,
						&prevCharByteCount);
				assert( ok );

				Undo(token.range, prevCharByteCount,
					 JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));	// rescan
				token.range.charRange.SetToEmptyAt(token.range.charRange.first);
				token.range.byteRange.SetToEmptyAt(token.range.byteRange.first);
				break;
				}
			}
		}

	totalRange->charRange.last = token.range.charRange.last;
	totalRange->byteRange.last = token.range.byteRange.last;
	return true;
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBCStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
		{
		for (JIndex i=1; i<=20; i++)
			{
			typeStyles->RemoveElement(11);
			}
		}

	if (vers < 2)
		{
		typeStyles->InsertElementAtIndex(5, typeStyles->GetElement(4));
		}

	if (vers < 3)
		{
		const JFontStyle style = typeStyles->GetElement(1);
		typeStyles->InsertElementAtIndex(6, style);
		typeStyles->InsertElementAtIndex(6, style);
		}

	// set new values after all new slots have been created
}

/******************************************************************************
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
CBCStyler::Receive
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
