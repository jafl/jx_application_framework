/******************************************************************************
 JXColormap.h

	Interface for the JXColormap class

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXColormap
#define _H_JXColormap

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JColormap.h>
#include <JBroadcaster.h>
#include <jXConstants.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;

typedef long (*JXColorDistanceFn)(const long r1, const long g1, const long b1,
								  const long r2, const long g2, const long b2);

class JXColormap : public JColormap, public virtual JBroadcaster
{
	friend class JXDisplay;

public:

	static JBoolean	Create(JXDisplay* display, const int visualClass,
						   JXColormap** colormap);

	virtual ~JXColormap();

	operator Colormap() const;

	JXDisplay*	GetDisplay() const;
	Colormap	GetXColormap() const;
	XVisualInfo	GetXVisualInfo() const;
	Visual*		GetVisual() const;
	int			GetVisualClass() const;
	JSize		GetMaxColorCount() const;

	virtual JBoolean	AllocateStaticNamedColor(const JCharacter* name, JColorIndex* colorIndex);
	virtual JBoolean	AllocateStaticColor(const JSize red, const JSize green,
											const JSize blue, JColorIndex* colorIndex,
											JBoolean* exactMatch = NULL);
	void				AllocateStaticColor(const unsigned long xPixel, JColorIndex* colorIndex);

	virtual JBoolean	CanAllocateDynamicColors() const;
	virtual JBoolean	AllocateDynamicColor(const JSize red, const JSize green,
											 const JSize blue, JColorIndex* colorIndex);
	virtual void		SetDynamicColor(const JColorIndex colorIndex, const JSize red,
										const JSize green, const JSize blue);
	virtual void		SetDynamicColors(const JArray<JDynamicColorInfo>& colorList);

	virtual void		UsingColor(const JColorIndex colorIndex);
	virtual void		DeallocateColor(const JColorIndex colorIndex);

	virtual void		PrepareForMassColorAllocation();
	virtual void		MassColorAllocationFinished();

	// color approximation

	JBoolean	WillApproximateColors() const;
	void		ShouldApproximateColors(const JBoolean approx);

	JBoolean	WillPreemptivelyApproximateColors() const;
	void		ShouldPreemptivelyApproximateColors(const JBoolean preempt);

	JXColorDistanceFn	GetColorDistanceFunction() const;
	void				SetColorDistanceFunction(JXColorDistanceFn f);

	JSize	GetColorBoxHalfWidth() const;
	void	SetColorBoxHalfWidth(const JSize halfWidth);

	JSize	GetPreemptiveColorBoxHalfWidth() const;
	void	SetPreemptiveColorBoxHalfWidth(const JSize halfWidth);

	// color distance functions

	static long	XPMColorDistance(const long r1, const long g1, const long b1,
								 const long r2, const long g2, const long b2);

	// called by JXImage

	virtual JBoolean	AllColorsPreallocated() const;
	JBoolean			CalcPreallocatedXPixel(const JRGB& color, unsigned long* xPixel) const;
	JBoolean			CalcPreallocatedXPixel(const JSize red, const JSize green,
											   const JSize blue, unsigned long* xPixel) const;

	JBoolean	GetColorIndex(const unsigned long xPixel,
							  JColorIndex* colorIndex) const;

	// called by various JX objects

	unsigned long	GetXPixel(const JColorIndex colorIndex) const;
	virtual void	GetRGB(const JColorIndex colorIndex, JSize* red,
						   JSize* green, JSize* blue) const;

	virtual int		GetSystemColorIndex(const JColorIndex colorIndex) const;

	// pre-allocated colors

	virtual JColorIndex	GetBlackColor() const;
	virtual JColorIndex	GetRedColor() const;
	virtual JColorIndex	GetGreenColor() const;
	virtual JColorIndex	GetYellowColor() const;
	virtual JColorIndex	GetBlueColor() const;
	virtual JColorIndex	GetMagentaColor() const;
	virtual JColorIndex	GetCyanColor() const;
	virtual JColorIndex	GetWhiteColor() const;

	virtual JColorIndex	GetGray10Color() const;
	virtual JColorIndex	GetGray20Color() const;
	virtual JColorIndex	GetGray30Color() const;
	virtual JColorIndex	GetGray40Color() const;
	virtual JColorIndex	GetGray50Color() const;
	virtual JColorIndex	GetGray60Color() const;
	virtual JColorIndex	GetGray70Color() const;
	virtual JColorIndex	GetGray80Color() const;
	virtual JColorIndex	GetGray90Color() const;

	virtual JColorIndex	GetGray25Color() const;
	virtual JColorIndex	GetGray75Color() const;

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

	struct ColorInfo
	{
		JRGB			color;
		unsigned long	xPixel;
		JBoolean		exactMatch;
		JBoolean		dynamic;
		JSize			useCount;
		JBoolean		preemptive;

		ColorInfo()
			:
			color(), xPixel(None), exactMatch(kJFalse),
			dynamic(kJFalse), useCount(1), preemptive(kJFalse)
		{ };

		ColorInfo(const JSize r, const JSize g, const JSize b,
				  const JColorIndex pixel, const JBoolean exact,
				  const JBoolean dyn, const JBoolean pre)
			:
			color(r,g,b), xPixel(pixel), exactMatch(exact),
			dynamic(dyn), useCount(1), preemptive(pre)
		{ };

		ColorInfo(const JRGB& c, const JColorIndex pixel,
				  const JBoolean exact, const JBoolean dyn,
				  const JBoolean pre)
			:
			color(c), xPixel(pixel), exactMatch(exact),
			dynamic(dyn), useCount(1), preemptive(pre)
		{ };
	};

private:

	JXDisplay*	itsDisplay;						// not owned
	Colormap	itsXColormap;
	Visual*		itsVisual;
	XVisualInfo	itsXVisualInfo;
	JBoolean	itsAllowApproxColorsFlag;		// kJTrue => approximate colors when colormap is full
	JBoolean	itsPreApproxColorsFlag;			// kJTrue => preemptively approximate colors
	JBoolean	itsAllColorsPreallocatedFlag;
	JBoolean	itsOwnsColormapFlag;
	JBoolean	itsCanSwitchCmapFlag;
	JBoolean	itsOrigCanSwitchCmapFlag;
	JBoolean	itsNotifyNewCmapFlag;
	JBoolean	itsSwitchingCmapFlag;

	JArray<ColorInfo>*	itsColorList;			// NULL if all colors are preallocated

	JXColorDistanceFn	itsColorDistanceFn;
	JSize				itsColorBoxHW;
	JSize				itsPreColorBoxHW;

	// for calculating preallocated X pixel values

	unsigned long	itsMask[3];
	short			itsStartIndex[3];
	short			itsEndIndex[3];

	// optimization for color approximation

	JSize		itsXColorListUseCount;			// # of unbalanced calls to PrepareForMassColorAllocation()
	JBoolean	itsXColorListInitFlag;			// kJTrue => itsXColorList contains colormap
	XColor*		itsXColorList;					// NULL unless approximating many colors

private:

	JXColormap(JXDisplay* display, Visual* visual, Colormap xColormap,
			   const JBoolean ownsColormap);

	void	InitPreallocInfo(const unsigned long redMask,
							 const unsigned long greenMask, const unsigned long blueMask);

	void		AllocateDefaultColors();
	JBoolean	CreateEmptyColormap();
	JColorIndex	StoreNewColor(const ColorInfo& newInfo);

	void		AllocateXColorList();
	void		FreeXColorList();
	JBoolean	AllocateApproxStaticColor(const JSize red, const JSize green,
										  const JSize blue,
										  JColorIndex* colorIndex,
										  JBoolean* exactMatch);
	JBoolean	PrivateAllocateStaticColor(const JSize red, const JSize green,
										   const JSize blue, JColorIndex* colorIndex,
										   JBoolean* exactMatch, const JBoolean* forceMatch = NULL);

	JBoolean	IsInsideColorBox(const long r1, const long g1, const long b1,
								 const long r2, const long g2, const long b2,
								 const long hw) const;

	// called by JXDisplay

	static JXColormap*	Create(JXDisplay* display);
	void				AllowSwitchColormap(const JBoolean allow);

	// not allowed

	JXColormap(const JXColormap& source);
	const JXColormap& operator=(const JXColormap& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNewColormap;

	class NewColormap : public JBroadcaster::Message
		{
		public:

			NewColormap(const unsigned long pixelTable[])
				:
				JBroadcaster::Message(kNewColormap),
				itsPixelTable(pixelTable)
			{ };

			unsigned long
			ConvertPixel(const unsigned long origPixel)
				const
			{
				return itsPixelTable [ origPixel ];
			};

		private:

			const unsigned long* itsPixelTable;
		};
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

/******************************************************************************
 GetMaxColorCount

 ******************************************************************************/

