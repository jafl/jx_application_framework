/******************************************************************************
 CBSearchColormap.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchColormap
#define _H_CBSearchColormap

#include <JXColorManager.h>

class CBSearchColormap : public JXColorManager
{
public:

	CBSearchColormap();

	virtual ~CBSearchColormap();

	virtual JBoolean	GetColor(const JCharacter* name, JColorID* colorIndex) const;
	virtual JColorID	GetColor(const JSize red, const JSize green, const JSize blue) const;
	virtual void		GetRGB(const JColorID colorIndex, JSize* red,
							   JSize* green, JSize* blue) const;

	// useful colors

	virtual JColorID	GetBlackColor() const;
	virtual JColorID	GetRedColor() const;
	virtual JColorID	GetGreenColor() const;
	virtual JColorID	GetYellowColor() const;
	virtual JColorID	GetBlueColor() const;
	virtual JColorID	GetMagentaColor() const;
	virtual JColorID	GetCyanColor() const;
	virtual JColorID	GetWhiteColor() const;

	virtual JColorID	GetGrayColor(const JSize percentage) const;

	virtual JColorID	GetDarkRedColor() const;
	virtual JColorID	GetOrangeColor() const;
	virtual JColorID	GetDarkGreenColor() const;
	virtual JColorID	GetLightBlueColor() const;
	virtual JColorID	GetBrownColor() const;
	virtual JColorID	GetPinkColor() const;

	virtual JColorID	GetDefaultSelectionColor() const;

	virtual JColorID	GetDefaultBackColor() const;
	virtual JColorID	GetDefaultFocusColor() const;
	virtual JColorID	GetDefaultSliderBackColor() const;
	virtual JColorID	GetInactiveLabelColor() const;
	virtual JColorID	GetDefaultSelButtonColor() const;
	virtual JColorID	GetDefaultDNDBorderColor() const;

	virtual JColorID	Get3DLightColor() const;
	virtual JColorID	Get3DShadeColor() const;

private:

	// not allowed

	CBSearchColormap(const CBSearchColormap& source);
	const CBSearchColormap& operator=(const CBSearchColormap& source);
};

#endif
