/******************************************************************************
 JMinMax.h

	Interface for JMin and JMax templates

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMinMax
#define _H_JMinMax

template <class T>
T JMin(const T v1, const T v2);

template <class T, typename... Args>
T JMin(const T v1, const T v2, Args... more);

template <class T>
T JMax(const T v1, const T v2);

template <class T, typename... Args>
T JMax(const T v1, const T v2, Args... more);

template <class T>
T JLimit(const T min, const T v, const T max);

#include <JMinMax.tmpl>

#endif
