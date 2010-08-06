/******************************************************************************
 JColormap.h

	Interface for the JColormap class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JColormap
#define _H_JColormap

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jColor.h>
#include <JArray.h>

class JColormap
{
public:

	JColormap();

	virtual ~JColormap();

	virtual JBoolean	GetColor(const JCharacter* name, JColorIndex* colorIndex) const = 0;
	JColorIndex			GetColor(const JRGB& color) const;
	virtual JColorIndex	GetColor(const JSize red, const JSize green, const JSize blue) const = 0;

	JRGB				GetRGB(const JColorIndex colorIndex) const;
	virtual void		GetRGB(const JColorIndex colorIndex, JSize* red,
							   JSize* green, JSize* blue) const = 0;

	// pre-allocated colors

	virtual JColorIndex	GetBlackColor() const = 0;
	virtual JColorIndex	GetRedColor() const = 0;
	virtual JColorIndex	GetGreenColor() const = 0;
	virtual JColorIndex	GetYellowColor() const = 0;
	virtual JColorIndex	GetBlueColor() const = 0;
	virtual JColorIndex	GetMagentaColor() const = 0;
	virtual JColorIndex	GetCyanColor() const = 0;
	virtual JColorIndex	GetWhiteColor() const = 0;

	virtual JColorIndex	GetGrayColor(const JSize percentage) const = 0;

	virtual JColorIndex	GetDarkRedColor() const = 0;
	virtual JColorIndex	GetOrangeColor() const = 0;
	virtual JColorIndex	GetDarkGreenColor() const = 0;
	virtual JColorIndex	GetLightBlueColor() const = 0;
	virtual JColorIndex	GetBrownColor() const = 0;
	virtual JColorIndex	GetPinkColor() const = 0;

	virtual JColorIndex	GetDefaultSelectionColor() const = 0;

	virtual JColorIndex	GetDefaultBackColor() const = 0;
	virtual JColorIndex	GetDefaultFocusColor() const = 0;
	virtual JColorIndex	GetDefaultSliderBackColor() const = 0;
	virtual JColorIndex	GetInactiveLabelColor() const = 0;
	virtual JColorIndex	GetDefaultSelButtonColor() const = 0;
	virtual JColorIndex	GetDefaultDNDBorderColor() const = 0;

	virtual JColorIndex	Get3DLightColor() const = 0;
	virtual JColorIndex	Get3DShadeColor() const = 0;

private:

	// not allowed

	JColormap(const JColormap& source);
	const JColormap& operator=(const JColormap& source);
};


/******************************************************************************
 GetColor

 ******************************************************************************/

inline JColorIndex
JColormap::GetColor
	(
	const JRGB& color
	)
	const
{
	return GetColor(color.red, color.green, color.blue);
}

/******************************************************************************
 GetRGB

 ******************************************************************************/

inline JRGB
JColormap::GetRGB
	(
	const JColorIndex colorIndex
	)
	const
{
	JRGB color;
	GetRGB(colorIndex, &(color.red), &(color.green), &(color.blue));
	return color;
}

#endif
