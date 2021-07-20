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
		auto* s = jnew JString;
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

	Returns true if the two lists contain the same strings.
	This function does not assume that the lists are sorted.

 ******************************************************************************/

bool
JSameStrings
	(
	const JPtrArray<JString>&	list1,
	const JPtrArray<JString>&	list2,
	const JString::Case			caseSensitive
	)
{
JIndex i;

	const JSize count = list1.GetElementCount();
	if (list2.GetElementCount() != count)
		{
		return false;
		}
	else if (count == 0)
		{
		return true;
		}

	JArray<bool> matched(count);
	for (i=1; i<=count; i++)
		{
		matched.AppendElement(false);
		}

	for (i=1; i<=count; i++)
		{
		const JString* s1 = list1.GetElement(i);
		bool found    = false;

		for (JIndex j=1; j<=count; j++)
			{
			if (!matched.GetElement(j) &&
				JString::Compare(*s1, *(list2.GetElement(j)), caseSensitive) == 0)
				{
				found = true;
				matched.SetElement(j, true);
				break;
				}
			}

		if (!found)
			{
			return false;
			}
		}

	return true;
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
	const int r = JString::Compare(*s1, *s2, JString::kCompareCase);

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
	const int r = JString::Compare(*s1, *s2, JString::kIgnoreCase);

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
 JStringJoin

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

	bool first = true;
	for (JString* s1 : list)
		{
		if (!first)
			{
			s += separator;
			}

		s    += *s1;
		first = false;
		}

	return s;
}
