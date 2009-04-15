/******************************************************************************
 JTEHTMLScanner.cpp

	Helper class to lex HTML for JTextEditor.  By not inheriting from
	JHTMLScanner, JTextEditor objects avoid bloat because they do not
	permanently store the contents of yyFlexLexer.

	BASE CLASS = JHTMLScanner

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEHTMLScanner.h>
#include <JTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

	This is protected so only JTextEditor can construct us.

 ******************************************************************************/

JTEHTMLScanner::JTEHTMLScanner
	(
	JTextEditor* te
	)
	:
	JHTMLScanner(),
	itsTE(te)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEHTMLScanner::~JTEHTMLScanner()
{
}

/******************************************************************************
 Handle something (virtual protected)

 ******************************************************************************/

JBoolean
JTEHTMLScanner::HandleHTMLWord
	(
	const JCharacter*	word,
	const JIndexRange&	range
	)
{
	itsTE->HandleHTMLWord(word);
	return kJTrue;
}

JBoolean
JTEHTMLScanner::HandleHTMLWhitespace
	(
	const JCharacter*	space,
	const JIndexRange&	range
	)
{
	itsTE->HandleHTMLWhitespace(space);
	return kJTrue;
}

JBoolean
JTEHTMLScanner::HandleHTMLTag
	(
	const JString&					name,
	const JStringPtrMap<JString>&	attr,
	const JIndexRange&				range
	)
{
	itsTE->HandleHTMLTag(name, attr);
	return kJTrue;
}

JBoolean
JTEHTMLScanner::HandleHTMLError
	(
	const HTMLError		err,
	const JCharacter*	errStr,
	const JIndexRange&	range
	)
{
	itsTE->HandleHTMLError(errStr);
	return kJTrue;
}

/******************************************************************************
 SwitchCharSet (virtual protected)

 ******************************************************************************/

JBoolean
JTEHTMLScanner::SwitchCharSet
	(
	const JCharacter* charSet
	)
{
	return itsTE->SwitchHTMLCharSet(charSet);
}
