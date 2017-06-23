/******************************************************************************
 JStringMatch.cpp

						The JString Match Class

	Stores information about a successful match by a JStringIterator.  As
	with JStringIterator, this object is automatically updated when the
	underlying JString changes.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStringMatch.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	If a list of submatches is provided, we take ownership of the list.

	Privileged creators may also call one of Set(First|Last)CharacterIndex
	or SetCharacterRange.

 ******************************************************************************/

JStringMatch::JStringMatch
	(
	const JString& target
	)
	:
	itsTarget(target),
	itsRegex(NULL),
	itsSubmatchList(NULL)
{
}

// protected

JStringMatch::JStringMatch
	(
	const JString&			target,
	const JUtf8ByteRange&	byteRange,
	const JRegex*			regex,
	JArray<JUtf8ByteRange>*	list
	)
	:
	itsTarget(target),
	itsByteRange(byteRange),
	itsRegex(regex),
	itsSubmatchList(list)
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JStringMatch::JStringMatch
	(
	const JStringMatch& source
	)
	:
	itsTarget(source.itsTarget),
	itsRegex(NULL),
	itsSubmatchList(NULL)
{
	*this = source;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStringMatch::~JStringMatch()
{
	jdelete itsSubmatchList;
}

/******************************************************************************
 Assignment operator

 *****************************************************************************/

const JStringMatch&
JStringMatch::operator=
	(
	const JStringMatch& source
	)
{
	if (&source == this)
		{
		return *this;
		}

	assert( &itsTarget == &(source.itsTarget) );

	itsByteRange = source.itsByteRange;
	itsCharacterRange = source.itsCharacterRange;
	itsRegex = source.itsRegex;

	if (source.itsSubmatchList == NULL)
		{
		jdelete itsSubmatchList;
		itsSubmatchList = NULL;
		}
	else if (itsSubmatchList == NULL)
		{
		itsSubmatchList = jnew JArray<JUtf8ByteRange>(*(source.itsSubmatchList));
		assert( itsSubmatchList != NULL );
		}
	else
		{
		*itsSubmatchList = *(source.itsSubmatchList);
		}

	return *this;
}

/******************************************************************************
 SetFirstCharacterIndex (protected)

 ******************************************************************************/

void
JStringMatch::SetFirstCharacterIndex
	(
	const JIndex index
	)
{
	assert( itsCharacterRange.IsNothing() );
	assert( index > 0 );
	itsCharacterRange.first = index;
	// itsCharacterRange.last will be computed when needed
}

/******************************************************************************
 SetLastCharacterIndex (protected)

 ******************************************************************************/

void
JStringMatch::SetLastCharacterIndex
	(
	const JIndex index
	)
{
	assert( itsCharacterRange.IsNothing() );
	assert( index > 0 );

	const JSize count = JString::CountCharacters(itsTarget.GetRawBytes(), itsByteRange);
	assert( count <= index );

	itsCharacterRange.Set(index - count + 1, index);
}

/******************************************************************************
 SetCharacterRange (protected)

 ******************************************************************************/

void
JStringMatch::SetCharacterRange
	(
	const JCharacterRange& range
	)
{
	assert( itsCharacterRange.IsNothing() );
	assert( !range.IsEmpty() );

	itsCharacterRange = range;
}

/******************************************************************************
 GetCharacterCount

 ******************************************************************************/

JSize
JStringMatch::GetCharacterCount()
	const
{
	ComputeCharacterRange();
	return itsCharacterRange.GetCount();
}

/******************************************************************************
 GetCharacterRange

 ******************************************************************************/

JCharacterRange
JStringMatch::GetCharacterRange
	(
	const JIndex submatchIndex
	)
	const
{
	if (submatchIndex == 0)
		{
		ComputeCharacterRange();
		return itsCharacterRange;
		}
	else if (itsSubmatchList != NULL && itsSubmatchList->IndexValid(submatchIndex))
		{
		const JUtf8ByteRange ur = itsSubmatchList->GetElement(submatchIndex);

		JCharacterRange cr;
		cr.SetFirstAndCount(
			itsCharacterRange.first +
				JString::CountCharacters(itsTarget.GetRawBytes() + itsByteRange.first - 1,
										 ur.first - itsByteRange.first),
			JString::CountCharacters(itsTarget.GetRawBytes() + ur.first - 1, ur.GetCount()));

		return cr;
		}
	else
		{
		return JCharacterRange();
		}
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
JStringMatch::GetString()
	const
{
	return JString(itsTarget.GetRawBytes(), itsByteRange, kJFalse);
}

/******************************************************************************
 GetSubstring

 ******************************************************************************/

JString
JStringMatch::GetSubstring
	(
	const JIndex index
	)
	const
{
	if (itsSubmatchList != NULL && itsSubmatchList->IndexValid(index))
		{
		const JUtf8ByteRange r = itsSubmatchList->GetElement(index);
		if (!r.IsEmpty())
			{
			return JString(itsTarget.GetRawBytes(), r, kJFalse);
			}
		}

	return JString();
}

/******************************************************************************
 GetSubstring

 ******************************************************************************/

JString
JStringMatch::GetSubstring
	(
	const JUtf8Byte* name
	)
	const
{
	JIndex i;
	if (itsRegex != NULL && itsRegex->GetSubexpressionIndex(name, &i))
		{
		return GetSubstring(i);
		}
	else
		{
		return JString();
		}
}

/******************************************************************************
 ComputeCharacterRange (private)

	Updating itsCharacterRange.last does not change conceptual constness,
	because this is just an optimization.

 ******************************************************************************/

void
JStringMatch::ComputeCharacterRange()
	const
{
	if (itsCharacterRange.IsNothing())	// compute start index
		{
		const_cast<JCharacterRange*>(&itsCharacterRange)->first =
			JString::CountCharacters(itsTarget.GetRawBytes(), itsByteRange.first-1) + 1;
		}

	if (itsCharacterRange.IsEmpty())	// compute end index
		{
		const_cast<JCharacterRange*>(&itsCharacterRange)->last =
			itsCharacterRange.first +
			JString::CountCharacters(itsTarget.GetRawBytes(), itsByteRange) - 1;
		}
}
