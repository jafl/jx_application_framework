/******************************************************************************
 CBHTMLStyler.cpp

	Helper object for CBTextEditor that displays HTML/PHP/XML with styles
	to hilight tags, scripts, etc.

	BASE CLASS = CBStylerBase, CBHTMLScanner

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBHTMLStyler.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <JColormap.h>
#include <ctype.h>
#include <jAssert.h>

CBHTMLStyler* CBHTMLStyler::itsSelf = NULL;

const JFileVersion kCurrentTypeListVersion = 7;

	// version 7 inserts kCDATABlock after kHTMLComment (5)
	// version 6 inserts kMustacheCommand after kHTMLComment (5)
	// version 5 inserts kJSPStartEnd, kJSPComment, kJavaID,
	//					 kJavaReservedKeyword, kJavaBuiltInDataType,
	//					 kJavaOperator, kJavaDelimiter, kJavaString,
	//					 kJavaCharConst after kPHPExecString (16)
	// version 4 inserts kJSRegexSearch after kJSString (21)
	// version 3 inserts kJSID, kJSReservedKeyword, kJSOperator,
	//					 kJSDelimiter, kJSString after kPHPExecString (16)
	//			 inserts kDocCommentHTMLTag, kDocCommentSpecialTag
	//					 after kComment (21)
	// version 2 inserts kHTMLText before kHTMLTag (1)
	//			 inserts 16 PHP types before kError (6)
	// version 1 inserts kHTMLScript after kHTMLTag (1)

static const JCharacter* kTypeNames[] =
{
	// HTML

	"Text",

	"HTML tag",
	"HTML script",
	"HTML special character",
	"HTML comment",
	"CDATA block",

	// Mustache

	"Mustache/Handlebars command",

	// PHP

	"PHP start/end marker",

	"PHP identifier",
	"PHP variable",
	"PHP reserved keyword",
	"PHP built-in data type",

	"PHP operator",
	"PHP delimiter",

	"PHP single quoted string",
	"PHP double quoted string",
	"PHP heredoc string",
	"PHP executed string",

	// JSP/Java

	"JSP start/end marker",
	"JSP comment",

	"Java identifier",
	"Java reserved keyword",
	"Java built-in data type",

	"Java operator",
	"Java delimiter",

	"Java string",
	"Java character constant",

	// JavaScript

	"JavaScript identifier",
	"JavaScript reserved keyword",

	"JavaScript operator",
	"JavaScript delimiter",

	"JavaScript string",
	"JavaScript regular expression",

	// shared

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",
	"Octal PHP constant",

	"Comment",
	"HTML tag in javadoc comment",
	"@ tag in javadoc comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JCharacter*);

static const JCharacter* kUnusedJavaKeyword[] =
{
	"byvalue", "cast", "const", "future", "generic", "inner",
	"operator", "outer", "rest"
};

const JSize kUnusedJavaKeywordCount = sizeof(kUnusedJavaKeyword)/sizeof(JCharacter*);

static const JCharacter* kUnusedJSKeyword[] =
{
	"abstract", "debugger", "enum", "goto", "implements", "interface",
	"native", "protected", "synchronized", "throws", "transient", "volatile"
};

const JSize kUnusedJSKeywordCount = sizeof(kUnusedJSKeyword)/sizeof(JCharacter*);

static const JCharacter* kEditDialogTitle = "Edit HTML/PHP/XML Styles";

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStylerBase*
CBHTMLStyler::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBHTMLStyler;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBHTMLStyler::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

	We use kCBPHPFT so $ will be included as "character in word".

 ******************************************************************************/

