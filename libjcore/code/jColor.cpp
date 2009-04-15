/******************************************************************************
 jColor.cpp

	System independent color functions.

	JColorIndex is a system dependent value that is allocated at run time.
	Each component of an RGB value has 16 bits (0 to 65535, see JColormap.h).

	This file can expand to support other ways of specifying colors.
	Conversions between the formats are handled via cross-constructors.

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JColormap.h>
#include <JMinMax.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Routines for JRGB

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JRGB&		color
	)
{
	input >> color.red >> color.green >> color.blue;
	return input;
}

ostream&
operator<<
	(
	ostream&	output,
	const JRGB&	color
	)
{
	output << color.red << ' ' << color.green << ' ' << color.blue;
	return output;
}

/******************************************************************************
 JCompareRGBValues

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareRGBValues
	(
	const JRGB& c1,
	const JRGB& c2
	)
{
	if (c1.red < c2.red)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.red > c2.red)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else if (c1.green < c2.green)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.green > c2.green)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else if (c1.blue < c2.blue)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.blue > c2.blue)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 Routines for JHSV

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JHSV&		color
	)
{
	input >> color.hue >> color.saturation >> color.value;
	return input;
}

ostream&
operator<<
	(
	ostream&	output,
	const JHSV&	color
	)
{
	output << color.hue << ' ' << color.saturation << ' ' << color.value;
	return output;
}

/******************************************************************************
 JCompareHSVValues

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareHSVValues
	(
	const JHSV& c1,
	const JHSV& c2
	)
{
	if (c1.hue < c2.hue)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.hue > c2.hue)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else if (c1.saturation < c2.saturation)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.saturation > c2.saturation)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else if (c1.value < c2.value)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (c1.value > c2.value)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}

	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 Cross-constructors

	Based on code from Computer Graphics, Principles and Practice,
	by James D. Foley, et al., Addison-Wesley.

 ******************************************************************************/

JRGB::JRGB
	(
	const JHSV& color
	)
{
	if (color.saturation == 0)
		{
		red = green = blue =
			JRound(kJMaxRGBValueF * color.value / kJMaxHSVValueF);
		}
	else
		{
		const JFloat h = 6.0 * color.hue  / (kJMaxHSVValueF + 1.0);
		const JFloat s = color.saturation / kJMaxHSVValueF;
		const JFloat v = color.value      / kJMaxHSVValueF;

		const JIndex i = JLFloor(h);
		const JFloat f = h - i;
		const JFloat p = v * (1.0-s);
		const JFloat q = v * (1.0-(s*f));
		const JFloat t = v * (1.0-(s*(1.0-f)));
		JFloat r=0, g=0, b=0;
		switch (i)
			{
			case 0: r=v; g=t; b=p; break;
			case 1: r=q; g=v; b=p; break;
			case 2: r=p; g=v; b=t; break;
			case 3: r=p; g=q; b=v; break;
			case 4: r=t; g=p; b=v; break;
			case 5: r=v; g=p; b=q; break;
			}
		red   = JRound(kJMaxRGBValueF * r);
		green = JRound(kJMaxRGBValueF * g);
		blue  = JRound(kJMaxRGBValueF * b);
		}
}

JHSV::JHSV
	(
	const JRGB& color
	)
{
	const JSize max = JMax(color.red, JMax(color.green, color.blue));
	const JSize min = JMin(color.red, JMin(color.green, color.blue));

	value = JRound(kJMaxHSVValueF * max / kJMaxRGBValueF);
	if (max == 0 || max == min)
		{
		hue = saturation = 0;
		}
	else
		{
		const JFloat delta = max - min;
		saturation         = JRound(kJMaxHSVValueF * delta / max);

		JFloat h;
		const long r = color.red;
		const long g = color.green;
		const long b = color.blue;
		if (color.red == max)
			{
			h = (g - b) / delta;
			}
		else if (color.green == max)
			{
			h = 2.0 + (b - r) / delta;
			}
		else	// color.blue == max
			{
			h = 4.0 + (r - g) / delta;
			}
		if (h < 0.0)
			{
			h += 6.0;
			}
		hue = JRound(kJMaxHSVValueF * h / 6.0);
		}
}

#define JTemplateType JDynamicColorInfo
#include <JArray.tmpls>
#undef JTemplateType
