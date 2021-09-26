/******************************************************************************
 JColorManager.h

	Copyright (C) 1997-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JColorManager
#define _H_JColorManager

#include "jx-af/jcore/jColor.h"

class JString;

class JColorManager
{
public:

	JColorManager();

	virtual ~JColorManager();

	virtual bool	GetColorID(const JString& name, JColorID* colorID) = 0;
	static JColorID		GetColorID(const JRGB& color);
	static JRGB			GetRGB(const JColorID colorID);

	// pre-allocated colors

	static JColorID	GetBlackColor();
	static JColorID	GetRedColor();
	static JColorID	GetGreenColor();
	static JColorID	GetYellowColor();
	static JColorID	GetBlueColor();
	static JColorID	GetMagentaColor();
	static JColorID	GetCyanColor();
	static JColorID	GetWhiteColor();

	static JColorID	GetGrayColor(const JSize percentage);

	static JColorID	GetDarkRedColor();
	static JColorID	GetOrangeColor();
	static JColorID	GetDarkGreenColor();
	static JColorID	GetLightBlueColor();
	static JColorID	GetBrownColor();
	static JColorID	GetPinkColor();

	static JColorID	GetDefaultSelectionColor();

	static JColorID	GetDefaultBackColor();
	static JColorID	GetDefaultFocusColor();
	static JColorID	GetDefaultSliderBackColor();
	static JColorID	GetInactiveLabelColor();
	static JColorID	GetDefaultSelButtonColor();
	static JColorID	GetDefaultDNDBorderColor();

	static JColorID	Get3DLightColor();
	static JColorID	Get3DShadeColor();

private:

	// not allowed

	JColorManager(const JColorManager&) = delete;
	JColorManager& operator=(const JColorManager&) = delete;
};

#endif
