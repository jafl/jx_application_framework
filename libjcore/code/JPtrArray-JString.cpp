/******************************************************************************
 JPtrArray-JString.cpp

	Comparison functions for JString, for use with JPtrArray<JString>.

	Copyright © 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

/******************************************************************************
 Stream operators: JPtrArray<JString>

 ******************************************************************************/

istream&
operator>>
	(
	istream&			input,
	JPtrArray<JString>&	list
	)
{
	list.CleanOut();

	JSize count;
	input >> count;
	if (input.eof() || input.fail())
		{
		return input;
		}

	for (JIndex i=1; i<=count; i++)
		{
		JString* s = new JString;
		assert( s != NULL );

		input >> *s;
		if (input.fail())
			{
			delete s;
			return input;
			}

		list.Append(s);
		}

	return input;
}

ostream&
operator<<
	(
	ostream&					output,
	const JPtrArray<JString>&	list
	)
{
	const JSize count = list.GetElementCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
		{
		output << ' ' << *(list.NthElement(i));
		}

	output << ' ';
	return output;
}

/******************************************************************************
 Stream operators: JStringPtrMap<JString>

 ******************************************************************************/

istream&
operator>>
	(
	istream&				input,
	JStringPtrMap<JString>&	map
	)
{
	map.CleanOut();

	JSize count;
	input >> count;
	if (input.eof() || input.fail())
		{
		return input;
		}

	JString key, value;
	for (JIndex i=1; i<=count; i++)
		{
		input >> key >> value;
		if (input.fail())
			{
			return input;
			}

		map.SetElement(key, value, JPtrArrayT::kDelete);
		}

	return input;
}

ostream&
operator<<
	(
	ostream&						output,
	const JStringPtrMap<JString>&	map
	)
{
	JSize count = 0;

	JStringPtrMapCursor<JString> cursor(&map);
	while (cursor.Next())
		{
		count++;
		}

	output << ' ' << count;

	cursor.Reset();
	JString s;
	while (cursor.Next())
		{
		s = cursor.GetKey();
		output << ' ' << s;
		output << ' ' << *(cursor.GetValue());
		}

	output << ' ';
	return output;
}

/******************************************************************************
 JSameStrings

	Returns kJTrue if the two lists contain the same strings.
	This function does not assume that the lists are sorted.

 ******************************************************************************/

JBoolean
JSameStrings
	(
	const JPtrArray<JString>&	list1,
	const JPtrArray<JString>&	list2,
	const JBoolean				caseSensitive
	)
{
JIndex i;

	const JSize count = list1.GetElementCount();
	if (list2.GetElementCount() != count)
		{
		return kJFalse;
		}
	else if (count == 0)
		{
		return kJTrue;
		}

	JArray<JBoolean> matched(count);
	for (i=1; i<=count; i++)
		{
		matched.AppendElement(kJFalse);
		}

	for (i=1; i<=count; i++)
		{
		const JString* s1 = list1.NthElement(i);
		JBoolean found    = kJFalse;

		for (JIndex j=1; j<=count; j++)
			{
			if (!matched.GetElement(j) &&
				JStringCompare(*s1, *(list2.NthElement(j)), caseSensitive) == 0)
				{
				found = kJTrue;
				matched.SetElement(j, kJTrue);
				break;
				}
			}

		if (!found)
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 JCompareStringsCaseSensitive

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareStringsCaseSensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const int r = JStringCompare(*s1, *s2, kJTrue);

	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 JCompareStringsCaseInsensitive

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareStringsCaseInsensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const int r = JStringCompare(*s1, *s2, kJFalse);

	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}
