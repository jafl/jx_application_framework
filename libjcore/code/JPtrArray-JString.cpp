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
	output << ' ' << list.GetItemCount();

	for (const auto* s : list)
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

		map.SetItem(key, value, JPtrArrayT::kDelete);
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

	const JSize count = list1.GetItemCount();
	if (list2.GetItemCount() != count)
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
		matched.AppendItem(false);
	}

	for (i=1; i<=count; i++)
	{
		const JString* s1 = list1.GetItem(i);
		bool found    = false;

		for (JIndex j=1; j<=count; j++)
		{
			if (!matched.GetItem(j) &&
				JString::Compare(*s1, *(list2.GetItem(j)), caseSensitive) == 0)
			{
				found = true;
				matched.SetItem(j, true);
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

std::weak_ordering
JCompareStringsCaseSensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	return JIntToWeakOrdering(JString::Compare(*s1, *s2, JString::kCompareCase));
}

/******************************************************************************
 JCompareStringsCaseInsensitive

 ******************************************************************************/

std::weak_ordering
JCompareStringsCaseInsensitive
	(
	JString* const & s1,
	JString* const & s2
	)
{
	return JIntToWeakOrdering(JString::Compare(*s1, *s2, JString::kIgnoreCase));
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
	for (const auto* s1 : list)
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
