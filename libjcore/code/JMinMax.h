/******************************************************************************
 JMinMax.h

	Interface for JMin and JMax templates

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMinMax
#define _H_JMinMax

template <class T, class U>
	requires std::totally_ordered_with<T, U>
std::common_type_t<T, U>
JMin
	(
	const T v1,
	const U v2
	)
{
	return (v1 <= v2) ? v1 : v2;
}

template <class T, class U, typename... Args>
	requires std::totally_ordered_with<T, U>
std::common_type_t<T, U>
JMin
	(
	const T v1,
	const U v2,
	Args... more
	)
{
	const T v3 = JMin(v2, more...);
	return (v1 <= v3) ? v1 : v3;
}

template <class T, class U>
	requires std::totally_ordered_with<T, U>
std::common_type_t<T, U>
JMax
	(
	const T v1,
	const U v2
	)
{
	return (v1 >= v2) ? v1 : v2;
}

template <class T, class U, typename... Args>
	requires std::totally_ordered_with<T, U>
std::common_type_t<T, U>
JMax
	(
	const T v1,
	const U v2,
	Args... more
	)
{
	const T v3 = JMax(v2, more...);
	return (v1 >= v3) ? v1 : v3;
}

template <class T, class U>
	requires std::totally_ordered_with<T, U>
std::common_type_t<T, U>
JLimit
	(
	const T min,
	const U v,
	const T max
	)
{
	if (v <= min)
	{
		return min;
	}
	else if (v >= max)
	{
		return max;
	}
	else
	{
		return v;
	}
}

#endif
