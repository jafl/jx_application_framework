/******************************************************************************
 JXImagePainter.h

	Interface for the JXImagePainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImagePainter
#define _H_JXImagePainter

#include "JXWindowPainter.h"
#include <JBroadcaster.h>

class JXImagePainter : public JXWindowPainter, virtual public JBroadcaster
{
public:

	JXImagePainter(JXImage* image, Drawable drawable, 
				   const JRect& defaultClipRect,
				   const Region defaultClipRegion);

	virtual ~JXImagePainter();

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;

	virtual void	Point(const JCoordinate x, const JCoordinate y) override;

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2) override;

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h) override;

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h) override;

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle) override;

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly) override;

	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect) override;

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JString& str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;

private:

	JXImage*	itsImage;	// we don't own this, can be nullptr

private:

	JBoolean	PrepareXDraw();

	// not allowed

	JXImagePainter(const JXImagePainter& source);
	const JXImagePainter& operator=(const JXImagePainter& source);
};

#endif
