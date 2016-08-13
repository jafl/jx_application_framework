/******************************************************************************
 window.cpp

	Window functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <assert.h>

using namespace std;

/******************************************************************************
 XCreateWindow

	border_width is not supported.

	TODO
		process depth, c_class, visual, valuemask, attributes

 ******************************************************************************/

Window
XCreateWindow
	(
	Display*				display,
	Window					parent,
	int						x,
	int						y,
	unsigned int			width,
	unsigned int			height,
	unsigned int			border_width,
	int						depth,
	unsigned int			c_class,
	Visual*					visual,
	unsigned long			valuemask,
	XSetWindowAttributes*	attributes
	)
{
	DWORD style    = 0;
	HWND parentWnd = NULL;
	if (parent == DefaultRootWindow(display))
		{
		style |= WS_OVERLAPPEDWINDOW;
		}
	else
		{
		style |= WS_CHILDWINDOW;

		xItem item;
		if (!xFindID(display, parent, &item) ||
			item.type != kXWindowType)
			{
			return None;
			}
		parentWnd = (HWND) item.handle.window;
		}

	HWND window = CreateWindow(NULL, "", style, x, y, width, height,
							   parentWnd, NULL, GetModuleHandle(NULL), NULL);
	return xAllocateID(display, kXWindowType, window);
}

/******************************************************************************
 XDestroyWindow

 ******************************************************************************/

int
XDestroyWindow
	(
	Display*	display,
	Window		window
	)
{
	xFreeID(display, window);
	return TRUE;
}

/******************************************************************************
 Map/Unmap Window

 ******************************************************************************/

int
XMapWindow
	(
	Display*	display,
	Window		window
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		ShowWindow((HWND) item.handle.window, SW_SHOW);
		}
	return Success;
}

int
XMapRaised
	(
	Display*	display,
	Window		window
	)
{
	XMapWindow(display, window);
	XRaiseWindow(display, window);
	return Success;
}

int
XUnmapWindow
	(
	Display*	display,
	Window		window
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		ShowWindow((HWND) item.handle.window, SW_HIDE);
		}
	return Success;
}

/******************************************************************************
 Raise/Lower Window

 ******************************************************************************/

int
XRaiseWindow
	(
	Display*	display,
	Window		window
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		ShowWindow((HWND) item.handle.window, SW_SHOW);
		BringWindowToTop((HWND) item.handle.window);
		// SetActiveWindow((HWND) item.handle.window);
		}
	return Success;
}

int
XLowerWindow
	(
	Display*	display,
	Window		window
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		SetWindowPos((HWND) item.handle.window, HWND_BOTTOM, 0, 0, 0, 0,
					 SWP_NOMOVE | SWP_NOSIZE);
		}
	return Success;
}

Status
XIconifyWindow
	(
	Display*	display,
	Window		window,
	int			screen_number
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		ShowWindow((HWND) item.handle.window, SW_MINIMIZE);
		}
	return Success;
}

/******************************************************************************
 XSetInputFocus

 ******************************************************************************/

int
XSetInputFocus
	(
	Display*	display,
	Window		focus,
	int			revert_to,
	Time		time
	)
{
	xItem item;
	if (xFindID(display, focus, &item) &&
		item.type == kXWindowType)
		{
		SetForegroundWindow((HWND) item.handle.window);
		// SetFocus((HWND) item.handle.window);
		}
	return Success;
}

/******************************************************************************
 XClearWindow

	Not supported.

	TODO
		Eliminate need for this function,
		or support background pixmap the hard way.

 ******************************************************************************/

int
XClearWindow
	(
	Display*	display,
	Window		w
	)
{
	return Success;
}

/******************************************************************************
 XGetGeometry

	border_width is not supported.

	TODO
		*** What should x & y be?
		handle other types of drawables
		handle depth

 ******************************************************************************/

