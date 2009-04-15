/******************************************************************************
 JHTMLScanner.h

	Interface for the HTML scanner class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JHTMLScanner
#define _H_JHTMLScanner

#ifndef _H_JHTMLScannerL
#undef yyFlexLexer
#define yyFlexLexer JHTMLFlexLexer
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <FlexLexer.h>
#endif

#include <JStringPtrMap.h>
#include <JString.h>

class JHTMLScanner : public JHTMLFlexLexer
{
public:

	virtual ~JHTMLScanner();

	JString	GetScriptLanguage(const JStringPtrMap<JString>& attr) const;

public:

	enum HTMLError
	{
		kEmptyTag,
		kEmptyAttrName,
		kUnclosedTag,
		kCharacterOutOfRange
	};

protected:

	JHTMLScanner();

	void	LexHTML(const JCharacter* text);
	void	LexHTML(istream& input);

	virtual JBoolean	SwitchCharSet(const JCharacter* charSet) = 0;

	virtual JBoolean	HandleHTMLWord(const JCharacter* word, const JIndexRange& range);
	virtual JBoolean	HandleHTMLWhitespace(const JCharacter* space, const JIndexRange& range);
	virtual JBoolean	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr,
									  const JIndexRange& range);
	virtual JBoolean	HandleHTMLScript(const JString& language, const JIndexRange& range);
	virtual JBoolean	HandleHTMLComment(const JIndexRange& range);
	virtual JBoolean	HandleHTMLError(const HTMLError err, const JCharacter* errStr,
										const JIndexRange& range);
	virtual void		HandleEmbeddedPHPTags(const JArray<JIndexRange>& list);

private:

	struct TagInfo
	{
		JString					name;
		JString					lastAttr;
		JString					valueBuffer;
		JStringPtrMap<JString>	attr;
		JIndexRange				range;

		TagInfo()
			:
			attr(JPtrArrayT::kDeleteAll)
		{ };
	};

private:

	JIndexRange	itsMatchRange;			// range of current token

	JString*	itsDefaultCharSet;		// NULL when not lexing to save space

	JBoolean	itsTagOpenFlag;			// kJTrue while accumulating tag
	TagInfo*	itsTagInfo;				// NULL when not lexing to save space

	JArray<JIndexRange>*	itsEmbeddedPHPRangeList;	// NULL when not lexing to save space
	JIndexRange				itsPHPRange;				// range of PHP tag

private:

	virtual int	yylex();

	void		NewTag();
	JBoolean	TagFinished();
	void		SaveAttributeValue();

	void		BeginPHP(const JCharacter* text, const JSize length);
	JBoolean	PHPFinished(const JCharacter* text, const JSize length,
							const JBoolean closed);

	JBoolean	IsScript(JString* language, JIndex* first);

	JBoolean	HandleChar(const JCharacter c);
	JBoolean	HandleGreekChar(const JCharacter c);
	JBoolean	HandleLatinChar(const JCharacter c, const JIndex latinIndex,
								const JCharacter* latin1Str);
	void		UpdatePosition();

	// not allowed

	JHTMLScanner(const JHTMLScanner& source);
	const JHTMLScanner& operator=(const JHTMLScanner& source);
};


/******************************************************************************
 UpdatePosition (private)

 ******************************************************************************/

inline void
JHTMLScanner::UpdatePosition()
{
	const JIndex prevEnd = itsMatchRange.last;
	itsMatchRange.Set(prevEnd+1, prevEnd+yyleng);
}

#endif
