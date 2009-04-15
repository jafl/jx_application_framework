/******************************************************************************
 private.cpp

	Internal data structures and functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <algorithm>
#include <assert.h>

using namespace std;

struct _XDisplay
{
	string*			name;
	vector<xItem>*	itemList;
	int				closeMode;
	bool			closed;
};

static vector<_XDisplay*> theDisplayList;

// Prototypes

static XID	xGetUniqueID(Display* display);

/******************************************************************************
 xOpenDisplay

 ******************************************************************************/

Display*
xOpenDisplay
	(
	const char* name
	)
{
	Display* display = new Display;
	assert( display != NULL );

	display->name = new string(name);
	assert( display->name != NULL );

	display->itemList = new vector<xItem>;
	assert( display->itemList != NULL );

	display->closeMode = DestroyAll;
	display->closed    = false;

	theDisplayList.insert(theDisplayList.end(), display);

	return display;
}

/******************************************************************************
 xCloseDisplay

 ******************************************************************************/

static void
xCloseHandle
	(
	const xItem& item
	)
{
	if (item.type == kXWindowType)
		{
		DestroyWindow(item.handle.window);
		}
	else if (item.type == kXPixmapType)
		{
		DeleteObject(item.handle.bitmap);
		}
	else if (item.type == kXCustomCursorType)
		{
		DestroyCursor(item.handle.cursor);
		}
	else if (item.type == kXAtomType)
		{
		free(item.handle.atomName);
		}
	else
		{
		assert( 0 );
		}
}

void
xCloseDisplay
	(
	Display* display
	)
{
	if (display->closeMode == DestroyAll)
		{
		vector<_XDisplay*>::iterator iter =
			remove(theDisplayList.begin(), theDisplayList.end(), display);
		theDisplayList.erase(iter, theDisplayList.end());

		for_each(display->itemList->begin(), display->itemList->end(), xCloseHandle);

		delete display->name;
		delete display->itemList;
		delete display;
		}
	else
		{
		display->closed = true;
		}
}

/******************************************************************************
 xKillDisplay

 ******************************************************************************/

void
xKillDisplay
	(
	Display* display
	)
{
	display->closeMode = DestroyAll;
	xCloseDisplay(display);
}

/******************************************************************************
 xCloseAllTemporary

 ******************************************************************************/

void
xCloseAllTemporary()
{
	vector<_XDisplay*>::iterator iter;
	for (iter = theDisplayList.begin(); iter < theDisplayList.end(); )
		{
		Display* display = *iter;
		if (display->closed && display->closeMode == RetainTemporary)
			{
			xKillDisplay(display);
			iter = theDisplayList.begin();
			}
		else
			{
			iter++;
			}
		}
}

/******************************************************************************
 xAllocateID

 ******************************************************************************/

XID
xAllocateID
	(
	Display*			display,
	const xHandleType	type,
	void*				handle
	)
{
	if (handle == NULL || handle == INVALID_HANDLE_VALUE)
		{
		return None;
		}

	xItem item;
	item.id   = xGetUniqueID(display);
	item.type = type;

	if (type == kXWindowType)
		{
		item.handle.window = (HWND) handle;
		}
	else if (type == kXPixmapType)
		{
		item.handle.bitmap = (HBITMAP) handle;
		}
	else if (type == kXStandardCursorType ||
			 type == kXCustomCursorType)
		{
		item.handle.cursor = (HCURSOR) handle;
		}
	else if (type == kXAtomType)
		{
		item.handle.atomName = strdup((char*) handle);
		}
	else
		{
		assert( 0 );
		}

	display->itemList->insert(display->itemList->end(), item);
	return item.id;
}

/******************************************************************************
 xFreeID

 ******************************************************************************/

void
xFreeID
	(
	Display*	display,
	const XID	id
	)
{
	xItem item;
	if (xFindID(display, id, &item, TRUE))
		{
		xCloseHandle(item);
		}
}

/******************************************************************************
 xGetUniqueID

	TODO
		share id counter with all other processes via CreateFileMapping()
		and MapViewOfFile()

		After wrapping the counter, need to search for unused id's

 ******************************************************************************/

static const XID theFirstID = XA_LAST_PREDEFINED + 1;
static XID theNextID        = theFirstID;

XID
xGetUniqueID
	(
	Display* display
	)
{
	theNextID++;
	if (theNextID == None)
		{
		assert( 0 );
		theNextID = theFirstID;
		}
	return theNextID;
}

/******************************************************************************
 xFindID

 ******************************************************************************/

bool
xFindID
	(
	const XID	id,
	Display**	display,
	xItem*		item,
	const BOOL	remove
	)
{
	xItem item1;
	vector<_XDisplay*>::iterator iter1;
	for (iter1 = theDisplayList.begin(); iter1 < theDisplayList.end(); iter1++)
		{
		if (xFindID(*iter1, id, &item1, remove))
			{
			*display = *iter1;
			if (item != NULL)
				{
				*item = item1;
				}
			return true;
			}
		}

	*display = NULL;
	return false;
}

/******************************************************************************
 xFindID

 ******************************************************************************/

bool
xFindID
	(
	Display*	display,
	const XID	id,
	xItem*		item,
	const BOOL	remove
	)
{
	vector<xItem>::iterator iter;
	for (iter = display->itemList->begin(); iter < display->itemList->end(); iter++)
		{
		if (iter->id == id)
			{
			*item = *iter;
			if (remove)
				{
				display->itemList->erase(iter);
				}
			return true;
			}
		}

	return false;
}

/******************************************************************************
 xFindAtom

 ******************************************************************************/

bool
xFindAtom
	(
	Display*	display,
	const char*	name,
	xItem*		item,
	const BOOL	remove
	)
{
	vector<xItem>::iterator iter;
	for (iter = display->itemList->begin(); iter < display->itemList->end(); iter++)
		{
		if (iter->type == kXAtomType && strcmp(name, iter->handle.atomName) == 0)
			{
			*item = *iter;
			if (remove)
				{
				display->itemList->erase(iter);
				}
			return true;
			}
		}

	return false;
}
