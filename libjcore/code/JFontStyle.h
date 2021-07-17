/******************************************************************************
 JFontStyle.h

	Style information for drawing text.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFontStyle
#define _H_JFontStyle

#include "jTypes.h"
#include "jColor.h"

struct JFontStyle
{
private:

	static JColorID itsDefaultColorIndex;

public:

	bool			bold;
	bool			italic;
	JSize				underlineCount;		// may change font height
	bool			strike;
	JColorID			color;

	JFontStyle()
		:
		bold(false),
		italic(false),
		underlineCount(0),
		strike(false),
		color(itsDefaultColorIndex)
	{ };

	JFontStyle
		(
		const bool		b,
		const bool		i,
		const JSize			u,
		const bool		s,
		const JColorID	c = JFontStyle::itsDefaultColorIndex
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
		const JColorID c
		)
		:
		bold(false),
		italic(false),
		underlineCount(0),
		strike(false),
		color(c)
	{ };

	bool
	IsBlank()
		const
	{
		return !bold && !italic && underlineCount == 0 && !strike &&
					color == itsDefaultColorIndex;
	};

	bool
	SameSystemAttributes
		(
		const JFontStyle& style
		)
		const
	{
		return style.bold == bold && style.italic == italic;
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
