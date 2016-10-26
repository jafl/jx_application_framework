/******************************************************************************
 JExtractHTMLTitle.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JExtractHTMLTitle
#define _H_JExtractHTMLTitle

#include <JHTMLScanner.h>

class JString;

class JExtractHTMLTitle : public JHTMLScanner
{
public:

	JExtractHTMLTitle();

	virtual ~JExtractHTMLTitle();

	JBoolean	ExtractTitle(const JString& text, JString* title);

protected:

	virtual JBoolean	HandleHTMLWord(const JUtf8Byte* word, const JIndexRange& range);
	virtual JBoolean	HandleHTMLWhitespace(const JUtf8Byte* space, const JIndexRange& range);
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