Status
XGetGeometry
	(
	Display*		display,
	Drawable		drawable,
	Window*			root,
	int*			x,
	int*			y,
	unsigned int*	width,
	unsigned int*	height,
	unsigned int*	border_width,
	unsigned int*	depth
	)
{
	xItem item;
	const bool found = xFindID(display, drawable, &item);
	if (found && item.type == kXWindowType)
		{
		RECT r;
		if (!GetWindowRect((HWND) item.handle.window, &r))
			{
			return FALSE;
			}
		*x            = 0;
		*y            = 0;
		*width        = r.right - r.left;
		*height       = r.bottom - r.top;
		*border_width = 0;
		*depth        = 32;
		return TRUE;
		}
	else
		{
		return FALSE;
		}
}

/******************************************************************************
 XMoveResizeWindow

 ******************************************************************************/

int
XMoveResizeWindow
	(
	Display*		display,
	Window			window,
	int				x,
	int				y,
	unsigned int	width,
	unsigned int	height
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		MoveWindow((HWND) item.handle.window, x, y, width, height, TRUE);
		return TRUE;
		}
	return FALSE;
}

/******************************************************************************
 XMoveWindow

 ******************************************************************************/

int
XMoveWindow
	(
	Display*	display,
	Window		window,
	int			x,
	int			y
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		SetWindowPos((HWND) item.handle.window, NULL, x, y, 0, 0,
					 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		return TRUE;
		}
	return FALSE;
}

/******************************************************************************
 XResizeWindow

 ******************************************************************************/

int
XResizeWindow
	(
	Display*		display,
	Window			window,
	unsigned int	width,
	unsigned int	height
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		SetWindowPos((HWND) item.handle.window, NULL, 0, 0, width, height,
					 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		return TRUE;
		}
	return FALSE;
}

/******************************************************************************
 XReparentWindow

 ******************************************************************************/

int
XReparentWindow
	(
	Display*	display,
	Window		window,
	Window		parent,
	int			x,
	int			y
	)
{
	xItem item1, item2;
	if (xFindID(display, window, &item1) && item1.type == kXWindowType &&
		xFindID(display, parent, &item2) && item2.type == kXWindowType)
		{
		if (SetParent((HWND) item1.handle.window, (HWND) item2.handle.window) != NULL)
			{
			SetWindowPos((HWND) item1.handle.window, NULL, x, y, 0, 0,
						 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
			return TRUE;
			}
		}
	return FALSE;
}

/******************************************************************************
 XSetWindowColormap

	TODO - Not supported?

 ******************************************************************************/

int
XSetWindowColormap
	(
	Display*	display,
	Window		window,
	Colormap	colormap
	)
{
	return Success;
}

/******************************************************************************
 XSetWindowBackground

	TODO - Not supported?

 ******************************************************************************/

int
XSetWindowBackground
	(
	Display*		display,
	Window			window,
	unsigned long	background_pixel
	)
{
	return Success;
}

/******************************************************************************
 XSetWindowBackgroundPixmap

	TODO - Not supported?

 ******************************************************************************/

int
XSetWindowBackgroundPixmap
	(
	Display*	display,
	Window		window,
	Pixmap		background_pixmap
	)
{
	return Success;
}

/******************************************************************************
 XGetWindowAttributes

	TODO: fill in rest of struct elements

 ******************************************************************************/

Status
XGetWindowAttributes
	(
	Display*			display,
	Window				window,
	XWindowAttributes*	attributes
	)
{
	xItem item;
	const bool found = xFindID(display, window, &item);
	if (found && item.type == kXWindowType)
		{
		RECT r;
		if (!GetWindowRect((HWND) item.handle.window, &r))
			{
			return FALSE;
			}

		POINT pt = { r.left, r.top };
		ScreenToClient(GetParent((HWND) item.handle.window), &pt);

		attributes->x                     = pt.x;
		attributes->y                     = pt.y;
		attributes->width                 = r.right - r.left;
		attributes->height                = r.bottom - r.top;
		attributes->border_width          = 0;
		attributes->depth                 = 32;
		attributes->visual                = NULL;
		attributes->root                  = DefaultRootWindow(display);
		attributes->c_class               = InputOutput;
		attributes->bit_gravity           = 0;
		attributes->win_gravity           = 0;
		attributes->backing_store         = NotUseful;
		attributes->backing_planes        = 0;
		attributes->backing_pixel         = 0;
		attributes->save_under            = FALSE;
		attributes->colormap              = None;
		attributes->map_installed         = TRUE;
		attributes->map_state             = IsViewable;
		attributes->all_event_masks       = 0;
		attributes->your_event_mask       = 0;
		attributes->do_not_propagate_mask = 0;
		attributes->override_redirect     = FALSE;
		attributes->screen                = NULL;

		return TRUE;
		}
	return FALSE;
}

/******************************************************************************
 XChangeWindowAttributes

	TODO

 ******************************************************************************/

int
XChangeWindowAttributes
	(
	Display*				display,
	Window					window,
	unsigned long			valueMask,
	XSetWindowAttributes*	attributes
	)
{
	return FALSE;
}

/******************************************************************************
 XListProperties

 ******************************************************************************/

Atom*
XListProperties
	(
	Display*	display,
	Window		window,
	int*		num_prop_return
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType &&
		item.data.propList != NULL)
		{
		*num_prop_return = item.data.propList->size();

		Atom* result = (Atom*) malloc(*num_prop_return * sizeof(Atom));
		if (result == NULL)
			{
			*num_prop_return = 0;
			return NULL;
			}

		Atom* p = result;
		vector<xWindowProperty>::iterator iter;
		for (iter = item.data.propList->begin(); iter < item.data.propList->end(); iter++)
			{
			*p = iter->property;
			p++;
			}

		return result;
		}

	*num_prop_return = 0;
	return NULL;
}

