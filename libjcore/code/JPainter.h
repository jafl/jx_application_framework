/******************************************************************************
 JPainter.h

	Interface for the JPainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPainter
#define _H_JPainter

#include "jx-af/jcore/JRect.h"
#include "jx-af/jcore/JPolygon.h"
#include "jx-af/jcore/JFont.h"

class JString;
class JFontManager;
class JImage;

class JPainter
{
public:

	enum HAlignment
	{
		kHAlignLeft,
		kHAlignCenter,
		kHAlignRight
	};

	enum VAlignment
	{
		kVAlignTop,
		kVAlignCenter,
		kVAlignBottom
	};

public:

	JPainter(JFontManager* fontManager, const JRect& defaultClipRect);

	virtual ~JPainter();

	JFontManager*	GetFontManager() const;

	virtual void	Reset();
	void			Reset(const JRect& defClipRect);	// calls Reset()
	virtual void	ResetAllButClipping();

	// drawing area

	const JRect&	GetClipRect() const;
	virtual JRect	SetClipRect(const JRect& r);		// must call inherited
	void			ResetClipRect();

	const JRect&	GetDefaultClipRect() const;
	void			SetDefaultClipRect(const JRect& r);

	// pen

	const JPoint&	GetOrigin() const;
	void			SetOrigin(const JCoordinate x, const JCoordinate y);
	void			SetOrigin(const JPoint& pt);
	void			ShiftOrigin(const JCoordinate dx, const JCoordinate dy);
	void			ShiftOrigin(const JPoint& delta);

	const JPoint&	GetPenLocation() const;
	void			SetPenLocation(const JCoordinate x, const JCoordinate y);
	void			SetPenLocation(const JPoint& pt);
	void			ShiftPenLocation(const JCoordinate dx, const JCoordinate dy);
	void			ShiftPenLocation(const JPoint& delta);

	JColorID		GetPenColor() const;
	void			SetPenColor(const JColorID color);

	JSize			GetLineWidth() const;
	void			SetLineWidth(const JSize width);

	bool			LinesAreDashed() const;
	void			DrawDashedLines(const bool on);
	bool			GetDashList(const JArray<JSize>** dashList, JSize* dashOffset) const;
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);
						// must call inherited

	bool			IsFilling() const;
	void			SetFilling(const bool on);

	// text

	const JFont&	GetFont() const;

	void	SetFontName(const JString& name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& f);

	JSize	GetLineHeight(JCoordinate* ascent, JCoordinate* descent) const;
	JSize	GetLineHeight() const;

	JSize	GetStringWidth(const JString& str) const;

	void			String(const JCoordinate left, const JCoordinate top,
						   const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	void			String(const JPoint& topLeft, const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	void			String(const JRect& rect, const JString& str,
						   const HAlignment hAlign = kHAlignLeft,
						   const VAlignment vAlign = kVAlignTop);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) = 0;
	void			String(const JFloat angle, const JPoint& topLeft,
						   const JString& str, const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	void			String(const JFloat angle, const JRect& rect,
						   const JString& str,
						   const HAlignment hAlign = kHAlignLeft,
						   const VAlignment vAlign = kVAlignTop);

	virtual void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
						const JString& str) = 0;

	// drawing

	virtual void	Point(const JCoordinate x, const JCoordinate y) = 0;
	void			Point(const JPoint& pt);

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2) = 0;
	void			Line(const JPoint& pt1, const JPoint& pt2);

	void			LineTo(const JCoordinate x, const JCoordinate y);
	void			LineTo(const JPoint& pt);

	void			DeltaLine(const JCoordinate dx, const JCoordinate dy);
	void			DeltaLine(const JPoint& delta);

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h) = 0;
	void			Rect(const JPoint& upperLeft,
						 const JCoordinate w, const JCoordinate h);
	void			Rect(const JRect& bounds);
	void			RectInside(const JRect& bounds);

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h) = 0;
	void			Ellipse(const JPoint& upperLeft,
							const JCoordinate w, const JCoordinate h);
	void			Ellipse(const JRect& bounds);

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle) = 0;
	void			Arc(const JPoint& upperLeft,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle);
	void			Arc(const JRect& bounds,
						const JFloat startAngle, const JFloat deltaAngle);

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly) = 0;
	void			Polygon(const JPoint& upperLeft, const JPolygon& poly);
	void			Polygon(const JPolygon& poly);

	void			Image(const JImage& image, const JRect& srcRect,
						  const JCoordinate left, const JCoordinate top);
	void			Image(const JImage& image, const JRect& srcRect, const JPoint& topLeft);
	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect) = 0;

protected:

	JSize	AlignString(JCoordinate* left, JCoordinate* top, const JString& str,
						const JCoordinate width, const HAlignment hAlign,
						const JCoordinate height, const VAlignment vAlign) const;

private:

	JPoint		itsOrigin;
	JPoint		itsPenLoc;
	JSize		itsLineWidth;
	JColorID	itsPenColor;
	bool		itsFillFlag;
	JRect		itsClipRect;

	bool			itsDrawDashedLinesFlag;
	JSize			itsDashOffset;
	JArray<JSize>*	itsDashList;			// can be nullptr

	JFontManager*	itsFontManager;
	JFont			itsFont;

	JRect			itsDefClipRect;		// global coords, so origin can change
	const JColorID	itsDefaultColor;

private:

	// not allowed

	JPainter(const JPainter&) = delete;
	JPainter& operator=(const JPainter&) = delete;
};


/******************************************************************************
 Clipping

 ******************************************************************************/

