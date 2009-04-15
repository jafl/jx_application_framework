/******************************************************************************
 JHTMLScanner.cpp

	Base class for all objects that read HTML files.

	Derived classes must override:

		SwitchCharSet
			Switch the font to a different character set.
			Return kJFalse if the character set is not available.

	Information on HTML can be found at:

		http://home.netscape.com/assist/net_sites/index.html
		http://www.ncsa.uiuc.edu/General/Internet/WWW/HTMLPrimer.html
		http://home.netscape.com/assist/net_sites/html_extensions.html
		http://home.netscape.com/assist/net_sites/html_extensions_3.html

	BASE CLASS = JHTMLFlexLexer

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JHTMLScanner.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <strstream>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JHTMLScanner::JHTMLScanner()
	:
	JHTMLFlexLexer()
{
	itsTagInfo = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JHTMLScanner::~JHTMLScanner()
{
}

/******************************************************************************
 LexHTML (protected)

 ******************************************************************************/

void
JHTMLScanner::LexHTML
	(
	const JCharacter* text
	)
{
	std::istrstream input(text, strlen(text));
	LexHTML(input);
}

void
JHTMLScanner::LexHTML
	(
	istream& input
	)
{
	JString defaultCharSet = "iso8859-1";
	itsDefaultCharSet      = &defaultCharSet;

	TagInfo info;
	itsTagInfo     = &info;
	itsTagOpenFlag = kJFalse;

	itsMatchRange.Set(JTellg(input)+1, JTellg(input));

	JArray<JIndexRange> phpRangeList;
	itsEmbeddedPHPRangeList = &phpRangeList;
	itsPHPRange.SetToNothing();

	SwitchCharSet(*itsDefaultCharSet);
	FlexLexer::yylex(&input);

	if (itsTagOpenFlag)
		{
		TagFinished();
		HandleHTMLError(kUnclosedTag, "*** unclosed tag ***", JIndexRange());
		}

	HandleEmbeddedPHPTags(*itsEmbeddedPHPRangeList);

	itsDefaultCharSet       = NULL;
	itsTagInfo              = NULL;
	itsEmbeddedPHPRangeList = NULL;
}

/******************************************************************************
 NewTag (private)

 ******************************************************************************/

void
JHTMLScanner::NewTag()
{
	itsTagOpenFlag = kJTrue;

	(itsTagInfo->name).Clear();
	(itsTagInfo->lastAttr).Clear();
	(itsTagInfo->valueBuffer).Clear();
	(itsTagInfo->attr).CleanOut();
	itsTagInfo->range.first = itsMatchRange.first;
}

/******************************************************************************
 TagFinished (private)

 ******************************************************************************/

JBoolean
JHTMLScanner::TagFinished()
{
	assert( itsTagOpenFlag );

	JBoolean result = kJTrue;

	(itsTagInfo->range).last = itsMatchRange.last;
	if ((itsTagInfo->name).IsEmpty())
		{
		result = HandleHTMLError(kEmptyTag, "*** empty tag ***", itsTagInfo->range);
		}
	else
		{
		result = HandleHTMLTag(itsTagInfo->name, itsTagInfo->attr, itsTagInfo->range);
		}

	itsTagOpenFlag = kJFalse;
	return result;
}

/******************************************************************************
 SaveAttributeValue (private)

	Stores the contents of valueBuffer as the value of the latest
	attribute.

 ******************************************************************************/

void
JHTMLScanner::SaveAttributeValue()
{
	JString* value = new JString(itsTagInfo->valueBuffer);
	assert( value != NULL );
	const JBoolean ok = (itsTagInfo->attr).SetOldElement(itsTagInfo->lastAttr, value,
														 JPtrArrayT::kDelete);
	assert( ok );

	(itsTagInfo->lastAttr).Clear();
	(itsTagInfo->valueBuffer).Clear();
}

/******************************************************************************
 BeginPHP (private)

	Saves the start of the PHP tag.

 ******************************************************************************/

void
JHTMLScanner::BeginPHP
	(
	const JCharacter*	text,
	const JSize			length
	)
{
	itsPHPRange.first = itsMatchRange.first;

	if (yy_top_state() == 0)
		{
		(itsTagInfo->name).Set(text, length);
		(itsTagInfo->name).RemoveSubstring(1,1);
		}
}

/******************************************************************************
 PHPFinished (private)

	Stores the contents of the tag under the "code" key.

 ******************************************************************************/

JBoolean
JHTMLScanner::PHPFinished
	(
	const JCharacter*	text,
	const JSize			length,
	const JBoolean		closed
	)
{
	itsPHPRange.last = itsMatchRange.last;
	if (yy_top_state() != 0)
		{
		itsEmbeddedPHPRangeList->AppendElement(itsPHPRange);

		(itsTagInfo->valueBuffer).Append(text, length);
		return kJTrue;
		}

	(itsTagInfo->name).Set("?php");

	JString* value = new JString(itsTagInfo->valueBuffer);
	assert( value != NULL );
	value->TrimWhitespace();
	(itsTagInfo->attr).SetElement("code", value, JPtrArrayT::kDelete);

	return TagFinished();
}

/******************************************************************************
 IsScript (private)

	Extracts the language and the start of the script range and returns
	kJTrue if the last tag was the start of a script.

 ******************************************************************************/