/******************************************************************************
 XGetWindowProperty

 ******************************************************************************/

int
XGetWindowProperty
	(
	Display*		display,
	Window			window,
	Atom			property,
	long			long_offset,
	long			long_length,
	Bool			delete_property,
	Atom			req_type,
	Atom*			actual_type_return,
	int*			actual_format_return,
	unsigned long*	nitems_return,
	unsigned long*	bytes_after_return,
	unsigned char**	prop_return
	)
{
	*actual_type_return   = None;
	*actual_format_return = 0;
	*nitems_return        = 0;
	*bytes_after_return   = 0;
	*prop_return          = NULL;

	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType &&
		item.data.propList != NULL)
		{
		vector<xWindowProperty>::iterator iter;
		for (iter = item.data.propList->begin(); iter < item.data.propList->end(); iter++)
			{
			if (iter->property == property &&
				(iter->type == AnyPropertyType || iter->type == req_type))
				{
				unsigned long N = iter->length;
				unsigned long I = 4 * long_offset;
				unsigned long T = N - I;
				unsigned long L = MIN(T, 4 * long_length);
				unsigned long A = N - (I + L);

				if (L < 0)
					{
					return BadValue;
					}
				else
					{
					*prop_return = (unsigned char*) malloc(L+1);
					if (*prop_return == NULL)
						{
						return BadAlloc;
						}

					memcpy(*prop_return, iter->data + I, L);
					(*prop_return)[L] = '\0';
					}

				*actual_type_return   = iter->type;
				*actual_format_return = iter->format;
				*nitems_return        = L / iter->format;
				*bytes_after_return   = A;

				if (delete_property && A == 0)
					{
					iter->Free();
					item.data.propList->erase(iter);
					}

				return Success;
				}
			else if (iter->property == property)
				{
				*actual_type_return   = iter->type;
				*actual_format_return = iter->format;
				*nitems_return        = 0;
				*bytes_after_return   = iter->length;
				*prop_return          = NULL;
				return Success;
				}
			}
		}

	return -1;
}

/******************************************************************************
 XChangeProperty

	TODO

 ******************************************************************************/

int
XChangeProperty
	(
	Display*		display,
	Window			window,
	Atom			property,
	Atom			type,
	int				format,
	int				mode,
	unsigned char*	data,
	int				nelements
	)
{
	return Success;
}

/******************************************************************************
 XDeleteProperty

 ******************************************************************************/

