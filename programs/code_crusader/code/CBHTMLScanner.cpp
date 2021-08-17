/******************************************************************************
 CBHTMLScanner.cpp

	HTML/PHP/XML lexer for keyword styling.

	BASE CLASS = public CBHTMLFlexLexer

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#include "CBHTMLScanner.h"
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBHTMLScanner::CBHTMLScanner()
	:
	CBHTMLFlexLexer(),
	itsResetFlag(false)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBHTMLScanner::~CBHTMLScanner()
{
}

/******************************************************************************
 BeginScan

 *****************************************************************************/

void
CBHTMLScanner::BeginScan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const yy_state_type				startState
	)
{
	itsResetFlag  = true;
	itsStartState = startState;

	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	itsScriptLanguage.Clear();
	itsPHPHereDocTag.Clear();
	itsPHPNowDocTag.Clear();
	itsProbableJSOperatorFlag = false;

	switch_streams(&input, nullptr);
}

/******************************************************************************
 IsScript (private)

	Extracts the language and the start of the script range and returns
	true if the last tag was the start of a script.

 ******************************************************************************/

static JRegex scriptTagPattern1 =
	"^<[[:space:]]*script[[:space:]]+[^>]*language[[:space:]]*=[[:space:]]*\"?([^>\"[:space:]]+)";
static JRegex scriptTagPattern2 =
	"^<[[:space:]]*script[[:space:]]+[^>]*type[[:space:]]*=[[:space:]]*\"?[^/]+/([^>\"[:space:]]+)";
static JRegex scriptTagPattern3 =
	"^<[[:space:]]*script(>|[[:space:]])";

bool
CBHTMLScanner::IsScript
	(
	JString* language
	)
	const
{
	scriptTagPattern1.SetCaseSensitive(false);
	scriptTagPattern2.SetCaseSensitive(false);
	scriptTagPattern3.SetCaseSensitive(false);

	language->Clear();

	const JString s(GetScannedText().GetRawBytes(), itsCurrentRange.byteRange, JString::kNoCopy);

	const JStringMatch m1 = scriptTagPattern1.Match(s, JRegex::kIncludeSubmatches);
	if (!m1.IsEmpty())
		{
		*language = m1.GetSubstring(1);
		return true;
		}

	const JStringMatch m2 = scriptTagPattern2.Match(s, JRegex::kIncludeSubmatches);
	if (!m2.IsEmpty())
		{
		*language = m2.GetSubstring(1);
		return true;
		}

	const JStringMatch m3 = scriptTagPattern3.Match(s, JRegex::kIgnoreSubmatches);
	if (!m3.IsEmpty())
		{
		*language = "JavaScript";
		return true;
		}
	else
		{
		return false;
		}
}
