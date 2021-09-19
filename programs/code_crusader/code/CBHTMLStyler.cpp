/******************************************************************************
 CBHTMLStyler.cpp

	Helper object for CBTextEditor that displays HTML/PHP/XML with styles
	to hilight tags, scripts, etc.

	BASE CLASS = CBStylerBase, CB::Text::HTML::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBHTMLStyler.h"
#include "cbmUtil.h"
#include <JXDialogDirector.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

CBHTMLStyler* CBHTMLStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 9;

	// version 9 inserts kPHPNowDocString after kPHPHereDocString (17)
	// version 8 inserts kJSTemplateString after kJSString (32)
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

static const JUtf8Byte* kTypeNames[] =
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
	"PHP nowdoc string",
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
	"JavaScript template string",
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

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

static const JUtf8Byte* kUnusedJavaKeyword[] =
{
	"byvalue", "cast", "future", "generic", "inner",
	"operator", "outer", "rest"
};

const JSize kUnusedJavaKeywordCount = sizeof(kUnusedJavaKeyword)/sizeof(JUtf8Byte*);

static const JUtf8Byte* kUnusedJSKeyword[] =
{
	"debugger", "goto"
};

const JSize kUnusedJSKeywordCount = sizeof(kUnusedJSKeyword)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStylerBase*
CBHTMLStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBHTMLStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBHTMLStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

	We use kCBPHPFT so $ will be included as "character in word".

 ******************************************************************************/

CBHTMLStyler::CBHTMLStyler()
	:
	CBStylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CBHTMLStyler"),
				 kCBHTMLStyleID, kCBPHPFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
		{
		SetTypeStyle(i, blankStyle);
		}

	SetTypeStyle(kHTMLTag            - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kHTMLScript         - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kHTMLNamedCharacter - kWhitespace, JFontStyle(false, false, 1, false));
	SetTypeStyle(kHTMLComment        - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError              - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	InitMustacheTypeStyles();
	InitPHPTypeStyles();
	InitJSPTypeStyles();
	InitJavaScriptTypeStyles();

	const JColorID red = JColorManager::GetRedColor();
	for (JUnsignedOffset i=0; i<kUnusedJavaKeywordCount; i++)
		{
		SetWordStyle(JString(kUnusedJavaKeyword[i], JString::kNoCopy), JFontStyle(red));
		}
	for (JUnsignedOffset i=0; i<kUnusedJSKeywordCount; i++)
		{
		SetWordStyle(JString(kUnusedJSKeyword[i], JString::kNoCopy), JFontStyle(red));
		}

	JPrefObject::ReadPrefs();

	JFontStyle style;
	const JString phpOpen("?php", JString::kNoCopy);
	if (GetWordStyle(phpOpen, &style))
		{
		RemoveWordStyle(phpOpen);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBHTMLStyler::~CBHTMLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 InitMustacheTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitMustacheTypeStyles()
{
	SetTypeStyle(kMustacheCommand - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
}

/******************************************************************************
 InitPHPTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitPHPTypeStyles()
{
	SetTypeStyle(kPHPStartEnd          - kWhitespace, JFontStyle(true, false, 0, false, JColorManager::GetDarkRedColor()));
	SetTypeStyle(kPHPReservedKeyword   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kPHPBuiltInDataType   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kPHPSingleQuoteString - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kPHPDoubleQuoteString - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kPHPHereDocString     - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kPHPNowDocString      - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kPHPExecString        - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kComment              - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
}

/******************************************************************************
 InitJSPTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitJSPTypeStyles()
{
	SetTypeStyle(kJSPStartEnd         - kWhitespace, JFontStyle(true, false, 0, false, JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kJSPComment          - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kJavaReservedKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kJavaBuiltInDataType - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kJavaString          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kJavaCharConst       - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
}

/******************************************************************************
 InitJavaScriptTypeStyles (private)

 ******************************************************************************/

void
CBHTMLStyler::InitJavaScriptTypeStyles()
{
	SetTypeStyle(kJSReservedKeyword    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kJSString             - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kJSTemplateString     - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kJSRegexSearch        - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kDocCommentHTMLTag    - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(false, false, 1, false));
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CBHTMLStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(GetStyledText(), startIndex, input);
	itsLatestTagName.Clear();

	const JString& text = GetText();

	bool keepGoing;
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

		if (states_empty() &&
			(token.type == kHTMLText    ||
			 token.type == kHTMLComment ||
			 token.type == kJSPComment  ||
			 (token.type == kHTMLTag &&
			  GetCharacter(token.range.GetFirst()) == '<')))
			{
			SaveTokenStart(token.range.GetFirst());

//			const JString s = text.GetSubstring(token.range);
//			std::cout << yy_top_state() << ' ' << yy_start_stack_ptr << ": " << s << std::endl;
			}

		// handle special cases

		if (token.type == kPHPDoubleQuoteString ||
			token.type == kPHPExecString)
			{
			ExtendCheckRangeForString(token.range.charRange);
			}
		else if (token.type == kPHPStartEnd)
			{
			ExtendCheckRangeForLanguageStartEnd(kPHPStartEnd, token.range.charRange);
			}
		else if (token.type == kJSPStartEnd)
			{
			ExtendCheckRangeForLanguageStartEnd(kJSPStartEnd, token.range.charRange);
			}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
			{
			style = GetDefaultFont().GetStyle();
			}
		else if ((token.type == kHTMLNamedCharacter ||
				  token.type == kHTMLInvalidNamedCharacter) &&
				 top_state() != INITIAL)
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
				 token.type == kPHPNowDocString      ||
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
			style = GetTagStyle(token.range.byteRange, typeIndex);
			}
		else if (token.type == kHTMLScript)
			{
			style = GetStyle(typeIndex, *token.scriptLanguage);
			}
		else if (token.type == kHTMLNamedCharacter)
			{
			JUtf8ByteRange r = token.range.byteRange;
			r.first++;	// skip over &
			if (GetCharacter(token.range.GetLast(*GetStyledText())) == ';')
				{
				r.last--;
				}
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), r, JString::kNoCopy));
			}
		else
			{
			if (token.type == kDocCommentHTMLTag ||
				token.type == kDocCommentSpecialTag)
				{
				if (!token.docCommentRange.IsEmpty())
					{
					SetStyle(token.docCommentRange.charRange, GetTypeStyle(kComment - kWhitespace));
					}
				ExtendCheckRange(token.range.charRange.last+1);
				}

			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
			}

		keepGoing = SetStyle(token.range.charRange, style);

		if (token.type == kPHPDoubleQuoteString ||
			token.type == kPHPHereDocString     ||
			token.type == kPHPExecString)
			{
			StyleEmbeddedPHPVariables(token);
			}
		else if (token.type == kJSTemplateString)
			{
			StyleEmbeddedJSVariables(token);
			}
		}
		while (keepGoing);
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
	const JCharacterRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.last+1);
}

