/******************************************************************************
 JXImagePainter.h

	Interface for the JXImagePainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImagePainter
#define _H_JXImagePainter

#include "jx-af/jx/JXWindowPainter.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXImagePainter : public JXWindowPainter, virtual public JBroadcaster
{
public:

	JXImagePainter(JXImage* image, Drawable drawable, 
				   const JRect& defaultClipRect,
				   const Region defaultClipRegion);

	~JXImagePainter();

	void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
						const JString& str) override;
	void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;

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

	void	String(const JCoordinate left, const JCoordinate top,
						   const JString& str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;

private:

	JXImage*	itsImage;	// we don't own this, can be nullptr

private:

	bool	PrepareXDraw();
};

#endif