CBHTMLStyler::CBHTMLStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 kEditDialogTitle, kCBHTMLStyleID, kCBPHPFT),
	CBHTMLScanner()
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	JColormap* colormap = GetColormap();

	SetTypeStyle(kHTMLTag            - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kHTMLScript         - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kHTMLNamedCharacter - kWhitespace, JFontStyle(kJFalse, kJFalse, 1, kJFalse));
	SetTypeStyle(kHTMLComment        - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kError              - kWhitespace, JFontStyle(colormap->GetRedColor()));

	InitMustacheTypeStyles();
	InitPHPTypeStyles();
	InitJSPTypeStyles();
	InitJavaScriptTypeStyles();

	const JColorIndex red = colormap->GetRedColor();
	for (JIndex i=0; i<kUnusedJavaKeywordCount; i++)
		{
		SetWordStyle(kUnusedJavaKeyword[i], JFontStyle(red));
		}
	for (JIndex i=0; i<kUnusedJSKeywordCount; i++)
		{
		SetWordStyle(kUnusedJSKeyword[i], JFontStyle(red));
		}

	JPrefObject::ReadPrefs();

	JFontStyle style;
	if (GetWordStyle("?php", &style))
		{
		RemoveWordStyle("?php");
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBHTMLStyler::~CBHTMLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = NULL;
}

/******************************************************************************
 InitMustacheTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitMustacheTypeStyles()
{
	JColormap* colormap = GetColormap();

	SetTypeStyle(kMustacheCommand - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
}

/******************************************************************************
 InitPHPTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitPHPTypeStyles()
{
	JColormap* colormap = GetColormap();

	SetTypeStyle(kPHPStartEnd          - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse, colormap->GetDarkRedColor()));
	SetTypeStyle(kPHPReservedKeyword   - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kPHPBuiltInDataType   - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kPHPSingleQuoteString - kWhitespace, JFontStyle(colormap->GetBrownColor()));
	SetTypeStyle(kPHPDoubleQuoteString - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kPHPHereDocString     - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kPHPExecString        - kWhitespace, JFontStyle(colormap->GetPinkColor()));
	SetTypeStyle(kComment              - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
}

/******************************************************************************
 InitJSPTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitJSPTypeStyles()
{
	JColormap* colormap = GetColormap();

	SetTypeStyle(kJSPStartEnd         - kWhitespace, JFontStyle(kJTrue, kJFalse, 0, kJFalse, colormap->GetDarkGreenColor()));
	SetTypeStyle(kJSPComment          - kWhitespace, JFontStyle(colormap->GetGrayColor(50)));
	SetTypeStyle(kJavaReservedKeyword - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kJavaBuiltInDataType - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kJavaString          - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kJavaCharConst       - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
}

/******************************************************************************
 InitJavaScriptTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitJavaScriptTypeStyles()
{
	JColormap* colormap = GetColormap();

	SetTypeStyle(kJSReservedKeyword    - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kJSString             - kWhitespace, JFontStyle(colormap->GetDarkRedColor()));
	SetTypeStyle(kJSRegexSearch        - kWhitespace, JFontStyle(colormap->GetDarkGreenColor()));
	SetTypeStyle(kDocCommentHTMLTag    - kWhitespace, JFontStyle(colormap->GetBlueColor()));
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(kJFalse, kJFalse, 1, kJFalse));
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBHTMLStyler::Scan
	(
	istream&			input,
	const TokenExtra&	initData
	)
{
	BeginScan(input, initData.lexerState);
	itsLatestTagName.Clear();

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

		if ((yy_start_stack_ptr == 0 &&
			 (token.type == kHTMLText    ||
			  token.type == kHTMLComment ||
			  token.type == kJSPComment  ||
			  (token.type == kHTMLTag &&
			   text.GetCharacter(token.range.first) == '<'))) ||
			(yy_start_stack_ptr > 0 && yy_top_state() == 0 &&
			 (token.type == kPHPID                ||
			  token.type == kPHPVariable          ||
			  token.type == kPHPReservedKeyword   ||
			  token.type == kPHPBuiltInDataType   ||
			  token.type == kPHPSingleQuoteString ||
			  token.type == kPHPDoubleQuoteString ||
			  token.type == kPHPHereDocString     ||
			  token.type == kPHPExecString        ||
			  token.type == kJavaID               ||
			  token.type == kJavaReservedKeyword  ||
			  token.type == kJavaBuiltInDataType  ||
			  token.type == kJavaString           ||
			  token.type == kJSID                 ||
			  token.type == kJSReservedKeyword    ||
			  token.type == kComment)))
			{
			TokenExtra data;
			data.lexerState = GetCurrentLexerState();
			SaveTokenStart(data);

//			const JString s = text.GetSubstring(token.range);
//			cout << yy_top_state() << ' ' << yy_start_stack_ptr << ": " << s << endl;
			}

		// handle special cases

		if (token.type == kPHPDoubleQuoteString ||
			token.type == kPHPExecString)
			{
			ExtendCheckRangeForString(token.range);
			}
		else if (token.type == kPHPStartEnd)
			{
			ExtendCheckRangeForPHPStartEnd(token.range);
			}
		else if (token.type == kJSPStartEnd)
			{
			ExtendCheckRangeForJSPStartEnd(token.range);
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFontStyle();
			}
		else if ((token.type == kHTMLNamedCharacter ||
				  token.type == kHTMLInvalidNamedCharacter) &&
				 GetCurrentLexerState() != 0)
			{
			style = GetStyle(kHTMLTag - kWhitespace, itsLatestTagName);
			if (token.type == kHTMLInvalidNamedCharacter)
				{
				style.strike = !style.strike;
				}
			else
				{
				style.underlineCount++;
				}
			}
		else if (token.type == kHTMLText             ||
				 token.type == kHTMLComment          ||
				 token.type == kCDATABlock           ||
				 token.type == kMustacheCommand      ||
				 token.type == kPHPStartEnd          ||
				 token.type == kPHPSingleQuoteString ||
				 token.type == kPHPDoubleQuoteString ||
				 token.type == kPHPHereDocString     ||
				 token.type == kPHPExecString        ||
				 token.type == kJSPStartEnd          ||
				 token.type == kJSPComment           ||
				 token.type == kJavaString           ||
				 token.type == kJSString             ||
				 token.type == kJSRegexSearch        ||
				 token.type == kComment)
			{
			style = GetTypeStyle(typeIndex);
			}
		else if (token.type < kWhitespace)
			{
			style = GetTypeStyle(kError - kWhitespace);
			}
		else if (token.type == kHTMLTag)
			{
			style = GetTagStyle(token.range, typeIndex);
			}
		else if (token.type == kHTMLScript)
			{
			style = GetStyle(typeIndex, *(token.language));
			}
		else if (token.type == kHTMLNamedCharacter)
			{
			JIndexRange r = token.range;
			r.first++;
			if (text.GetCharacter(r.last) == ';')
				{
				r.last--;
				}
			style = GetStyle(typeIndex, text.GetSubstring(r));
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

			style = GetStyle(typeIndex, text.GetSubstring(token.range));
			}

		keepGoing = SetStyle(token.range, style);

		if (token.type == kPHPDoubleQuoteString ||
			token.type == kPHPHereDocString     ||
			token.type == kPHPExecString)
			{
			StyleEmbeddedVariables(token);
			}
		}
		while (keepGoing);
}

/******************************************************************************
 GetFirstTokenExtraData (virtual protected)

 ******************************************************************************/

