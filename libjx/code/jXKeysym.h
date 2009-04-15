/******************************************************************************
 jXKeysym.h

	Defines X keysym's that are not in X11R5

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <X11/keysym.h>

#ifndef XK_KP_Up
#define XK_KP_Up 0xFF97
#endif

#ifndef XK_KP_Down
#define XK_KP_Down 0xFF99
#endif

#ifndef XK_KP_Left
#define XK_KP_Left 0xFF96
#endif

#ifndef XK_KP_Right
#define XK_KP_Right 0xFF98
#endif

#ifndef XK_Page_Up
#define XK_Page_Up 0xFF55
#endif

#ifndef XK_KP_Page_Up
#define XK_KP_Page_Up 0xFF9A
#endif

#ifndef XK_Page_Down
#define XK_Page_Down 0xFF56
#endif

#ifndef XK_KP_Page_Down
#define XK_KP_Page_Down 0xFF9B
#endif

#ifndef XK_KP_Home
#define XK_KP_Home 0xFF95
#endif

#ifndef XK_KP_End
#define XK_KP_End 0xFF9C
#endif

#ifndef XK_ISO_Left_Tab
#define	XK_ISO_Left_Tab 0xFE20
#endif
