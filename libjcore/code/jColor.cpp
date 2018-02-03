/******************************************************************************
 jColor.cpp

	System independent color functions.

	JColorIndex is a system dependent value that is allocated at run time.
	Each component of an RGB value has 16 bits (0 to 65535, see JColormap.h).

	This file can expand to support other ways of specifying colors.
	Conversions between the formats are handled via cross-constructors.

	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#include <JColormap.h>
#include <JMinMax.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Routines for JRGB

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	JRGB&		color
	)
{
	input >> color.red >> color.green >> color.blue;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&	output,
	const JRGB&	color
	)
{
	output << color.red << ' ' << color.green << ' ' << color.blue;
	return output;
}

/******************************************************************************
 JCompareRGBValues

 ******************************************************************************/

JListT::CompareResult
JCompareRGBValues
	(
	const JRGB& c1,
	const JRGB& c2
	)
{
	if (c1.red < c2.red)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.red > c2.red)
		{
		return JListT::kFirstGreaterSecond;
		}

	else if (c1.green < c2.green)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.green > c2.green)
		{
		return JListT::kFirstGreaterSecond;
		}

	else if (c1.blue < c2.blue)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.blue > c2.blue)
		{
		return JListT::kFirstGreaterSecond;
		}

	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 Routines for JHSB

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	JHSB&		color
	)
{
	input >> color.hue >> color.saturation >> color.brightness;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&	output,
	const JHSB&	color
	)
{
	output << color.hue << ' ' << color.saturation << ' ' << color.brightness;
	return output;
}

/******************************************************************************
 JCompareHSBValues

 ******************************************************************************/

JListT::CompareResult
JCompareHSBValues
	(
	const JHSB& c1,
	const JHSB& c2
	)
{
	if (c1.hue < c2.hue)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.hue > c2.hue)
		{
		return JListT::kFirstGreaterSecond;
		}

	else if (c1.saturation < c2.saturation)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.saturation > c2.saturation)
		{
		return JListT::kFirstGreaterSecond;
		}

	else if (c1.brightness < c2.brightness)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1.brightness > c2.brightness)
		{
		return JListT::kFirstGreaterSecond;
		}

	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 Cross-constructors

	Based on code from Computer Graphics, Principles and Practice,
	by James D. Foley, et al., Addison-Wesley.

 ******************************************************************************/

JRGB::JRGB
	(
	const JHSB& color
	)
{
	if (color.saturation == 0)
		{
		red = green = blue =
			JRound(kJMaxRGBValueF * color.brightness / kJMaxHSBValueF);
		}
	else
		{
		const JFloat h = 6.0 * color.hue  / (kJMaxHSBValueF + 1.0);
		const JFloat s = color.saturation / kJMaxHSBValueF;
		const JFloat v = color.brightness / kJMaxHSBValueF;

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

JHSB::JHSB
	(
	const JRGB& color
	)
{
	const JSize max = JMax(color.red, JMax(color.green, color.blue));
	const JSize min = JMin(color.red, JMin(color.green, color.blue));

	brightness = JRound(kJMaxHSBValueF * max / kJMaxRGBValueF);
	if (max == 0 || max == min)
		{
		hue = saturation = 0;
		}
	else
		{
		const JFloat delta = max - min;
		saturation         = JRound(kJMaxHSBValueF * delta / max);

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
		hue = JRound(kJMaxHSBValueF * h / 6.0);
		}
}