inline JSize
JXColormap::GetMaxColorCount()
	const
{
	return itsXVisualInfo.colormap_size;
}

/******************************************************************************
 CalcPreallocatedXPixel

	Returns kJTrue if it was able to calculate the xPixel value of the given
	color without contacting the server.

 ******************************************************************************/

inline JBoolean
JXColormap::CalcPreallocatedXPixel
	(
	const JRGB&		color,
	unsigned long*	xPixel
	)
	const
{
	return CalcPreallocatedXPixel(color.red, color.green, color.blue, xPixel);
}

/******************************************************************************
 AllowSwitchColormap (private)

 ******************************************************************************/

inline void
JXColormap::AllowSwitchColormap
	(
	const JBoolean allow
	)
{
	itsCanSwitchCmapFlag = JConvertToBoolean( itsOrigCanSwitchCmapFlag && allow );
}

/******************************************************************************
 Color approximation

	This is turned on by default.

 ******************************************************************************/

inline JBoolean
JXColormap::WillApproximateColors()
	const
{
	return itsAllowApproxColorsFlag;
}

inline void
JXColormap::ShouldApproximateColors
	(
	const JBoolean approx
	)
{
	itsAllowApproxColorsFlag = approx;
}

/******************************************************************************
 Preemptive color approximation

	If this is turned on, the colormap will fill up more slowly because
	existing colors will be used when possible.

 ******************************************************************************/

