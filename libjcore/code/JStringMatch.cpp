/******************************************************************************
 JStringMatch.cpp

						The JString Match Class

	Stores information about a successful match by a JStringIterator.  As
	with JStringIterator, this object is automatically updated when the
	underlying JString changes.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStringMatch.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

	If a list of submatches is provided, we take ownership of the list.

	The creator must also call one if Set(First|Last)CharacterIndex

 ******************************************************************************/

JStringMatch::JStringMatch
	(
	const JString&			target,
	const JUtf8ByteRange&	byteRange,
	JArray<JUtf8ByteRange>*	list
	)
	:
	itsTarget(target),
	itsByteRange(byteRange),
	itsSubmatchList(list)
{
	assert( !itsByteRange.IsEmpty() );		// avoid potential infinite loops
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
	itsByteRange(source.itsByteRange),
	itsCharacterRange(source.itsCharacterRange),
	itsSubmatchList(NULL)
{
	if (source.itsSubmatchList != NULL)
		{
		itsSubmatchList = jnew JArray<JUtf8ByteRange>(*(source.itsSubmatchList));
		assert( itsSubmatchList != NULL );
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JStringMatch::~JStringMatch()
{
	jdelete itsSubmatchList;
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
	// itsCharacterRange.first will be computed when needed
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

	const JSize count = JString::CountCharacters(itsTarget.GetBytes(), itsByteRange);
	assert( count <= index );

	itsCharacterRange.Set(index - count + 1, index);
}

/******************************************************************************
 GetCharacterCount

 ******************************************************************************/

JSize
JStringMatch::GetCharacterCount()
	const
{
	FinishCharacterRange();
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
		FinishCharacterRange();
		return itsCharacterRange;
		}
	else if (itsSubmatchList != NULL && itsSubmatchList->IndexValid(submatchIndex))
		{
		const JUtf8ByteRange ur = itsSubmatchList->GetElement(submatchIndex);

		JCharacterRange cr;
		cr.SetFirstAndCount(
			itsCharacterRange.first +
				JString::CountCharacters(itsTarget.GetBytes() + itsByteRange.first - 1,
										 ur.first - itsByteRange.first),
			JString::CountCharacters(itsTarget.GetBytes() + ur.first - 1, ur.GetCount()));

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
	return JString(itsTarget.GetBytes(), itsByteRange);
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
		return JString(itsTarget.GetBytes(), itsSubmatchList->GetElement(index));
		}
	else
		{
		return JString();
		}
}

/******************************************************************************
 FinishCharacterRange (private)

	Updating itsCharacterRange.last does not change conceptual constness,
	because this is just an optimization.

 ******************************************************************************/

void
JStringMatch::FinishCharacterRange()
	const
{
	if (itsCharacterRange.IsEmpty())
		{
		const_cast<JCharacterRange*>(&itsCharacterRange)->last =
			itsCharacterRange.first +
			JString::CountCharacters(itsTarget.GetBytes(), itsByteRange) - 1;
		}
}
