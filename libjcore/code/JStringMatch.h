/******************************************************************************
 JStringMatch.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStringMatch
#define _H_JStringMatch

#include <JString.h>
#include <JArray.h>

class JRegex;

class JStringMatch
{
	friend class JStringIterator;	// construction
	friend class JRegex;			// construction

public:

	~JStringMatch();

	JBoolean	IsEmpty() const;
	JSize		GetCharacterCount() const;
	JSize		GetByteCount() const;
	JSize		GetSubstringCount() const;

	JString	GetString() const;
	JString	GetSubstring(const JIndex submatchIndex) const;
	JString	GetSubstring(const JUtf8Byte* name) const;
	JString	GetSubstring(const JString& name) const;

	JCharacterRange	GetCharacterRange(const JIndex submatchIndex = 0) const;
	JUtf8ByteRange	GetUtf8ByteRange(const JIndex submatchIndex = 0) const;

protected:

	JStringMatch(const JString& target, const JUtf8ByteRange& byteRange,
				 JRegex* regex = NULL, JArray<JUtf8ByteRange>* list = NULL);

	void	SetFirstCharacterIndex(const JIndex index);
	void	SetLastCharacterIndex(const JIndex index);
	void	SetCharacterRange(const JCharacterRange& range);

private:

	const JString&			itsTarget;
	JUtf8ByteRange			itsByteRange;
	JCharacterRange			itsCharacterRange;	// empty if have not computed # of characters
	JRegex*					itsRegex;			// can be NULL
	JArray<JUtf8ByteRange>*	itsSubmatchList;	// can be NULL

private:

	void	ComputeCharacterRange() const;

	// not allowed

	JStringMatch(const JStringMatch& source);	// JStringMatches should never be stored
	const JStringMatch& operator=(const JStringMatch& source);
};


/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JStringMatch::IsEmpty()
	const
{
	return itsByteRange.IsEmpty();
}

/******************************************************************************
 GetByteCount

 ******************************************************************************/

inline JSize
JStringMatch::GetByteCount()
	const
{
	return itsByteRange.GetCount();
}

/******************************************************************************
 GetSubstringCount

 ******************************************************************************/

inline JSize
JStringMatch::GetSubstringCount()
	const
{
	return (itsSubmatchList == NULL ? 0 : itsSubmatchList->GetElementCount());
}

/******************************************************************************
 GetSubstring

 ******************************************************************************/

inline JString
JStringMatch::GetSubstring
	(
	const JString& name
	)
	const
{
	return GetSubstring(name.GetBytes());
}

/******************************************************************************
 GetUtf8ByteRange

 ******************************************************************************/

inline JUtf8ByteRange
JStringMatch::GetUtf8ByteRange
	(
	const JIndex submatchIndex
	)
	const
{
	return (submatchIndex == 0 ? itsByteRange :
			itsSubmatchList == NULL || !itsSubmatchList->IndexValid(submatchIndex) ? JUtf8ByteRange() :
			itsSubmatchList->GetElement(submatchIndex));
}

#endif
