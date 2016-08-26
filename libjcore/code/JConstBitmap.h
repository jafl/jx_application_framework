/******************************************************************************
 JConstBitmap.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JConstBitmap
#define _H_JConstBitmap

#include <jTypes.h>

struct JConstBitmap
{
	JCoordinate		w,h;
	unsigned char*	data;
};

#endif
