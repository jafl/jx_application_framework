/******************************************************************************
 CBHTMLScanner.cpp

	HTML/PHP/XML lexer for keyword styling.

	BASE CLASS = public CBPHPFlexLexer

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
	CBPHPFlexLexer(),
	itsResetFlag(kJFalse)
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
	itsResetFlag  = kJTrue;
	itsStartState = startState;

	itsCurrentRange.charRange.SetToEmptyAt(startIndex.charIndex);
	itsCurrentRange.byteRange.SetToEmptyAt(startIndex.byteIndex);

	itsScriptLanguage.Clear();
	itsPHPHereDocTag.Clear();
	itsPHPNowDocTag.Clear();
	itsProbableJSOperatorFlag = kJFalse;

	switch_streams(&input, nullptr);
}

/******************************************************************************
 IsScript (private)

	Extracts the language and the start of the script range and returns
	kJTrue if the last tag was the start of a script.

 ******************************************************************************/

static JRegex scriptTagPattern1 =
	"^<[[:space:]]*script[[:space:]]+[^>]*language[[:space:]]*=[[:space:]]*\"?([^>\"[:space:]]+)";
static JRegex scriptTagPattern2 =
	"^<[[:space:]]*script[[:space:]]+[^>]*type[[:space:]]*=[[:space:]]*\"?[^/]+/([^>\"[:space:]]+)";
static JRegex scriptTagPattern3 =
	"^<[[:space:]]*script(>|[[:space:]])";

JBoolean
CBHTMLScanner::IsScript
	(
	JString* language
	)
	const
{
	scriptTagPattern1.SetCaseSensitive(kJFalse);
	scriptTagPattern2.SetCaseSensitive(kJFalse);
	scriptTagPattern3.SetCaseSensitive(kJFalse);

	language->Clear();

	const JString s(GetScannedText().GetRawBytes(), itsCurrentRange.byteRange, kJFalse);

	const JStringMatch m1 = scriptTagPattern1.Match(s, kJTrue);
	if (!m1.IsEmpty())
		{
		*language = m1.GetSubstring(1);
		return kJTrue;
		}

	const JStringMatch m2 = scriptTagPattern2.Match(s, kJTrue);
	if (!m2.IsEmpty())
		{
		*language = m2.GetSubstring(1);
		return kJTrue;
		}

	const JStringMatch m3 = scriptTagPattern3.Match(s, kJFalse);
	if (!m3.IsEmpty())
		{
		*language = "JavaScript";
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
