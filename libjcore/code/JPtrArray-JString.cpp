/******************************************************************************
 JPtrArray-JString.cpp

	Comparison functions for JString, for use with JPtrArray<JString>.

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include "JPtrArray-JString.h"
#include "jAssert.h"

/******************************************************************************
 Stream operators: JPtrArray<JString>

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
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
		JString* s = jnew JString;
		assert( s != nullptr );

		input >> *s;
		if (input.fail())
			{
			jdelete s;
			return input;
			}

		list.Append(s);
		}

	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const JPtrArray<JString>&	list
	)
{
	output << ' ' << list.GetElementCount();

	for (JString* s : list)
		{
		output << ' ' << *s;
		}

	output << ' ';
	return output;
}

/******************************************************************************
 Stream operators: JStringPtrMap<JString>

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&			input,
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

std::ostream&
operator<<
	(
	std::ostream&					output,
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
		const JString* s1 = list1.GetElement(i);
		JBoolean found    = kJFalse;

		for (JIndex j=1; j<=count; j++)
			{
			if (!matched.GetElement(j) &&
				JString::Compare(*s1, *(list2.GetElement(j)), caseSensitive) == 0)
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

JListT::CompareResult
JCompareStringsCaseSensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const int r = JString::Compare(*s1, *s2, kJTrue);

	if (r > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 JCompareStringsCaseInsensitive

 ******************************************************************************/

JListT::CompareResult
JCompareStringsCaseInsensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const int r = JString::Compare(*s1, *s2, kJFalse);

	if (r > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 Join

	It would be nice if this were a member function of JPtrArray, but it is
	only for strings, and g++ 7 complains when T is an abstract base class.

 ******************************************************************************/

JString
JStringJoin
	(
	const JString&				separator,
	const JPtrArray<JString>&	list
	)
{
	return JStringJoin(separator.GetBytes(), list);
}

JString
JStringJoin
	(
	const JUtf8Byte*			separator,
	const JPtrArray<JString>&	list
	)
{
	JString s;

	JBoolean first = kJTrue;
	for (JString* s1 : list)
		{
		if (!first)
			{
			s += separator;
			}

		s    += *s1;
		first = kJFalse;
		}

	return s;
}
