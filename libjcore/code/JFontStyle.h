/******************************************************************************
 JFontStyle.h

	Style information for drawing text.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFontStyle
#define _H_JFontStyle

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <jColor.h>

typedef unsigned long	JFontID;

const JSize kJDefaultFontSize = 12;

struct JFontStyle
{
private:

	static JColorIndex itsDefaultColorIndex;

public:

	JBoolean			bold;
	JBoolean			italic;
	JSize				underlineCount;		// may change font height
	JBoolean			strike;
	JColorIndex			color;

	JFontStyle()
		:
		bold(kJFalse),
		italic(kJFalse),
		underlineCount(0),
		strike(kJFalse),
		color(itsDefaultColorIndex)
	{ };

	JFontStyle
		(
		const JBoolean		b,
		const JBoolean		i,
		const JSize			u,
		const JBoolean		s,
		const JColorIndex	c = JFontStyle::itsDefaultColorIndex
		)
		:
		bold(b),
		italic(i),
		underlineCount(u),
		strike(s),
		color(c)
	{ };

	JFontStyle
		(
		const JColorIndex c
		)
		:
		bold(kJFalse),
		italic(kJFalse),
		underlineCount(0),
		strike(kJFalse),
		color(c)
	{ };

	static void
	SetDefaultColorIndex
		(
		const JColorIndex c
		)
	{
		itsDefaultColorIndex = c;
	};
};

int operator==(const JFontStyle& s1, const JFontStyle& s2);

inline int
operator!=
	(
	const JFontStyle& s1,
	const JFontStyle& s2
	)
{
	return !(s1 == s2);
}

#endif
