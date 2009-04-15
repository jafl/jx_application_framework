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

	virtual JBoolean	AllocateStaticNamedColor(const JCharacter* name, JColorIndex* colorIndex) = 0;
	JBoolean			AllocateStaticColor(const JRGB& color, JColorIndex* colorIndex,
											JBoolean* exactMatch = NULL);
	virtual JBoolean	AllocateStaticColor(const JSize red, const JSize green,
											const JSize blue, JColorIndex* colorIndex,
											JBoolean* exactMatch = NULL) = 0;

	virtual JBoolean	CanAllocateDynamicColors() const = 0;
	JBoolean			AllocateDynamicColor(const JRGB& color, JColorIndex* colorIndex);
	virtual JBoolean	AllocateDynamicColor(const JSize red, const JSize green,
											 const JSize blue, JColorIndex* colorIndex) = 0;
	void				SetDynamicColor(const JDynamicColorInfo& info);
	void				SetDynamicColor(const JColorIndex colorIndex, const JRGB& color);
	virtual void		SetDynamicColor(const JColorIndex colorIndex, const JSize red,
										const JSize green, const JSize blue) = 0;
	virtual void		SetDynamicColors(const JArray<JDynamicColorInfo>& colorList);

	virtual void		UsingColor(const JColorIndex colorIndex) = 0;
	virtual void		DeallocateColor(const JColorIndex colorIndex) = 0;

	JRGB				GetRGB(const JColorIndex colorIndex) const;
	virtual void		GetRGB(const JColorIndex colorIndex, JSize* red,
							   JSize* green, JSize* blue) const = 0;

	virtual int			GetSystemColorIndex(const JColorIndex colorIndex) const = 0;

	virtual JBoolean	AllColorsPreallocated() const = 0;
	virtual void		PrepareForMassColorAllocation() = 0;
	virtual void		MassColorAllocationFinished() = 0;

	// pre-allocated colors

	virtual JColorIndex	GetBlackColor() const = 0;
	virtual JColorIndex	GetRedColor() const = 0;
	virtual JColorIndex	GetGreenColor() const = 0;
	virtual JColorIndex	GetYellowColor() const = 0;
	virtual JColorIndex	GetBlueColor() const = 0;
	virtual JColorIndex	GetMagentaColor() const = 0;
	virtual JColorIndex	GetCyanColor() const = 0;
	virtual JColorIndex	GetWhiteColor() const = 0;

	virtual JColorIndex	GetGray10Color() const = 0;
	virtual JColorIndex	GetGray20Color() const = 0;
	virtual JColorIndex	GetGray30Color() const = 0;
	virtual JColorIndex	GetGray40Color() const = 0;
	virtual JColorIndex	GetGray50Color() const = 0;
	virtual JColorIndex	GetGray60Color() const = 0;
	virtual JColorIndex	GetGray70Color() const = 0;
	virtual JColorIndex	GetGray80Color() const = 0;
	virtual JColorIndex	GetGray90Color() const = 0;

	virtual JColorIndex	GetGray25Color() const = 0;
	virtual JColorIndex	GetGray75Color() const = 0;

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
 AllocateStaticColor

	exactMatch can be NULL.

 ******************************************************************************/

inline JBoolean
JColormap::AllocateStaticColor
	(
	const JRGB&		color,
	JColorIndex*	colorIndex,
	JBoolean*		exactMatch
	)
{
	return AllocateStaticColor(color.red, color.green, color.blue,
							   colorIndex, exactMatch);
}

/******************************************************************************
 AllocateDynamicColor

 ******************************************************************************/

inline JBoolean
JColormap::AllocateDynamicColor
	(
	const JRGB&		color,
	JColorIndex*	colorIndex
	)
{
	return AllocateDynamicColor(color.red, color.green, color.blue, colorIndex);
}

/******************************************************************************
 SetDynamicColor

	Sets the rgb components for the specified color.

 ******************************************************************************/

inline void
JColormap::SetDynamicColor
	(
	const JDynamicColorInfo& info
	)
{
	SetDynamicColor(info.index, info.color.red, info.color.green, info.color.blue);
}

inline void
JColormap::SetDynamicColor
	(
	const JColorIndex	colorIndex,
	const JRGB&			color
	)
{
	SetDynamicColor(colorIndex, color.red, color.green, color.blue);
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
