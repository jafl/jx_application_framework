/******************************************************************************
 JStringMatch.cpp

	Stores information about a successful match by a JStringIterator.

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JStringMatch.h"
#include "jx-af/jcore/JRegex.h"
#include "jx-af/jcore/jAssert.h"

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
	itsRegex(nullptr),
	itsSubmatchList(nullptr)
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

JStringMatch::JStringMatch
	(
	const JString&	target,
	JStringMatch&	dyingSource
	)
	:
	itsTarget(target),
	itsByteRange(dyingSource.itsByteRange),
	itsCharacterRange(dyingSource.itsCharacterRange),
	itsRegex(dyingSource.itsRegex),
	itsSubmatchList(dyingSource.itsSubmatchList)
{
	dyingSource.itsSubmatchList = nullptr;
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
	itsRegex(nullptr),
	itsSubmatchList(nullptr)
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

JStringMatch&
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

	if (source.itsSubmatchList == nullptr)
	{
		jdelete itsSubmatchList;
		itsSubmatchList = nullptr;
	}
	else if (itsSubmatchList == nullptr)
	{
		itsSubmatchList = jnew JArray<JUtf8ByteRange>(*(source.itsSubmatchList));
		assert( itsSubmatchList != nullptr );
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
	else if (itsSubmatchList != nullptr && itsSubmatchList->IndexValid(submatchIndex))
	{
		const JUtf8ByteRange ur = itsSubmatchList->GetElement(submatchIndex);
		if (!ur.IsEmpty())
		{
			ComputeCharacterRange();

			JCharacterRange cr;
			cr.SetFirstAndCount(
				itsCharacterRange.first +
					JString::CountCharacters(itsTarget.GetRawBytes() + itsByteRange.first - 1,
											 ur.first - itsByteRange.first),
				JString::CountCharacters(itsTarget.GetRawBytes() + ur.first - 1, ur.GetCount()));

			return cr;
		}
	}

	return JCharacterRange();
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
	if (itsSubmatchList != nullptr && itsSubmatchList->IndexValid(index))
	{
		const JUtf8ByteRange r = itsSubmatchList->GetElement(index);
		if (!r.IsEmpty())
		{
			return JString(itsTarget.GetRawBytes(), r, JString::kNoCopy);
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
	if (itsRegex != nullptr && itsRegex->GetSubexpressionIndex(name, &i))
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
	if (itsByteRange.IsEmpty())
	{
		return;
	}

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
