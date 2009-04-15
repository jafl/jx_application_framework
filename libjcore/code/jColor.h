/******************************************************************************
 jColor.h

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JColor
#define _H_JColor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JOrderedSet.h>
#include <jMath.h>

struct JHSV;

	// index into JColormap

typedef JIndex	JColorIndex;

	// color specified via red,green,blue

const JSize  kJMaxRGBValue  = 65535;
const JFloat kJMaxRGBValueF = 65535.0;	// often want to divide

struct JRGB
{
	JSize	red;
	JSize	green;
	JSize	blue;

	JRGB()
		:
		red(0), green(0), blue(0)
	{ };

	JRGB(const JSize r, const JSize g, const JSize b)
		:
		red(r), green(g), blue(b)
	{ };

	JRGB(const JHSV& color);

	void
	Set(const JSize r, const JSize g, const JSize b)
	{
		red   = r;
		green = g;
		blue  = b;
	};

	JRGB&
	operator*=(const JFloat v)
	{
		red   = JRound(red   * v);
		green = JRound(green * v);
		blue  = JRound(blue  * v);
		return *this;
	};

	JRGB&
	operator/=(const JFloat v)
	{
		red   = JRound(red   / v);
		green = JRound(green / v);
		blue  = JRound(blue  / v);
		return *this;
	};
};

istream& operator>>(istream& input, JRGB& color);
ostream& operator<<(ostream& output, const JRGB& color);

JOrderedSetT::CompareResult JCompareRGBValues(const JRGB& c1, const JRGB& c2);

inline JRGB
JBlend
	(
	const JRGB&		c1,
	const JRGB&		c2,
	const JFloat	alpha
	)
{
	return JRGB(
		JMax(0UL, JMin(kJMaxRGBValue, (JSize) JRound((1.0-alpha) * c1.red   + alpha * c2.red))),
		JMax(0UL, JMin(kJMaxRGBValue, (JSize) JRound((1.0-alpha) * c1.green + alpha * c2.green))),
		JMax(0UL, JMin(kJMaxRGBValue, (JSize) JRound((1.0-alpha) * c1.blue  + alpha * c2.blue))));
}

inline int
operator==
	(
	const JRGB& c1,
	const JRGB& c2
	)
{
	return (c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue);
}

inline int
operator!=
	(
	const JRGB& c1,
	const JRGB& c2
	)
{
	return (c1.red != c2.red || c1.green != c2.green || c1.blue != c2.blue);
}

inline JRGB
operator*
	(
	const JRGB&		c,
	const JFloat	v
	)
{
	return JRGB(JRound(c.red * v), JRound(c.green * v), JRound(c.blue * v));
}

inline JRGB
operator*
	(
	const JFloat	v,
	const JRGB&		c
	)
{
	return JRGB(JRound(c.red * v), JRound(c.green * v), JRound(c.blue * v));
}

inline JRGB
operator/
	(
	const JRGB&		c,
	const JFloat	v
	)
{
	return JRGB(JRound(c.red / v), JRound(c.green / v), JRound(c.blue / v));
}

	// color specified via hue,saturation,value

const JSize  kJMaxHSVValue  = 65535;
const JFloat kJMaxHSVValueF = 65535.0;

struct JHSV
{
	JSize	hue;
	JSize	saturation;
	JSize	value;

	JHSV()
		:
		hue(0), saturation(0), value(0)
	{ };

	JHSV(const JSize h, const JSize s, const JSize v)
		:
		hue(h), saturation(s), value(v)
	{ };

	JHSV(const JRGB& color);

	void
	Set(const JSize h, const JSize s, const JSize v)
	{
		hue        = h;
		saturation = s;
		value      = v;
	};
};

istream& operator>>(istream& input, JHSV& color);
ostream& operator<<(ostream& output, const JHSV& color);

JOrderedSetT::CompareResult JCompareHSVValues(const JHSV& c1, const JHSV& c2);

inline int
operator==
	(
	const JHSV& c1,
	const JHSV& c2
	)
{
	return (c1.hue == c2.hue && c1.saturation == c2.saturation && c1.value == c2.value);
}

inline int
operator!=
	(
	const JHSV& c1,
	const JHSV& c2
	)
{
	return (c1.hue != c2.hue || c1.saturation != c2.saturation || c1.value != c2.value);
}

	// information about an animated color

struct JDynamicColorInfo
{
	JColorIndex	index;
	JRGB		color;

	JDynamicColorInfo()
		:
		index(0), color()
	{ };

	JDynamicColorInfo(const JColorIndex i, const JSize r,
					  const JSize g, const JSize b)
		:
		index(i), color(r,g,b)
	{ };

	JDynamicColorInfo(const JColorIndex i, const JRGB& rgb)
		:
		index(i), color(rgb)
	{ };
};

#endif