JTEStyler::TokenExtra
CBHTMLStyler::GetFirstTokenExtraData()
	const
{
	TokenExtra data;
	data.lexerState = 0;	// INITIAL
	return data;
}

/******************************************************************************
 ExtendCheckRangeForString (private)

	There is one case where modifying a string doesn't force a restyling:

	"x"$x"
	  ^
	Inserting the marked character tricks JTEStyler into not continuing
	because the style didn't change and it was at a style run boundary.

 ******************************************************************************/

void
CBHTMLStyler::ExtendCheckRangeForString
	(
	const JIndexRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.last+1);
}

/******************************************************************************
 ExtendCheckRangeForPHPStartEnd (private)

	If the token doesn't have the correct style, then everything following
	it needs to be rescanned.

 ******************************************************************************/

void
CBHTMLStyler::ExtendCheckRangeForPHPStartEnd
	(
	const JIndexRange& tokenRange
	)
{
	const JFontStyle& style = GetTypeStyle(kPHPStartEnd - kWhitespace);
	if ((GetStyles()).IndexValid(tokenRange.last) &&	// avoid crash if redoing all
		(style != (GetStyles()).GetElement(tokenRange.first).style ||
		 style != (GetStyles()).GetElement(tokenRange.last).style))
		{
		ExtendCheckRange((GetText()).GetLength());
		}
}

