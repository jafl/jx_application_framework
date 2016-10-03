/******************************************************************************
 JTEHTMLScanner.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEHTMLScanner
#define _H_JTEHTMLScanner

#include <JHTMLScanner.h>

class JTextEditor;

class JTEHTMLScanner : public JHTMLScanner
{
	friend class JTextEditor;

public:

	virtual ~JTEHTMLScanner();

protected:

	JTEHTMLScanner(JTextEditor* te);

	virtual JBoolean	HandleHTMLWord(const JCharacter* word, const JIndexRange& range);
	virtual JBoolean	HandleHTMLWhitespace(const JCharacter* space, const JIndexRange& range);
	virtual JBoolean	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr,
									  const JIndexRange& range);
	virtual JBoolean	HandleHTMLError(const HTMLError err, const JCharacter* errStr,
										const JIndexRange& range);

private:

	JTextEditor*	itsTE;		// owns us

private:

	// not allowed

	JTEHTMLScanner(const JTEHTMLScanner& source);
	const JTEHTMLScanner& operator=(const JTEHTMLScanner& source);
};

#endif
