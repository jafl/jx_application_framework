/******************************************************************************
 J3DLibVersion.h

	Defines current version of J3D library

	Copyright (C) 1998-2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_J3DLibVersion
#define _H_J3DLibVersion

// These have to be #defined so they can be used for conditional compilation.

#define CURRENT_J3D_MAJOR_VERSION	0
#define CURRENT_J3D_MINOR_VERSION	2
#define CURRENT_J3D_PATCH_VERSION	1

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJ3DLibVersionStr = "0.2.1";

// 0.2.1
//	Updated to work with Mesa 7.1

// 0.2.0
//	Updated to work with Mesa 6.2
//	JX3DWidget:
//		Added rotation drag, mouse wheel zoom, and keyboard rotate/zoom.
//		Added Will/ShouldShowFocus().
//	Added J3DSurface.

// version 0.1.9:
//	Updated to work with Mesa 3.2

// version 0.1.6:
//	JX3DWidget:
//		Fixed bug so it redraws the border after getting and losing focus.

// version 0.1.2:
//	Updated to work with Mesa 3.0.

// version 0.1.1:
//	J3DObject:
//		Added functions for dealing with parent and children.
//		Added documentation explaining how to do animation.

#endif