/******************************************************************************
 ExtendCheckRangeForLanguageStartEnd (private)

	If the token doesn't have the correct style, then everything following
	it needs to be rescanned.

 ******************************************************************************/

void
CBHTMLStyler::ExtendCheckRangeForLanguageStartEnd
	(
	const TokenType			tokenType,
	const JCharacterRange&	tokenRange
	)
{
	JFont f1, f2;

	JRunArrayIterator iter(GetStyles(), kJIteratorStartBefore, tokenRange.first);
	if (!iter.Next(&f1))
		{
		return;
		}
	iter.MoveTo(kJIteratorStartBefore, tokenRange.last);
	if (!iter.Next(&f2))
		{
		return;
		}

	const JFontStyle& style = GetTypeStyle(tokenType - kWhitespace);
	if (style != f1.GetStyle() || style != f1.GetStyle())
		{
		ExtendCheckRange(GetText().GetCharacterCount());
		}
}

/******************************************************************************
 GetTagStyle (private)

 ******************************************************************************/

static const JRegex tagNamePattern = "<[[:space:]]*(/?([^>/[:space:]]+))";

JFontStyle
CBHTMLStyler::GetTagStyle
	(
	const JUtf8ByteRange&	tokenRange,
	const JIndex			typeIndex
	)
{
	const JString s(GetText().GetBytes(), tokenRange, JString::kNoCopy);

	JFontStyle style;
	const JStringMatch m = tagNamePattern.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		itsLatestTagName = m.GetSubstring(1);
		itsLatestTagName.ToLower();

		JString openTag;
		if (itsLatestTagName.GetFirstCharacter() == '/' &&
			itsLatestTagName.GetCharacterCount() > 1)
			{
			openTag = m.GetSubstring(2);
			openTag.ToLower();
			}

		bool found = GetWordStyle(itsLatestTagName, &style);
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
	else if (s.GetFirstCharacter() == '<')
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

bool
CBHTMLStyler::GetXMLStyle
	(
	const JString&	tagName,
	JFontStyle*		style
	)
{
	JStringIterator iter(tagName);
	if (!iter.Next(":") || iter.AtEnd())
		{
		return false;
		}

	iter.BeginMatch();
	iter.MoveTo(kJIteratorStartAtEnd, 0);
	const JStringMatch m = iter.FinishMatch();
	if (GetWordStyle(m.GetString(), style))
		{
		itsLatestTagName = m.GetString();
		return true;
		}

	return false;
}

/******************************************************************************
 StyleEmbeddedPHPVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

#define CBPHPStringID    "(_|\\p{L})(_|\\p{L}|\\d)*"
#define CBPHPStringNotID "\\p{^L}"

static JRegex emptyPHPVariablePattern =
	"^\\$+(\\{\\}|" CBPHPStringID "(->(?=" CBPHPStringNotID ")|\\[\\]))";	// update special conditions in code below
static JRegex phpVariablePattern =
	"^\\$+(\\{[^}]+\\}|" CBPHPStringID "(\\[[^]]+\\]|->" CBPHPStringID ")*)";

#undef CBPHPStringID

void
CBHTMLStyler::StyleEmbeddedPHPVariables
	(
	const Token& token
	)
{
	emptyPHPVariablePattern.SetSingleLine();
	phpVariablePattern.SetSingleLine();

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

	const JString s(GetText().GetBytes(), token.range.byteRange);
	JStringIterator iter(s);
	JUtf8Character c;
	while (iter.Next(&c) && !iter.AtEnd())
		{
		if (c == '\\')
			{
			iter.SkipNext();
			}
		else if (c == '$')
			{
			iter.SkipPrev();

			JCharacterRange r = MatchAt(token, iter, phpVariablePattern);
			if (!r.IsEmpty())
				{
				if (iter.Prev(&c, kJIteratorStay) && c == '{')
					{
					r.first--;
					iter.SkipNext(r.GetCount()-1);
					if (iter.Next(&c, kJIteratorStay) && c == '}')
						{
						iter.SkipNext();
						r.last++;
						}
					}
				else
					{
					iter.SkipNext(r.GetCount());
					}
				AdjustStyle(r, varStyle);
				continue;
				}

			r = MatchAt(token, iter, emptyPHPVariablePattern);
			if (!r.IsEmpty())
				{
				AdjustStyle(r, errStyle);
				iter.SkipNext(r.GetCount());
				}
			}
		else if (c == '{' && iter.Next(&c) && c == '$')
			{
			if (iter.Next(&c, kJIteratorStay) && c == '}')
				{
				iter.SkipNext();
				const JIndex i = token.range.charRange.first - 1 + iter.GetPrevCharacterIndex();
				AdjustStyle(JCharacterRange(i-2, i), errStyle);
				}
			else
				{
				iter.SkipPrev();
				}
			}
		}
}

/******************************************************************************
 StyleEmbeddedJSVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

static JRegex variablePattern =      "(?<!\\\\)\\$\\{.+?\\}";
static JRegex emptyVariablePattern = "(?<!\\\\)\\$\\{\\}?";

#define ClassName CBHTMLStyler
#define FunctionName StyleEmbeddedJSVariables
#define NoVariableIndex
#include "CBSTStylerEmbeddedVariables.th"
#undef NoVariableIndex
#undef ClassName

/******************************************************************************
 MatchAt (private)

 ******************************************************************************/

JCharacterRange
CBHTMLStyler::MatchAt
	(
	const Token&		token,
	JStringIterator&	iter,
	const JRegex&		pattern
	)
{
	const JIndex orig = iter.GetNextCharacterIndex();

	iter.BeginMatch();
	iter.MoveTo(kJIteratorStartAtEnd, 0);
	const JStringMatch& m1 = iter.FinishMatch();
	const JStringMatch m2  = pattern.Match(m1.GetString(), JRegex::kIncludeSubmatches);

	iter.MoveTo(kJIteratorStartBefore, orig);

	return m2.IsEmpty() ? JCharacterRange() :
		JCharacterRange(
			m2.GetCharacterRange() +
				(token.range.charRange.first - 1 + iter.GetPrevCharacterIndex()));
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
	if (vers < 1)
		{
		typeStyles->InsertElementAtIndex(2, JFontStyle(JColorManager::GetDarkRedColor()));
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
		typeStyles->InsertElementAtIndex(22, JFontStyle(JColorManager::GetDarkGreenColor()));
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

	if (vers < 8)
		{
		typeStyles->InsertElementAtIndex(33, JFontStyle(JColorManager::GetPinkColor()));
		}

	if (vers < 9)
		{
		typeStyles->InsertElementAtIndex(18, JFontStyle(JColorManager::GetBrownColor()));
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
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
CBHTMLStyler::Receive
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


/******************************************************************************
 GetScannedText (virtual protected)

 ******************************************************************************/

const JString&
CBHTMLStyler::GetScannedText()
	const
{
	return GetText();
}