/******************************************************************************
 ExtendCheckRangeForJSPStartEnd (private)

	If the token doesn't have the correct style, then everything following
	it needs to be rescanned.

 ******************************************************************************/

void
CBHTMLStyler::ExtendCheckRangeForJSPStartEnd
	(
	const JIndexRange& tokenRange
	)
{
	const JFontStyle& style = GetTypeStyle(kJSPStartEnd - kWhitespace);
	if ((GetStyles()).IndexValid(tokenRange.last) &&	// avoid crash if redoing all
		(style != (GetStyles()).GetElement(tokenRange.first).style ||
		 style != (GetStyles()).GetElement(tokenRange.last).style))
		{
		ExtendCheckRange((GetText()).GetLength());
		}
}

/******************************************************************************
 GetTagStyle (private)

 ******************************************************************************/

static const JRegex tagNamePattern = "<[[:space:]]*([^>[:space:]][^>/[:space:]]*)";

JFontStyle
CBHTMLStyler::GetTagStyle
	(
	const JIndexRange&	tokenRange,
	const JIndex		typeIndex
	)
{
	const JString& text = GetText();

	JFontStyle style;
	JArray<JIndexRange> matchList;
	if (tagNamePattern.MatchWithin(text, tokenRange, &matchList))
		{
		itsLatestTagName = text.GetSubstring(matchList.GetElement(2));
		itsLatestTagName.ToLower();

		JString openTag;
		if (itsLatestTagName.GetFirstCharacter() == '/' &&
			itsLatestTagName.GetLength() > 1)
			{
			openTag = itsLatestTagName.GetSubstring(2, itsLatestTagName.GetLength());
			}

		JBoolean found = GetWordStyle(itsLatestTagName, &style);
		if (!found && !openTag.IsEmpty())
			{
			found = GetWordStyle(openTag, &style);
			}

		if (!found)
			{
			found = GetXMLStyle(itsLatestTagName, &style);
			}

		if (!found && !openTag.IsEmpty())
			{
			found = GetXMLStyle(openTag, &style);
			}

		if (!found)
			{
			style = GetTypeStyle(typeIndex);
			}
		}
	else if (text.GetCharacter(tokenRange.first) == '<')
		{
		itsLatestTagName.Clear();
		style = GetTypeStyle(typeIndex);
		}
	else
		{
		style = GetStyle(typeIndex, itsLatestTagName);
		}

	return style;
}

