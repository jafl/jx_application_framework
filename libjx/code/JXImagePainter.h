/******************************************************************************
 JXImagePainter.h

	Interface for the JXImagePainter class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImagePainter
#define _H_JXImagePainter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowPainter.h>
#include <JBroadcaster.h>

class JXImagePainter : public JXWindowPainter, virtual public JBroadcaster
{
public:

	JXImagePainter(JXImage* image, Drawable drawable, 
				   const JRect& defaultClipRect,
				   const Region defaultClipRegion);

	virtual ~JXImagePainter();

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

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JCharacter* str, const JIndex uIndex,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

protected:

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXImage*	itsImage;	// we don't own this, can be NULL

private:

	JBoolean	PrepareText();
	JBoolean	PrepareDraw();
	JBoolean	PrepareImage(const JXImage& image);
	JBoolean	PrepareXDraw();

	// not allowed

	JXImagePainter(const JXImagePainter& source);
	const JXImagePainter& operator=(const JXImagePainter& source);
};

#endif
