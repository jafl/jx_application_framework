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
#include <JXColorManager.h>
#include <jAssert.h>

CBCSharpStyler* CBCSharpStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JCharacter* kTypeNames[] =
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit C# Styles";

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
				 kEditDialogTitle, kCBCSharpStyleID, kCBCSharpFT),
	CBCSharpScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JXColorManager* colormap   = GetColormap();
	const JColorID red = colormap->GetRedColor();

	SetTypeStyle(kReservedCKeyword   - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kString             - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kCharConst          - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kDocComment         - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(colormap->GetBlueColor()));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	SetWordStyle("goto", JFontStyle(kJTrue, kJFalse, 0, kJFalse, red));

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
			token.type == kBuiltInDataType    ||
			token.type == kString             ||
			token.type == kComment            ||
			token.type == kDocComment)
			{
			SaveTokenStart(TokenExtra());
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
CBCSharpStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