inline const JRect&
JPainter::GetClipRect()
	const
{
	return itsClipRect;
}

inline const JRect&
JPainter::GetDefaultClipRect()
	const
{
	return itsDefClipRect;
}

/******************************************************************************
 Origin

 ******************************************************************************/

inline const JPoint&
JPainter::GetOrigin()
	const
{
	return itsOrigin;
}

inline void
JPainter::SetOrigin
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	itsOrigin = JPoint(x,y);
}

inline void
JPainter::SetOrigin
	(
	const JPoint& pt
	)
{
	itsOrigin = pt;
}

inline void
JPainter::ShiftOrigin
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	itsOrigin.x += dx;
	itsOrigin.y += dy;
}

inline void
JPainter::ShiftOrigin
	(
	const JPoint& delta
	)
{
	itsOrigin += delta;
}

/******************************************************************************
 Pen location

 ******************************************************************************/

inline const JPoint&
JPainter::GetPenLocation()
	const
{
	return itsPenLoc;
}

inline void
JPainter::SetPenLocation
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	itsPenLoc = JPoint(x,y);
}

inline void
JPainter::SetPenLocation
	(
	const JPoint& pt
	)
{
	itsPenLoc = pt;
}

inline void
JPainter::ShiftPenLocation
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	itsPenLoc.x += dx;
	itsPenLoc.y += dy;
}

inline void
JPainter::ShiftPenLocation
	(
	const JPoint& delta
	)
{
	itsPenLoc += delta;
}

/******************************************************************************
 Pen color

 ******************************************************************************/

inline JColorID
JPainter::GetPenColor()
	const
{
	return itsPenColor;
}

inline void
JPainter::SetPenColor
	(
	const JColorID color
	)
{
	itsPenColor = color;
}

/******************************************************************************
 GetLineWidth

 ******************************************************************************/

inline JSize
JPainter::GetLineWidth()
	const
{
	return itsLineWidth;
}

/******************************************************************************
 Dashed lines

 ******************************************************************************/

inline bool
JPainter::LinesAreDashed()
	const
{
	return itsDrawDashedLinesFlag && itsDashList != nullptr;
}

inline void
JPainter::DrawDashedLines
	(
	const bool on
	)
{
	itsDrawDashedLinesFlag = on;
}

inline bool
JPainter::GetDashList
	(
	const JArray<JSize>**	dashList,
	JSize*					dashOffset
	)
	const
{
	*dashList   = itsDashList;
	*dashOffset = itsDashOffset;
	return *dashList != nullptr;
}

/******************************************************************************
 Fill

 ******************************************************************************/

inline bool
JPainter::IsFilling()
	const
{
	return itsFillFlag;
}

inline void
JPainter::SetFilling
	(
	const bool on
	)
{
	itsFillFlag = on;
}

/******************************************************************************
 GetFont

 ******************************************************************************/

inline const JFont&
JPainter::GetFont()
	const
{
	return itsFont;
}

/******************************************************************************
 Set font info

 ******************************************************************************/

inline void
JPainter::SetFontName
	(
	const JString& name
	)
{
	itsFont.SetName(name);
}

inline void
JPainter::SetFontSize
	(
	const JSize size
	)
{
	itsFont.SetSize(size);
}

