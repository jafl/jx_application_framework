/******************************************************************************
 JMinMax.h

	Interface for JMin and JMax templates

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JMinMax
#define _H_JMinMax

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

template <class T>
T JMin(const T v1, const T v2);

#if __GNUG__ < 4
template <class T>
T JMin(const T v1, const T v2, const T v3, const T v4, const T v5);
#else
template <class T, typename... Args>
T JMin(const T v1, const T v2, Args... more);
#endif

template <class T>
T JMax(const T v1, const T v2);

#if __GNUG__ < 4
// versions with more than 2 args
#else
template <class T, typename... Args>
T JMax(const T v1, const T v2, Args... more);
#endif

template <class T>
T JLimit(const T min, const T v, const T max);

#include <JMinMax.tmpl>

#endif
