/******************************************************************************
 private.h
 
	Internal data structures and functions.

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include <vector>

enum xHandleType
{
	kXWindowType,
	kXPixmapType,
	kXStandardCursorType,
	kXCustomCursorType,
	kXAtomType,
};

union xHandle
{
	HWND	window;
	HBITMAP	bitmap;
	HCURSOR	cursor;
	HRGN	region;
	char*	atomName;
};

struct xWindowProperty
{
	Atom	property;
	Atom	type;
	int		format;
	int		length;
	char*	data;

	void
	Free()
	{
		free(data);
		data   = nullptr;
		length = 0;
		type   = None;
	};
};

union xData
{
	std::vector<xWindowProperty>*	propList;
};

struct xItem
{
	XID			id;
	xHandleType	type;
	xHandle		handle;
	xData		data;
};

Display*	xOpenDisplay(const char* name);
void		xCloseDisplay(Display* display);
void		xKillDisplay(Display* display);
void		xCloseAllTemporary();

XID			xAllocateID(Display* display, const xHandleType type, void* handle);
void		xFreeID(Display* display, const XID id);
bool		xFindID(const XID id, Display** display, xItem* item = nullptr, const BOOL remove = FALSE);
bool		xFindID(Display* display, const XID id, xItem* item, const BOOL remove = FALSE);
bool		xFindAtom(Display* display, const char* name, xItem* item, const BOOL remove = FALSE);

#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))
