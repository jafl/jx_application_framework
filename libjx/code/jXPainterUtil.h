/******************************************************************************
 jXPainterUtil.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXPainterUtil
#define _H_jXPainterUtil

#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontStyle.h>		// for convenience
#include "jXConstants.h"

// frame

void JXDrawUpFrame(JPainter& p, const JRect& rect, const JCoordinate width,
				   const bool fill = false,
				   const JColorID fillColor = kJXTransparentColor);			// placeholder for GetBlackColor()
void JXDrawDownFrame(JPainter& p, const JRect& rect, const JCoordinate width,
					 const bool fill = false,
					 const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()

void JXDrawEngravedFrame(JPainter& p, const JRect& rect,
						 const JCoordinate downWidth, const JCoordinate betweenWidth,
						 const JCoordinate upWidth,
						 const bool fill = false,
						 const JColorID fillColor = kJXTransparentColor);	// placeholder for GetBlackColor()
void JXDrawEmbossedFrame(JPainter& p, const JRect& rect,
						 const JCoordinate upWidth, const JCoordinate betweenWidth,
						 const JCoordinate downWidth,
						 const bool fill = false,
						 const JColorID fillColor = kJXTransparentColor);	// placeholder for GetBlackColor()

// diamond

void JXDrawFlatDiamond(JPainter& p, const JRect& rect, const JCoordinate width,
					   const JColorID borderColor,
					   const bool fill = false,
					   const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()

void JXDrawUpDiamond(JPainter& p, const JRect& rect, const JCoordinate width,
					 const bool fill = false,
					 const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()
void JXDrawDownDiamond(JPainter& p, const JRect& rect, const JCoordinate width,
					   const bool fill = false,
					   const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()

void JXFillDiamond(JPainter& p, const JRect& rect, const JColorID fillColor);

// left arrow

void JXDrawUpArrowLeft(JPainter& p, const JRect& rect, const JCoordinate width,
					   const bool fill = false,
					   const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()
void JXDrawDownArrowLeft(JPainter& p, const JRect& rect, const JCoordinate width,
						 const bool fill = false,
						 const JColorID fillColor = kJXTransparentColor);	// placeholder for GetBlackColor()

void JXFillArrowLeft(JPainter& p, const JRect& rect, const JColorID fillColor);

// right arrow

void JXDrawUpArrowRight(JPainter& p, const JRect& rect, const JCoordinate width,
						const bool fill = false,
						const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()
void JXDrawDownArrowRight(JPainter& p, const JRect& rect, const JCoordinate width,
						  const bool fill = false,
						  const JColorID fillColor = kJXTransparentColor);	// placeholder for GetBlackColor()

void JXFillArrowRight(JPainter& p, const JRect& rect, const JColorID fillColor);

// up arrow

void JXDrawUpArrowUp(JPainter& p, const JRect& rect, const JCoordinate width,
					 const bool fill = false,
					 const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()
void JXDrawDownArrowUp(JPainter& p, const JRect& rect, const JCoordinate width,
					   const bool fill = false,
					   const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()

void JXFillArrowUp(JPainter& p, const JRect& rect, const JColorID fillColor);

// down arrow

void JXDrawUpArrowDown(JPainter& p, const JRect& rect, const JCoordinate width,
					   const bool fill = false,
					   const JColorID fillColor = kJXTransparentColor);		// placeholder for GetBlackColor()
void JXDrawDownArrowDown(JPainter& p, const JRect& rect, const JCoordinate width,
						 const bool fill = false,
						 const JColorID fillColor = kJXTransparentColor);	// placeholder for GetBlackColor()

void JXFillArrowDown(JPainter& p, const JRect& rect, const JColorID fillColor);

// Drag-and-Drop

void	JXDrawDNDBorder(JPainter& p, const JRect& rect, const JCoordinate width);

#endif