JBoolean
JHTMLScanner::IsScript
	(
	JString*	language,
	JIndex*		first
	)
{
	language->Clear();

	if (itsTagInfo->name == "script")
		{
		*language = GetScriptLanguage(itsTagInfo->attr);
		*first    = itsMatchRange.last+1;
		return kJTrue;
		}
	else
		{
		*first = 0;
		return kJFalse;
		}
}

/******************************************************************************
 GetScriptLanguage

 ******************************************************************************/

JString
JHTMLScanner::GetScriptLanguage
	(
	const JStringPtrMap<JString>& attr
	)
	const
{
	JString* lang;
	if ((itsTagInfo->attr).GetElement("language", &lang) && lang != NULL)
		{
		return *lang;
		}
	else
		{
		return "JavaScript";
		}
}

/******************************************************************************
 HandleChar (private)

	If the character is part of an attribute value, we append it to the
	attribute value buffer.  Otherwise, we treat it like a normal word.

 ******************************************************************************/

JBoolean
JHTMLScanner::HandleChar
	(
	const JCharacter c
	)
{
	JBoolean result = kJTrue;
	if (yy_top_state() == 0)
		{
		const JCharacter str[] = { c, '\0' };
		result = HandleHTMLWord(str, itsMatchRange);
		}
	else
		{
		(itsTagInfo->valueBuffer).AppendCharacter(c);
		}

	yy_pop_state();
	return result;
}

/******************************************************************************
 HandleGreekChar (private)

	If the character is part of an attribute value, we append it to the
	attribute value buffer.  Otherwise, we treat it like a normal word.

 ******************************************************************************/

JBoolean
JHTMLScanner::HandleGreekChar
	(
	const JCharacter c
	)
{
	JBoolean result = kJTrue;
	if (yy_top_state() == 0)
		{
		itsTagInfo->name = "font";
		(itsTagInfo->attr).CleanOut();
		JString* value = new JString(JGetGreekFontName());
		assert( value != NULL );
		(itsTagInfo->attr).SetNewElement("face", value);
		result = HandleHTMLTag(itsTagInfo->name, itsTagInfo->attr,
							   JIndexRange(itsMatchRange.first, itsMatchRange.first-1));
		(itsTagInfo->name).Clear();
		(itsTagInfo->attr).CleanOut();

		const JCharacter str[] = { c, '\0' };
		result = JI2B(HandleHTMLWord(str, itsMatchRange) && result);

		itsTagInfo->name = "/font";
		result = JI2B(HandleHTMLTag(itsTagInfo->name, itsTagInfo->attr,
									JIndexRange(itsMatchRange.last+1, itsMatchRange.last)) &&
					  result);
		(itsTagInfo->name).Clear();
		}
	else
		{
		(itsTagInfo->valueBuffer).AppendCharacter(c);
		}

	yy_pop_state();
	return result;
}

/******************************************************************************
 HandleLatinChar (private)

	If the character is part of an attribute value, we append it to the
	attribute value buffer.  Otherwise, we treat it like a normal word.

 ******************************************************************************/

JBoolean
JHTMLScanner::HandleLatinChar
	(
	const JCharacter	c,
	const JIndex		latinIndex,
	const JCharacter*	latin1Str
	)
{
	JBoolean result = kJTrue;
	if (yy_top_state() == 0)
		{
		JString charSet = "iso8859-" + JString(latinIndex, 0);
		if (SwitchCharSet(charSet))
			{
			const JCharacter str[] = { c, '\0' };
			result = HandleHTMLWord(str, itsMatchRange);
			SwitchCharSet(*itsDefaultCharSet);
			}
		else
			{
			SwitchCharSet(*itsDefaultCharSet);
			result = HandleHTMLWord(latin1Str, itsMatchRange);
			}
		}
	else
		{
		(itsTagInfo->valueBuffer).AppendCharacter(c);
		}

	yy_pop_state();
	return result;
}

/******************************************************************************
 Handle something (virtual protected)

	These functions are not pure virtual because some derived classes
	may not need to implement all of them.

	Return kJFalse to cancel lexing.

 ******************************************************************************/

JBoolean
JHTMLScanner::HandleHTMLWord
	(
	const JCharacter*	word,
	const JIndexRange&	range
	)
{
	return kJTrue;
}

JBoolean
JHTMLScanner::HandleHTMLWhitespace
	(
	const JCharacter*	space,
	const JIndexRange&	range
	)
{
	return kJTrue;
}

JBoolean
JHTMLScanner::HandleHTMLTag
	(
	const JString&					name,
	const JStringPtrMap<JString>&	attr,
	const JIndexRange&				range
	)
{
	return kJTrue;
}

JBoolean
JHTMLScanner::HandleHTMLScript
	(
	const JString&		language,
	const JIndexRange&	range
	)
{
	return kJTrue;
}

JBoolean
JHTMLScanner::HandleHTMLComment
	(
	const JIndexRange& range
	)
{
	return kJTrue;
}

JBoolean
JHTMLScanner::HandleHTMLError
	(
	const HTMLError		err,
	const JCharacter*	errStr,
	const JIndexRange&	range
	)
{
	return kJTrue;
}

/******************************************************************************
 HandleEmbeddedPHPTags (virtual protected)

	Called after lexing is finished to notify the derived class of
	PHP tags that were embedded inside HTML tags.

 ******************************************************************************/

void
JHTMLScanner::HandleEmbeddedPHPTags
	(
	const JArray<JIndexRange>& list
	)
{
}
