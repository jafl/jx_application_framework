/******************************************************************************
 JXColormap.h

	Copyright (C) 1996-2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXColormap
#define _H_JXColormap

#include <JColormap.h>
#include <JBroadcaster.h>
#include <jXConstants.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;

class JXColormap : public JColormap, public virtual JBroadcaster
{
	friend class JXDisplay;

public:

	virtual ~JXColormap();

	operator Colormap() const;

	JXDisplay*	GetDisplay() const;
	Colormap	GetXColormap() const;
	XVisualInfo	GetXVisualInfo() const;
	Visual*		GetVisual() const;
	int			GetVisualClass() const;

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

	JXDisplay*	itsDisplay;		// not owned
	Colormap	itsXColormap;	// not owned
	Visual*		itsVisual;		// not owned
	XVisualInfo	itsXVisualInfo;

	// for calculating preallocated X pixel values

	unsigned long	itsMask[3];
	short			itsStartIndex[3];
	short			itsEndIndex[3];

private:

	JXColormap(JXDisplay* display, Visual* visual, Colormap xColormap);

	void	InitMasks(const unsigned long redMask,
					  const unsigned long greenMask,
					  const unsigned long blueMask);

	// called by JXDisplay

	static JXColormap*	Create(JXDisplay* display);

	// not allowed

	JXColormap(const JXColormap& source);
	const JXColormap& operator=(const JXColormap& source);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXColormap::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 Cast to Colormap

 ******************************************************************************/

inline
JXColormap::operator Colormap()
	const
{
	return itsXColormap;
}

/******************************************************************************
 GetXColormap

 ******************************************************************************/

inline Colormap
JXColormap::GetXColormap()
	const
{
	return itsXColormap;
}

/******************************************************************************
 GetXVisualInfo

	Nobody wants a "const XVisualInfo*", but it is too dangerous to
	return a non-const pointer to our data.

 ******************************************************************************/

inline XVisualInfo
JXColormap::GetXVisualInfo()
	const
{
	return itsXVisualInfo;
}

/******************************************************************************
 GetVisual

 ******************************************************************************/

inline Visual*
JXColormap::GetVisual()
	const
{
	return itsVisual;
}

/******************************************************************************
 GetVisualClass

 ******************************************************************************/

inline int
JXColormap::GetVisualClass()
	const
{
	return itsXVisualInfo.c_class;
}

#endif
