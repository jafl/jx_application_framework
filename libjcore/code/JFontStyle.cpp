/******************************************************************************
 JFontStyle.cpp

	System init code should call SetDefaultColorIndex so default constructor
	for JFontStyle will work correctly.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFontStyle.h>

/******************************************************************************
 JFontStyle

 ******************************************************************************/

JColorIndex JFontStyle::itsDefaultColorIndex = 0;

int
operator==
	(
	const JFontStyle& s1,
	const JFontStyle& s2
	)
{
	return (s1.bold           == s2.bold &&
			s1.italic         == s2.italic &&
			s1.underlineCount == s2.underlineCount &&
			s1.strike         == s2.strike &&
			s1.color          == s2.color);
}
