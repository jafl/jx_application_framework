/******************************************************************************
 pixmap.cpp

	Pixmap functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <assert.h>

/******************************************************************************
 XCreatePixmap

	TODO
		use CreateCompatibleBitmap for color pixmaps?

 ******************************************************************************/

Pixmap
XCreatePixmap
	(
	Display*		display,
    Drawable		drawable,
    unsigned int	width,
    unsigned int	height,
    unsigned int	depth
	)
{
	HBITMAP bitmap = CreateBitmap(width, height, depth, depth, NULL);
	return xAllocateID(display, kXPixmapType, bitmap);
}

/******************************************************************************
 XFreePixmap

 ******************************************************************************/

int
XFreePixmap
	(
	Display*	display,
	Pixmap		pixmap
	)
{
	xFreeID(display, pixmap);
	return TRUE;
}