inline JBoolean
JXColormap::WillPreemptivelyApproximateColors()
	const
{
	return itsPreApproxColorsFlag;
}

inline void
JXColormap::ShouldPreemptivelyApproximateColors
	(
	const JBoolean preempt
	)
{
	itsPreApproxColorsFlag = preempt;
}

/******************************************************************************
 Color distance function

	This determines which existing color is closest to the requested one.

 ******************************************************************************/

inline JXColorDistanceFn
JXColormap::GetColorDistanceFunction()
	const
{
	return itsColorDistanceFn;
}

inline void
JXColormap::SetColorDistanceFunction
	(
	JXColorDistanceFn f
	)
{
	itsColorDistanceFn = f;
}

/******************************************************************************
 Color box

	This determines which existing colors can be considered when looking
	for an approximation.

 ******************************************************************************/

inline JSize
JXColormap::GetColorBoxHalfWidth()
	const
{
	return itsColorBoxHW;
}

inline void
JXColormap::SetColorBoxHalfWidth
	(
	const JSize halfWidth
	)
{
	itsColorBoxHW = halfWidth;
}

/******************************************************************************
 Preemptive color box

	This determines which existing colors can be considered when looking
	for a preemptive approximation.

 ******************************************************************************/

inline JSize
JXColormap::GetPreemptiveColorBoxHalfWidth()
	const
{
	return itsPreColorBoxHW;
}

inline void
JXColormap::SetPreemptiveColorBoxHalfWidth
	(
	const JSize halfWidth
	)
{
	itsPreColorBoxHW = halfWidth;
}

/******************************************************************************
 IsInsideColorBox (private)

 ******************************************************************************/

inline JBoolean
JXColormap::IsInsideColorBox
	(
	const long r1,
	const long g1,
	const long b1,
	const long r2,
	const long g2,
	const long b2,
	const long hw
	)
	const
{
	return JI2B(r1 - hw <= r2 && r2 <= r1 + hw &&
				g1 - hw <= g2 && g2 <= g1 + hw &&
				b1 - hw <= b2 && b2 <= b1 + hw);
}

#endif
