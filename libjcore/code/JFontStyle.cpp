/******************************************************************************
 JFontStyle.cpp

	System init code should call SetDefaultColorIndex so default constructor
	for JFontStyle will work correctly.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JFontStyle.h>
#include <JColorManager.h>

JColorID JFontStyle::itsDefaultColorIndex = JColorManager::GetBlackColor();

/******************************************************************************
 JFontStyle

 ******************************************************************************/

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
