/******************************************************************************
 CBSearchColormap.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSearchColormap
#define _H_CBSearchColormap

#include <JColormap.h>

class CBSearchColormap : public JColormap
{
public:

	CBSearchColormap();

	virtual ~CBSearchColormap();

	virtual JBoolean	GetColor(const JCharacter* name, JColorIndex* colorIndex) const;
	virtual JColorIndex	GetColor(const JSize red, const JSize green, const JSize blue) const;
	virtual void		GetRGB(const JColorIndex colorIndex, JSize* red,
							   JSize* green, JSize* blue) const;

	// useful colors

	virtual JColorIndex	GetBlackColor() const;
	virtual JColorIndex	GetRedColor() const;
	virtual JColorIndex	GetGreenColor() const;
	virtual JColorIndex	GetYellowColor() const;
	virtual JColorIndex	GetBlueColor() const;
	virtual JColorIndex	GetMagentaColor() const;
	virtual JColorIndex	GetCyanColor() const;
	virtual JColorIndex	GetWhiteColor() const;

	virtual JColorIndex	GetGrayColor(const JSize percentage) const;

	virtual JColorIndex	GetDarkRedColor() const;
	virtual JColorIndex	GetOrangeColor() const;
	virtual JColorIndex	GetDarkGreenColor() const;
	virtual JColorIndex	GetLightBlueColor() const;
	virtual JColorIndex	GetBrownColor() const;
	virtual JColorIndex	GetPinkColor() const;

	virtual JColorIndex	GetDefaultSelectionColor() const;

	virtual JColorIndex	GetDefaultBackColor() const;
	virtual JColorIndex	GetDefaultFocusColor() const;
	virtual JColorIndex	GetDefaultSliderBackColor() const;
	virtual JColorIndex	GetInactiveLabelColor() const;
	virtual JColorIndex	GetDefaultSelButtonColor() const;
	virtual JColorIndex	GetDefaultDNDBorderColor() const;

	virtual JColorIndex	Get3DLightColor() const;
	virtual JColorIndex	Get3DShadeColor() const;

private:

	// not allowed

	CBSearchColormap(const CBSearchColormap& source);
	const CBSearchColormap& operator=(const CBSearchColormap& source);
};

#endif
