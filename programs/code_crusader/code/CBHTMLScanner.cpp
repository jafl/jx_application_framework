/******************************************************************************
 CBHTMLScanner.cpp

	HTML/PHP/XML lexer for keyword styling.

	BASE CLASS = public CBPHPFlexLexer

	Copyright (C) 2001 by John Lindal.  All rights reserved.

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
	istream&			input,
	const yy_state_type	startState
	)
{
	itsResetFlag  = kJTrue;
	itsStartState = startState;
	itsCurrentRange.Set(JTellg(input)+1, JTellg(input));
	itsScriptLanguage.Clear();
	itsPHPHereDocTag.Clear();
	itsProbableJSOperatorFlag = kJFalse;

	switch_streams(&input, NULL);
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

	const JString& text = GetScannedText();
	const JCharacter* s = text.GetCString() + itsCurrentRange.first - 1;
	JIndexRange r       = itsCurrentRange - (itsCurrentRange.first - 1);

	JArray<JIndexRange> matchList;
	if (scriptTagPattern1.MatchWithin(s, r, &matchList))
		{
		*language = text.GetSubstring(matchList.GetElement(2) + (itsCurrentRange.first - 1));
		return kJTrue;
		}
	else if (scriptTagPattern2.MatchWithin(s, r, &matchList))
		{
		*language = text.GetSubstring(matchList.GetElement(2) + (itsCurrentRange.first - 1));
		return kJTrue;
		}
	else if (scriptTagPattern3.MatchWithin(s, r))
		{
		*language = "JavaScript";
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
