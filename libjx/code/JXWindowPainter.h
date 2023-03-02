/******************************************************************************
 JXWindowPainter.h

	Interface for the JXWindowPainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindowPainter
#define _H_JXWindowPainter

#include <jx-af/jcore/JPainter.h>
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

	~JXWindowPainter() override;

	JXDisplay*		GetDisplay() const;
	JXColorManager*	GetXColorManager() const;
	JXGC*			GetGC() const;

	void	Reset() override;
	void	Reset(const JRect& defClipRect, const Region clipRegion);

	Region	GetClipRegion() const;
	void	SetClipRegion(const Region region);
	void	ResetClipRegion();

	JRect	SetClipRect(const JRect& r) override;
	void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0) override;

	void	StringNoSubstitutions(const JCoordinate left, const JCoordinate top,
								  const JString& str) override;
	void	String(const JFloat angle, const JCoordinate left,
				   const JCoordinate top, const JString& str,
				   const JCoordinate width = 0,
				   const HAlign hAlign = HAlign::kLeft,
				   const JCoordinate height = 0,
				   const VAlign vAlign = VAlign::kTop) override;

	void	Point(const JCoordinate x, const JCoordinate y) override;

	void	Line(const JCoordinate x1, const JCoordinate y1,
				 const JCoordinate x2, const JCoordinate y2) override;

	void	Rect(const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h) override;

	void	Ellipse(const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h) override;

	void	Arc(const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h,
				const JFloat startAngle, const JFloat deltaAngle) override;

	void	Polygon(const JCoordinate left, const JCoordinate top,
					const JPolygon& poly) override;

	void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect) override;

	// this is virtual so JXImagePainter can override it correctly

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JString& str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlign hAlign = HAlign::kLeft,
						   const JCoordinate height = 0,
						   const VAlign vAlign = VAlign::kTop);

	// use with extreme caution

	Drawable	GetDrawable() const;
	XftDraw*	GetFontDrawable() const;

	bool	GetDefaultClipRegion(Region* region) const;
	void	SetDefaultClipRegion(const Region region);

protected:

	void	SetDrawable(Drawable d);

private:

	JXDisplay*	itsDisplay;			// we don't own this
	Drawable	itsDrawable;		// we don't own this
	JXGC*		itsGC;				// we don't own this
	Region		itsDefClipRegion;	// can be nullptr, global coords to avoid overflow
	Region		itsClipRegion;		// can be nullptr, global coords to avoid overflow
	XftDraw*	itsFontDrawable;	// nullptr until first needed
	Region		itsFontClipRegion;
	bool		itsResetShouldClearClipRegionFlag;

private:

	void	StyleString(const JString& str,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate ascent, const JCoordinate descent);

	void	CalcClipRegion(const Region region);
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

inline bool
JXWindowPainter::GetDefaultClipRegion
	(
	Region* region
	)
	const
{
	*region = itsDefClipRegion;
	return itsDefClipRegion != nullptr;
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
