/******************************************************************************
 JConstBitmap.h

	Copyright (C) 1996 by John Lindal.

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
