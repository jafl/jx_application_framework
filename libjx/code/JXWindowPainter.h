/******************************************************************************
 JXWindowPainter.h

	Interface for the JXWindowPainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindowPainter
#define _H_JXWindowPainter

#include <JPainter.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

class JXDisplay;
class JXGC;
class JXColorManager;
class JXImage;

class JXWindowPainter : public JPainter
{
public:

	JXWindowPainter(JXGC* gc, Drawable drawable, 
					const JRect& defaultClipRect,
					const Region defaultClipRegion);

	virtual ~JXWindowPainter();

	JXDisplay*		GetDisplay() const;
	JXColorManager*	GetXColorManager() const;
	JXGC*			GetGC() const;

	virtual void	Reset();
	void			Reset(const JRect& defClipRect, const Region clipRegion);

	Region	GetClipRegion() const;
	void	SetClipRegion(const Region region);
	void	ResetClipRegion();

	virtual JRect	SetClipRect(const JRect& r);
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
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
						   const JString& str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

	// use with extreme caution

	Drawable	GetDrawable() const;
	XftDraw*	GetFontDrawable() const;

	JBoolean	GetDefaultClipRegion(Region* region) const;
	void		SetDefaultClipRegion(const Region region);

protected:

	void	SetDrawable(Drawable d);

private:

	JXDisplay*	itsDisplay;			// we don't own this
	Drawable	itsDrawable;		// we don't own this
	JXGC*		itsGC;				// we don't own this
	Region		itsDefClipRegion;	// can be NULL
	Region		itsClipRegion;		// can be NULL
	XftDraw*	itsFontDrawable;	// NULL until first needed
	Region		itsFontClipRegion;
	JBoolean	itsResetShouldClearClipRegionFlag;

private:

	void	StyleString(const JString& str,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate ascent, const JCoordinate descent,
						const JColorID color);

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
JXWindowPainter::GetDrawable()
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
