/******************************************************************************
 cursor.cpp

	Cursor functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <assert.h>

/******************************************************************************
 XCreateFontCursor

	TODO
		call XCreatePixmapCursor for other standard X cursors

 ******************************************************************************/

Cursor
XCreateFontCursor
	(
	Display*		display,
	unsigned int	shape
	)
{
	LPCTSTR name;
	if (shape == XC_left_ptr)
		{
		name = IDC_ARROW;
		}
	else if (shape == XC_xterm)
		{
		name = IDC_IBEAM;
		}
	else if (shape == XC_tcross)
		{
		name = IDC_CROSS;
		}
	else if (shape == XC_watch)
		{
		name = IDC_WAIT;
		}
	else if (shape == XC_X_cursor)
		{
		name = IDC_NO;
		}
	else
		{
		name = IDC_SIZEALL;
		}

	HCURSOR cursor = LoadCursor(GetModuleHandle(NULL), name);
	return xAllocateID(display, kXStandardCursorType, cursor);
}

/******************************************************************************
 XCreatePixmapCursor

	TODO

	AND mask	XOR mask	Display
	0			0			Black
	0			1			White
	1			0			Screen
	1			1			Reverse screen

 ******************************************************************************/

Cursor
XCreatePixmapCursor
	(
	Display*		display,
	Pixmap			source,
	Pixmap			mask,
	XColor*			foreground_color,
	XColor*			background_color,
	unsigned int	x,
	unsigned int	y
	)
{
	HCURSOR cursor = LoadCursor(GetModuleHandle(NULL), IDC_SIZEALL);
	// = CreateCursor(GetModuleHandle(NULL), x, y, w, h, 
	return xAllocateID(display, kXCustomCursorType, cursor);
}

/******************************************************************************
 XFreeCursor

	TODO
		Save cursor for each window and process WM_SETCURSOR

 ******************************************************************************/

int
XFreeCursor
	(
	Display*	display,
	Cursor		cursor
	)
{
	xFreeID(display, cursor);
	return TRUE;
}

/******************************************************************************
 XDefineCursor

	TODO
		Save cursor for each window and process WM_SETCURSOR

 ******************************************************************************/

int
XDefineCursor
	(
	Display*	display,
	Window		window,
	Cursor		cursor
	)
{
	xItem item;
	if (xFindID(display, cursor, &item))
		{
		SetCursor(item.handle.cursor);
		}
	return TRUE;
}
