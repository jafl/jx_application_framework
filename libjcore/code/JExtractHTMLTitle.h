/******************************************************************************
 JExtractHTMLTitle.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JExtractHTMLTitle
#define _H_JExtractHTMLTitle

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JHTMLScanner.h>

class JString;

class JExtractHTMLTitle : public JHTMLScanner
{
public:

	JExtractHTMLTitle();

	virtual ~JExtractHTMLTitle();

	JBoolean	ExtractTitle(const JCharacter* text, JString* title);

protected:

	virtual JBoolean	SwitchCharSet(const JCharacter* charSet);

	virtual JBoolean	HandleHTMLWord(const JCharacter* word, const JIndexRange& range);
	virtual JBoolean	HandleHTMLWhitespace(const JCharacter* space, const JIndexRange& range);
	virtual JBoolean	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr,
									  const JIndexRange& range);

private:

	JBoolean	itsInTitleFlag;	// kJTrue => collect words
	JBoolean	itsNeedWSFlag;	// kJTrue => insert whitespace before next word
	JString*	itsTitle;		// NULL unless lexing

private:

	// not allowed

	JExtractHTMLTitle(const JExtractHTMLTitle& source);
	const JExtractHTMLTitle& operator=(const JExtractHTMLTitle& source);
};

#endif
