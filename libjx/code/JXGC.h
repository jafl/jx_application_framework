/******************************************************************************
 JXGC.h

	Interface for the JXGC class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGC
#define _H_JXGC

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JRect.h>
#include <jColor.h>
#include <JFontStyle.h>
#include <JArray.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;
class JXColormap;
class JXImageMask;

class JXGC
{
public:

	JXGC(JXDisplay* display, JXColormap* colormap, const Drawable drawable);

	~JXGC();

	JXDisplay*	GetDisplay() const;
	JXColormap*	GetColormap() const;

	JBoolean	GetClipping(JPoint* offset, Region* region, JXImageMask** pixmap);
	void		SetClipRect(const JRect& clipRect);
	void		SetClipRegion(const Region clipRegion);
	void		SetClipPixmap(const JPoint& offset, Pixmap pixmap);
	void		SetClipPixmap(const JPoint& offset, const JXImageMask& mask);
	void		ClearClipping();

	void	SetDrawingColor(const JColorIndex color);
	void	SetDrawingFunction(const int function);
	void	SetLineWidth(const JSize width);
	void	DrawDashedLines(const JBoolean on);
	void	SetDashList(const JArray<JSize>& dashList, const JSize offset = 0);
	void	SetSubwindowMode(const int mode);

	void	DrawPoint(const Drawable drawable,
					  const JCoordinate x, const JCoordinate y) const;
	void	DrawLine(const Drawable drawable,
					 const JCoordinate x1, const JCoordinate y1,
					 const JCoordinate x2, const JCoordinate y2) const;
	void	DrawLines(const Drawable drawable,
					  const JSize ptCount, XPoint xpt[]) const;
	void	DrawRect(const Drawable drawable,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate width, const JCoordinate height) const;
	void	FillRect(const Drawable drawable,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate width, const JCoordinate height) const;
	void	DrawArc(const Drawable drawable,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate width, const JCoordinate height,
					const JFloat startAngle, const JFloat deltaAngle) const;
	void	FillArc(const Drawable drawable,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate width, const JCoordinate height,
					const JFloat startAngle, const JFloat deltaAngle) const;
	void	FillPolygon(const Drawable drawable,
						const JSize ptCount, XPoint xpt[]) const;

	void	SetFont(const JFontID id);
	void	DrawString(const Drawable drawable, const JCoordinate x,
					   const JCoordinate y, const JCharacter* str) const;

	void	CopyPixels(const Drawable source,
					   const JCoordinate src_x, const JCoordinate src_y,
					   const JCoordinate width, const JCoordinate height,
					   const Drawable dest,
					   const JCoordinate dest_x, const JCoordinate dest_y) const;
	void	CopyImage(const XImage* source,
					  const JCoordinate src_x, const JCoordinate src_y,
					  const JCoordinate width, const JCoordinate height,
					  const Drawable dest,
					  const JCoordinate dest_x, const JCoordinate dest_y) const;

private:

	JXDisplay*	itsDisplay;		// we don't own this
	JXColormap*	itsColormap;	// we don't own this
	GC			itsXGC;
	JSize		itsDepth;

	// clipping

	JPoint	itsClipOffset;
	Region	itsClipRegion;
	Pixmap	itsClipPixmap;

	// buffered values

	JBoolean	itsLastColorInit;
	JColorIndex itsLastColor;
	int			itsLastFunction;
	JSize		itsLastLineWidth;
	JBoolean	itsDashedLinesFlag;
	JFontID		itsLastFont;
	int			itsLastSubwindowMode;

private:

	void	ClearPrivateClipping();
	int		GetXLineStyle(const JBoolean drawDashedLines) const;

	// not allowed

	JXGC(const JXGC& source);
	const JXGC& operator=(const JXGC& source);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXGC::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JXColormap*
JXGC::GetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 GetXLineStyle (private)

 ******************************************************************************/

inline int
JXGC::GetXLineStyle
	(
	const JBoolean drawDashedLines
	)
	const
{
	return (drawDashedLines ? LineOnOffDash : LineSolid);
}

#endif
