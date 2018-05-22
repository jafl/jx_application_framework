/******************************************************************************
 CBCStyler.cpp

	Helper object for CBTextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = CBStylerBase, CBCScanner

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBCStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JXColorManager.h>
#include <jAssert.h>

CBCStyler* CBCStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in cbmUtil.cc

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JCharacter* kTypeNames[] =
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit C/C++ Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBCStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBCStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
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
				 kEditDialogTitle, kCBCStyleID, kCBCSourceFT),
	CBCScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JXColorManager* colormap   = GetColormap();
	const JColorID red = colormap->GetRedColor();

	SetTypeStyle(kDollarID           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kReservedCKeyword   - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kReservedCPPKeyword - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kString             - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kCharConst          - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(colormap->GetBlueColor()));

	SetTypeStyle(kTrigraph           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kRespelling         - kWhitespace, JFontStyle(red));
	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	SetWordStyle("#pragma",       JFontStyle(red));
	SetWordStyle("#include_next", JFontStyle(red));
	SetWordStyle("goto",          JFontStyle(kJTrue, kJFalse, 0, kJFalse, red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCStyler::~CBCStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBCStyler::Scan
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

		if (token.type == kID                 ||
			token.type == kReservedCKeyword   ||
			token.type == kReservedCPPKeyword ||
			token.type == kBuiltInDataType    ||
			token.type == kString             ||
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
		else if (token.type == kPPDirective && SlurpPPComment(&(token.range)))
			{
			token.type = kComment;
			style      = GetTypeStyle(kComment - kWhitespace);
			}
		else if (token.type == kPPDirective)
			{
			style = GetStyle(typeIndex, text.GetSubstring(GetPPNameRange()));
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
 PreexpandCheckRange (virtual protected)

	Expand checkRange if the preceding text is #[[:space:]]*
	This catches "#includ" when typing 'e' and the token start happens
	to be after the #.

	modifiedRange is the range of text that was changed.
	deletion is kJTrue if the modification was that text was deleted.

 ******************************************************************************/

void
CBCStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JIndexRange&		modifiedRange,
	const JBoolean			deletion,
	JIndexRange*			checkRange
	)
{
	JIndex i = checkRange->first - 1;
	while (i > 1 && isspace(text.GetCharacter(i)))
		{
		i--;
		}
	if (i > 0 && text.GetCharacter(i) == '#')
		{
		checkRange->first = i;
		}
}

/******************************************************************************
 SlurpPPComment (private)

	Treat "#if 0 ... #(elif|else|endif)" as a comment.

 ******************************************************************************/

static const JRegex ppCommentPattern =
	"^[[:space:]]*#[[:space:]]*(el)?if[[:space:]]+0[[:space:]]*(//.*|/\\*[^*]*\\*/)?\n";

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
CBCStyler::SlurpPPComment
	(
	JIndexRange* totalRange
	)
{
	const JString& text = GetText();
	const JString s     = text.GetSubstring((GetPPNameRange()).first, totalRange->last);
	if (!ppCommentPattern.Match(s))
		{
		return kJFalse;
		}

	Token token;
	JString ppCmd;
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
			ppCmd = text.GetSubstring(GetPPNameRange());
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
				Undo(token.range, text.GetSubstring(token.range));	// rescan
				token.range.last = token.range.first - 1;
				break;
				}
			}
		}

	totalRange->last = token.range.last;
	return kJTrue;
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

#ifdef CODE_CRUSADER

	if (message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CBMWriteSharedPrefs(kJTrue);
			}
		}

#endif
}
