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
T JMin(const T x1, const T x2);

template <class T>
T JMax(const T x1, const T x2);

template <class T>
T JLimit(const T min, const T x, const T max);

#endif
