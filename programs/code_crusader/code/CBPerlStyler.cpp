/******************************************************************************
 CBPerlStyler.cpp

	Helper object for CBTextEditor that displays Perl with styles to hilight
	keywords, comments, etc.

	BASE CLASS = CBStylerBase, CBPerlScanner

	Copyright (C) 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBPerlStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBPerlStyler* CBPerlStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 3;

	// version 3 inserts kFileGlob after kTransliteration (26)
	// version 2 inserts kReference after kPrototypeArgList (5)
	// version 1 inserts kPrototypeArgList after kSubroutine (4)

static const JCharacter* kTypeNames[] =
{
	"Scalar identifier",
	"List identifier",
	"Hash identifier",
	"Subroutine name (explicit via &)",
	"Subroutine prototype argument list",
	"Reference",
	"Reserved keyword",
	"Bareword",

	"Operator",
	"Delimiter",

	"Single quoted string",
	"Double quoted string",
	"Heredoc string",
	"Executed string",
	"Quoted word list",

	"Floating point constant",
	"Decimal constant",
	"Binary constant",
	"Octal constant",
	"Hexadecimal constant",
	"Version constant",

	"Regex search",
	"Regex replace",
	"One-shot regex search (?...?)",
	"Compiled regex",
	"Transliteration",

	"Readline / File Glob",

	"Comment",
	"POD",
	"Preprocessor directive",
	"Module data",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit Perl Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBPerlStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPerlStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPerlStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPerlStyler::CBPerlStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBPerlStyleID, kCBPerlFT),
	CBPerlScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap   = GetColormap();
	const JColorIndex red = colormap->GetRedColor();

	SetTypeStyle(kPrototypeArgList   - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse));
	SetTypeStyle(kReservedKeyword    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kSingleQuoteString  - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString  - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kHereDocString      - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kExecString         - kWhitespace, JFontStyle(colormap->GetPinkColor()));

	SetTypeStyle(kRegexSearch        - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kRegexReplace       - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kOneShotRegexSearch - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kCompiledRegex      - kWhitespace, JFontStyle(colormap->GetPinkColor()));
	SetTypeStyle(kTransliteration    - kWhitespace, JFontStyle(colormap->GetLightBlueColor()));
	SetTypeStyle(kFileGlob           - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kPOD                - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse, colormap->GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(colormap->GetBlueColor()));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPerlStyler::~CBPerlStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBPerlStyler::Scan
	(
	std::istream&			input,
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

		// save token starts -- must set itsProbableOperatorFlag

		if (token.type == kScalar            ||
			token.type == kList              ||
			token.type == kHash              ||
			token.type == kSubroutine        ||
			token.type == kReservedKeyword   ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString)
			{
			SaveTokenStart(TokenExtra());
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if (token.type == kSingleQuoteString  ||
				 token.type == kDoubleQuoteString  ||
				 token.type == kHereDocString      ||
				 token.type == kExecString         ||
				 token.type == kWordList           ||
				 token.type == kRegexSearch        ||
				 token.type == kRegexReplace       ||
				 token.type == kOneShotRegexSearch ||
				 token.type == kCompiledRegex      ||
				 token.type == kTransliteration    ||
				 token.type == kFileGlob           ||
				 token.type == kComment            ||
				 token.type == kPOD                ||
				 token.type == kModuleData)
			{
			style = GetTypeStyle(typeIndex);
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
	This catches "#lin" when typing 'e' and the token start happens
	to be after the #.

	modifiedRange is the range of text that was changed.
	deletion is kJTrue if the modification was that text was deleted.

 ******************************************************************************/

void
CBPerlStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JIndexRange&		modifiedRange,
	const JBoolean			deletion,
	JIndexRange*			checkRange
	)
{
	// We have to extend past any whitespace to include the next
	// real token.  This fixes:
	//		= 2 ? ... ?
	//		Delete 2 and then undo.  Forces restyling of range to not be regex.

	JIndex i = checkRange->last;
	if (!isspace(text.GetCharacter(i)))
		{
		i++;
		}
	const JIndex saved = i;

	while (i < text.GetLength() && isspace(text.GetCharacter(i)))
		{
		i++;
		}

	if (i > saved)
		{
		checkRange->last = i;
		}
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBPerlStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	JColormap* colormap = GetColormap();

	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(5, JFontStyle(kJTrue, kJFalse, 0, kJFalse));
		}

	if (vers < 2)
		{
		typeStyles->InsertElementAtIndex(6, typeStyles->GetElement(1));
		}

	if (vers < 3)
		{
		typeStyles->InsertElementAtIndex(27, JFontStyle(colormap->GetDarkGreenColor()));
		}

	// set new values after all new slots have been created
}
