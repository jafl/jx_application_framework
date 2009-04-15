/******************************************************************************
 JXWindowPainter.h

	Interface for the JXWindowPainter class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWindowPainter
#define _H_JXWindowPainter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPainter.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;
class JXGC;
class JXColormap;
class JXImage;

class JXWindowPainter : public JPainter
{
public:

	JXWindowPainter(JXGC* gc, Drawable drawable, 
					const JRect& defaultClipRect,
					const Region defaultClipRegion);

	virtual ~JXWindowPainter();

	JXDisplay*	GetDisplay() const;
	JXColormap*	GetXColormap() const;
	JXGC*		GetGC() const;

	virtual void	Reset();
	void			Reset(const JRect& defClipRect, const Region clipRegion);

	Region	GetClipRegion() const;
	void	SetClipRegion(const Region region);
	void	ResetClipRegion();

	virtual JRect	SetClipRect(const JRect& r);
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

	virtual void	Point(const JCoordinate x, const JCoordinate y);

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2);

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle);

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly);

	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect);

	// this is virtual so JXImagePainter can override it correctly

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JCharacter* str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

	// use with extreme caution

	JBoolean	GetDefaultClipRegion(Region* region) const;
	void		SetDefaultClipRegion(const Region region);

protected:

	Drawable	GetDrawable() const;

	void	SetDrawable(Drawable d);

private:

	JXDisplay*	itsDisplay;			// we don't own this
	Drawable	itsDrawable;		// we don't own this
	JXGC*		itsGC;				// we don't own this
	Region		itsDefClipRegion;	// can be NULL
	Region		itsClipRegion;		// can be NULL
	JXGC*		itsRotTextGC;
	JBoolean	itsResetShouldClearClipRegionFlag;

private:

	void	StyleString(const JCharacter* str,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate ascent, const JCoordinate descent,
						const JColorIndex color);

	void	CalcClipRegion(const Region region);

	// not allowed

	JXWindowPainter(const JXWindowPainter& source);
	const JXWindowPainter& operator=(const JXWindowPainter& source);
};


/******************************************************************************
 Clipping

 ******************************************************************************/

inline Region
JXWindowPainter::GetClipRegion()
	const
{
	return itsClipRegion;
}

inline JBoolean
JXWindowPainter::GetDefaultClipRegion
	(
	Region* region
	)
	const
{
	*region = itsDefClipRegion;
	return JI2B( itsDefClipRegion != NULL );
}

/******************************************************************************
 Get info

 ******************************************************************************/

inline JXDisplay*
JXWindowPainter::GetDisplay()
	const
{
	return itsDisplay;
}

inline Drawable
JXWindowPainter::GetDrawable()	// protected
	const
{
	return itsDrawable;
}

inline JXGC*
JXWindowPainter::GetGC()
	const
{
	return itsGC;
}

/******************************************************************************
 SetDrawable (protected)

 ******************************************************************************/

inline void
JXWindowPainter::SetDrawable
	(
	Drawable d
	)
{
	itsDrawable = d;
}

#endif
