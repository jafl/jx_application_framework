/******************************************************************************
 display.cpp

	Display functions.

	TODO:
		Implement shared state between all programs using this library
			to allow XGrabServer(), XSetCloseDownMode(RetainPermanent),
			XKillClient(non-local XID).

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <assert.h>

/******************************************************************************
 XOpenDisplay

 ******************************************************************************/

Display*
XOpenDisplay
	(
	_Xconst char* name
	)
{
	return xOpenDisplay(name);
}

/******************************************************************************
 XCloseDisplay

 ******************************************************************************/

int
XCloseDisplay
	(
	Display* display
	)
{
	xCloseDisplay(display);
	return TRUE;
}

/******************************************************************************
 InternAtom(s)

 ******************************************************************************/

Atom
XInternAtom
	(
	Display*	display,
	char*		atom_name,
	Bool		only_if_exists
	)
{
	xItem item;
	if (xFindAtom(display, atom_name, &item))
		{
		return item.id;
		}

	if (only_if_exists)
		{
		return None;
		}

	return xAllocateID(display, kXAtomType, atom_name);
}

Status
XInternAtoms
	(
	Display*	display,
	char**		names,
	int			count,
	Bool		only_if_exists,
	Atom*		atoms_return
	)
{
	Status result = TRUE;
	for (int i=0; i<count; i++)
		{
		atoms_return[i] = XInternAtom(display, names[i], only_if_exists);
		if (atoms_return[i] == None)
			{
			result = FALSE;
			}
		}

	return result;
}

/******************************************************************************
 XGetAtomName

 ******************************************************************************/

char*
XGetAtomName
	(
	Display*	display,
	Atom		atom
	)
{
	xItem item;
	if (xFindID(display, atom, &item))
		{
		return strdup(item.handle.atomName);
		}
	else
		{
		return NULL;
		}
}

/******************************************************************************
 XFlush

 ******************************************************************************/

int
XFlush
	(
	Display* display
	)
{
	return TRUE;
}

/******************************************************************************
 XSync

 ******************************************************************************/

int
XSync
	(
	Display*	display,
    Bool		discard
	)
{
	return TRUE;
}

/******************************************************************************
 XGrabServer

 ******************************************************************************/

int
XGrabServer
	(
	Display* display
	)
{
	return TRUE;
}

/******************************************************************************
 XUngrabServer

 ******************************************************************************/

int
XUngrabServer
	(
	Display* display
	)
{
	return TRUE;
}

/******************************************************************************
 XDisplayName

 ******************************************************************************/

char*
XDisplayName
	(
	_Xconst char* str
	)
{
	return "internal:0.0";
}

/******************************************************************************
 XKillClient

 ******************************************************************************/

int
XKillClient
	(
	Display*	display,
	XID			id
	)
{
	if (id == AllTemporary)
		{
		xCloseAllTemporary();
		}
	else if (xFindID(id, &display))
		{
		xKillDisplay(display);
		}

	return TRUE;
}

/******************************************************************************
 XSetCloseDownMode

 ******************************************************************************/

int
XSetCloseDownMode
	(
	Display*	display,
	int			mode
	)
{
	return TRUE;
}

/******************************************************************************
 XGrabKeyboard

 ******************************************************************************/

int
XGrabKeyboard
	(
	Display*	display,
	Window		window,
	Bool		ownerEvents,
	int			pointerMode,
	int			keyboardMode,
	Time		time
	)
{
	return FALSE;
}

/******************************************************************************
 XUngrabKeyboard

 ******************************************************************************/

int
XUngrabKeyboard
	(
	Display*	display,
	Time		time
	)
{
	return FALSE;
}

/******************************************************************************
 XGrabPointer

 ******************************************************************************/

int
XGrabPointer
	(
	Display*		display,
	Window			window,
	Bool			ownerEvents,
	unsigned int	eventMask,
	int				pointerMode,
	int				keyboardMode,
	Window			confineTo,
	Cursor			cursor,
	Time			time
	)
{
	return FALSE;
}

/******************************************************************************
 XUngrabPointer

 ******************************************************************************/

int
XUngrabPointer
	(
	Display*	display,
	Time		time
	)
{
	return FALSE;
}

/******************************************************************************
 XMaxRequestSize

 ******************************************************************************/

long
XMaxRequestSize
	(
	Display* display
	)
{
	return LONG_MAX;
}

/******************************************************************************
 ErrorText

 ******************************************************************************/

int
XGetErrorText
	(
	Display*	display,
	int			code,
	char*		buffer_return,
	int			length
	)
{
	_snprintf(buffer_return, length-1, "error #%d", code);
	buffer_return[ length-1 ] = '\0';
	return Success;
}

int
XGetErrorDatabaseText
	(
	Display*	display,
	char*		name,
	char*		message,
	char*		default_string,
	char*		buffer_return,
	int			length
	)
{
	strncpy(buffer_return, default_string, length-1);
	buffer_return[ length-1 ] = '\0';
	return Success;
}

/******************************************************************************
 ErrorHandler

	Not supported.

 ******************************************************************************/

static int
defaultXErrorHandler
	(
    Display*		display,
    XErrorEvent*	error_event
	)
{
	return Success;
}

static int
defaultXIOErrorHandler
	(
	Display* display
	)
{
	return Success;
}

XErrorHandler
XSetErrorHandler
	(
	XErrorHandler f
	)
{
	return defaultXErrorHandler;
}

XIOErrorHandler
XSetIOErrorHandler
	(
	XIOErrorHandler f
	)
{
	return defaultXIOErrorHandler;
}