JBoolean
CBHTMLStyler::GetXMLStyle
	(
	const JString&	tagName,
	JFontStyle*		style
	)
{
	JIndex i;
	if (!tagName.LocateLastSubstring(":", &i))
		{
		return kJFalse;
		}

	// tag name takes priority over XML namespaces

	JString s;
	if (i < tagName.GetLength())
		{
		s = tagName.GetSubstring(i+1, tagName.GetLength());
		if (GetWordStyle(s, style))
			{
			itsLatestTagName = s;
			return kJTrue;
			}
		}

	do
		{
		s = tagName.GetSubstring(1, i);
		if (GetWordStyle(s, style))
			{
			itsLatestTagName = s;
			return kJTrue;
			}

		i--;	// skip past the one we found
		}
		while (itsLatestTagName.LocatePrevSubstring(":", &i));

	return kJFalse;
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

#define CBPHPStringID    "[[:alpha:]_][[:alnum:]_]*"
#define CBPHPStringNotID "[^[:alpha:]_]"

static JRegex emptyVariablePattern =
	"^\\$+(\\{\\}|" CBPHPStringID "(->" CBPHPStringNotID "|\\[\\]))";	// update special conditions in code below
static JRegex variablePattern =
	"^\\$+(\\{[^}]+\\}|" CBPHPStringID "(\\[[^]]+\\]|->" CBPHPStringID ")?)";

#undef CBPHPStringID

void
CBHTMLStyler::StyleEmbeddedVariables
	(
	const Token& token
	)
{
	emptyVariablePattern.SetSingleLine();
	variablePattern.SetSingleLine();

	const JString& text = GetText();

	JFontStyle varStyle = GetTypeStyle(token.type - kWhitespace);
	varStyle.underlineCount++;
	if (varStyle == GetTypeStyle(kPHPVariable - kWhitespace))
		{
		varStyle.underlineCount++;
		}

	JFontStyle errStyle = GetTypeStyle(kError - kWhitespace);
	errStyle.underlineCount++;
	if (errStyle == GetTypeStyle(kPHPVariable - kWhitespace))
		{
		errStyle.underlineCount++;
		}

	JIndexRange r = token.range, r1, r2;
	while (!r.IsEmpty())
		{
		const JCharacter c = text.GetCharacter(r.first);
		if (c == '\\')
			{
			r.first++;
			}
		else if (c == '$')
			{
			r1 = r - (r.first-1);
			if (emptyVariablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				const JCharacter c1 = text.GetCharacter(r2.last);
				if (c1 != '}' && c1 != ']')
					{
					r2.last--;
					}
				AdjustStyle(r2, errStyle);
				r.first = r2.last;
				}
			else if (variablePattern.MatchWithin(text.GetCString() + r.first-1, r1, &r2))
				{
				r2 += r.first-1;
				if (r2.first > 1 && text.GetCharacter(r2.first-1) == '{')
					{
					r2.first--;
					}
				AdjustStyle(r2, varStyle);
				r.first = r2.last;
				}
			}
		else if (c == '{' && r.first < r.last - 1 &&
				 text.GetCharacter(r.first+1) == '$' &&
				 text.GetCharacter(r.first+2) == '}')
			{
			r1.SetFirstAndLength(r.first, 3);
			AdjustStyle(r1, errStyle);
			r.first = r1.last;
			}

		r.first++;
		}
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CBHTMLStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	JColormap* cmap = GetColormap();

	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(2, JFontStyle(cmap->GetDarkRedColor()));
		}

	if (vers < 2)
		{
		typeStyles->InsertElementAtIndex(1, JFontStyle());

		for (JIndex i=1; i<=16; i++)
			{
			typeStyles->InsertElementAtIndex(6, JFontStyle());
			}
		}

	if (vers < 3)
		{
		typeStyles->InsertElementAtIndex(22, JFontStyle());
		typeStyles->InsertElementAtIndex(22, JFontStyle());

		typeStyles->InsertElementAtIndex(17, JFontStyle());
		typeStyles->InsertElementAtIndex(17, JFontStyle());
		typeStyles->InsertElementAtIndex(17, JFontStyle());
		typeStyles->InsertElementAtIndex(17, JFontStyle());
		typeStyles->InsertElementAtIndex(17, JFontStyle());
		}

	if (vers < 4)
		{
		typeStyles->InsertElementAtIndex(22, JFontStyle(cmap->GetDarkGreenColor()));
		}

	if (vers < 5)
		{
		for (JIndex i=1; i<=9; i++)
			{
			typeStyles->InsertElementAtIndex(17, JFontStyle());
			}
		}

	if (vers < 6)
		{
		typeStyles->InsertElementAtIndex(6, JFontStyle());
		}

	if (vers < 7)
		{
		typeStyles->InsertElementAtIndex(6, JFontStyle());
		}

	// set new values after all new slots have been created

	if (vers < 2)
		{
		InitPHPTypeStyles();
		}

	if (vers < 3)
		{
		InitJavaScriptTypeStyles();
		}

	if (vers < 5)
		{
		InitJSPTypeStyles();
		}

	if (vers < 6)
		{
		InitMustacheTypeStyles();
		}
}

/******************************************************************************
 GetScannedText (virtual protected)

 ******************************************************************************/

const JString&
CBHTMLStyler::GetScannedText()
	const
{
	return GetText();
}
