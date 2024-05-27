/******************************************************************************
 JXGC.h

	Interface for the JXGC class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXGC
#define _H_JXGC

#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jColor.h>
#include <jx-af/jcore/JFont.h>
#include <jx-af/jcore/JArray.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define _XFT_NO_COMPAT_
#include <X11/Xft/Xft.h>

class JXDisplay;
class JXImageMask;

class JXGC
{
public:

	JXGC(JXDisplay* display, const Drawable drawable);

	~JXGC();

	JXDisplay*	GetDisplay() const;

	bool	GetClipping(JPoint* offset, Region* region, JXImageMask** pixmap);
	void	SetClipRect(const JRect& clipRect);
	void	SetClipRegion(const Region clipRegion);
	void	SetClipPixmap(const JPoint& offset, Pixmap pixmap);
	void	SetClipPixmap(const JPoint& offset, const JXImageMask& mask);
	void	ClearClipping();

	void	SetDrawingColor(const JColorID color);
	void	SetDrawingFunction(const int function);
	void	SetLineWidth(const JSize width);
	void	DrawDashedLines(const bool on);
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
	void	DrawString(const Drawable drawable, XftDraw* fdrawable,
					   const JCoordinate x, const JCoordinate y,
					   const JString& str) const;

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
	GC			itsXGC;
	JSize		itsDepth;

	// clipping

	JPoint	itsClipOffset;
	Region	itsClipRegion;
	Pixmap	itsClipPixmap;

	// buffered values

	bool		itsLastColorInit;
	JColorID 	itsLastColor;
	int			itsLastFunction;
	JSize		itsLastLineWidth;
	bool		itsDashedLinesFlag;
	JFontID		itsLastFontID;
	int			itsLastSubwindowMode;

private:

	void	ClearPrivateClipping();
	int		GetXLineStyle(const bool drawDashedLines) const;

	// not allowed

	JXGC(const JXGC&) = delete;
	JXGC& operator=(const JXGC&) = delete;
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
 SetFont

 ******************************************************************************/

inline void
JXGC::SetFont
	(
	const JFontID id
	)
{
	itsLastFontID = id;
}

#endif