int
XDeleteProperty
	(
	Display*	display,
	Window		window,
	Atom		property
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType &&
		item.data.propList != NULL)
		{
		vector<xWindowProperty>::iterator iter;
		for (iter = item.data.propList->begin(); iter < item.data.propList->end(); iter++)
			{
			if (iter->property == property)
				{
				iter->Free();
				item.data.propList->erase(iter);
				break;
				}
			}
		}

	return Success;
}

/******************************************************************************
 XSetWMProperties

	TODO

 ******************************************************************************/

void
XSetWMProperties
	(
	Display*		display,
	Window			window,
	XTextProperty*	window_name,
	XTextProperty*	icon_name,
	char**			argv,
	int				argc,
	XSizeHints*		normal_hints,
	XWMHints*		wm_hints,
	XClassHint*		class_hints
	)
{
}

/******************************************************************************
 WMName

 ******************************************************************************/

Status
XGetWMName
	(
	Display*		display,
	Window			window,
	XTextProperty*	text_prop_return
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		char title[1024];
		const int length = GetWindowText((HWND) item.handle.window, title, 1024);
		if (length > 0)
			{
			return XStringListToTextProperty((char**) &title, 1, text_prop_return);
			}
		}

	return FALSE;
}

void
XSetWMName
	(
	Display*		display,
	Window			window,
	XTextProperty*	text_prop
	)
{
	xItem item;
	if (xFindID(display, window, &item) &&
		item.type == kXWindowType)
		{
		SetWindowText((HWND) item.handle.window, (char*) text_prop->value);
		}
}

/******************************************************************************
 WMIconName

	Not supported.

 ******************************************************************************/

Status
XGetWMIconName
	(
	Display*		display,
	Window			window,
	XTextProperty*	text_prop_return
	)
{
	return TRUE;
}

void
XSetWMIconName
	(
	Display*		display,
	Window			window,
	XTextProperty*	text_prop
	)
{
}

/******************************************************************************
 WMNormalHints

	TODO - get
		return hints
		process supplied
	TODO - set
		save hints in xInfo
		process WM_GETMINMAXINFO to enforce min & max size

 ******************************************************************************/

Status
XGetWMNormalHints
	(
	Display*	display,
	Window		window,
	XSizeHints*	hints,
	long*		supplied
	)
{
	return FALSE;
}

void
XSetWMNormalHints
	(
	Display*	display,
	Window		window,
	XSizeHints*	hints
	)
{
}

/******************************************************************************
 WMHints

	TODO

 ******************************************************************************/

XWMHints*
XGetWMHints
	(
	Display*	display,
	Window		window
	)
{
	return NULL;
}

int
XSetWMHints
	(
	Display*	display,
	Window		window,
	XWMHints*	wmhints
	)
{
	return Success;
}

/******************************************************************************
 ClassHint

	TODO - Not supported?

 ******************************************************************************/

Status
XGetClassHint
	(
	Display*	display,
	Window		window,
	XClassHint*	class_hints_return
	)
{
	return FALSE;
}

int
XSetClassHint
	(
	Display*	display,
	Window		window,
	XClassHint*	class_hints
	)
{
	return Success;
}

/******************************************************************************
 TransientForHint

	TODO - Not supported?

 ******************************************************************************/

Status
XGetTransientForHint
	(
	Display*	display,
	Window		window,
	Window*		prop_window_return
	)
{
	return FALSE;
}

int
XSetTransientForHint
	(
	Display*	display,
	Window		window,
	Window		prop_window
	)
{
	return Success;
}

/******************************************************************************
 XSetWMProtocols

	Not supported.

 ******************************************************************************/

Status
XSetWMProtocols
	(
	Display*	display,
	Window		window,
	Atom*		protocols,
	int			count
	)
{
	return TRUE;
}

/******************************************************************************
 XSetCommand

	Not supported.

 ******************************************************************************/

int
XSetCommand
	(
	Display*	display,
	Window		window,
	char**		argv,
	int			argc
	)
{
	return Success;
}
