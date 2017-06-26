/******************************************************************************
 TEColormap.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TEColormap
#define _H_TEColormap

#include <JColormap.h>

class TEColormap : public JColormap
{
public:

	TEColormap();

	virtual ~TEColormap();

	virtual JBoolean	GetColor(const JString& name, JColorIndex* colorIndex) const;
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

	TEColormap(const TEColormap& source);
	const TEColormap& operator=(const TEColormap& source);
};

#endif
