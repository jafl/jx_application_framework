/******************************************************************************
 JFontStyle.cpp

	System init code should call SetDefaultColorIndex so default constructor
	for JFontStyle will work correctly.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
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

#define JTemplateType JFontStyle
#include <JArray.tmpl>
#include <JRunArray.tmpls>
#include <JAuxTableData.tmpls>
#undef JTemplateType

#define JTemplateType JRunArray<JFontStyle>
#include <JPtrArray.tmpls>
#undef JTemplateType