inline void
JPainter::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFont.SetStyle(style);
}

inline void
JPainter::SetFont
	(
	const JFont& font
	)
{
	itsFont = font;
}

/******************************************************************************
 String

 ******************************************************************************/

inline void
JPainter::String
	(
	const JPoint&		topLeft,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	String(topLeft.x, topLeft.y, str, width, hAlign, height, vAlign);
}

inline void
JPainter::String
	(
	const JRect&		rect,
	const JString&		str,
	const HAlignment	hAlign,
	const VAlignment	vAlign
	)
{
	String(rect.left, rect.top, str, rect.width(), hAlign, rect.height(), vAlign);
}

inline void
JPainter::String
	(
	const JFloat		angle,
	const JPoint&		topLeft,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	String(angle, topLeft.x, topLeft.y, str, width, hAlign, height, vAlign);
}

/******************************************************************************
 Point

 ******************************************************************************/

inline void
JPainter::Point
	(
	const JPoint& pt
	)
{
	Point(pt.x, pt.y);
}

/******************************************************************************
 Line

 ******************************************************************************/

inline void
JPainter::Line
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
{
	Line(pt1.x, pt1.y, pt2.x, pt2.y);
}

/******************************************************************************
 LineTo

 ******************************************************************************/

inline void
JPainter::LineTo
	(
	const JPoint& pt
	)
{
	Line(itsPenLoc.x,itsPenLoc.y, pt.x,pt.y);
}

inline void
JPainter::LineTo
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	Line(itsPenLoc.x,itsPenLoc.y, x,y);
}

/******************************************************************************
 DeltaLine

 ******************************************************************************/

inline void
JPainter::DeltaLine
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	LineTo(itsPenLoc.x + dx, itsPenLoc.y + dy);
}

inline void
JPainter::DeltaLine
	(
	const JPoint& delta
	)
{
	LineTo(itsPenLoc.x + delta.x, itsPenLoc.y + delta.y);
}

/******************************************************************************
 Rect

 ******************************************************************************/

inline void
JPainter::Rect
	(
	const JPoint&		upperLeft,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	Rect(upperLeft.x, upperLeft.y, w, h);
}

inline void
JPainter::Rect
	(
	const JRect& bounds
	)
{
	Rect(bounds.left, bounds.top, bounds.width(), bounds.height());
}

/******************************************************************************
 Ellipse

 ******************************************************************************/

inline void
JPainter::Ellipse
	(
	const JPoint&		upperLeft,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	Ellipse(upperLeft.x, upperLeft.y, w, h);
}

inline void
JPainter::Ellipse
	(
	const JRect& bounds
	)
{
	Ellipse(bounds.left, bounds.top, bounds.width(), bounds.height());
}

/******************************************************************************
 Arc

 ******************************************************************************/

inline void
JPainter::Arc
	(
	const JPoint&		upperLeft,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
{
	Arc(upperLeft.x, upperLeft.y, w, h, startAngle, deltaAngle);
}

inline void
JPainter::Arc
	(
	const JRect&	bounds,
	const JFloat	startAngle,
	const JFloat	deltaAngle
	)
{
	Arc(bounds.left, bounds.top, bounds.width(), bounds.height(),
		startAngle, deltaAngle);
}

/******************************************************************************
 Polygon

 ******************************************************************************/

inline void
JPainter::Polygon
	(
	const JPoint&	upperLeft,
	const JPolygon&	poly
	)
{
	Polygon(upperLeft.x, upperLeft.y, poly);
}

inline void
JPainter::Polygon
	(
	const JPolygon& poly
	)
{
	Polygon(0,0, poly);
}

/******************************************************************************
 Image

 ******************************************************************************/

inline void
JPainter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JPoint&	topLeft
	)
{
	JRect destRect(topLeft.y, topLeft.x, topLeft.y + srcRect.height(),
				   topLeft.x + srcRect.width());
	Image(image, srcRect, destRect);
}

inline void
JPainter::Image
	(
	const JImage&		image,
	const JRect&		srcRect,
	const JCoordinate	left,
	const JCoordinate	top
	)
{
	JRect destRect(top, left, top + srcRect.height(),
				   left + srcRect.width());
	Image(image, srcRect, destRect);
}

/******************************************************************************
 GetFontManager

 ******************************************************************************/

inline JFontManager*
JPainter::GetFontManager()
	const
{
	return itsFontManager;
}

#endif
