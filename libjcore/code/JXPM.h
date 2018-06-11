/******************************************************************************
 JXPM.h

	Defines struct for passing XPM format data to JImage.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPM
#define _H_JXPM

struct JXPM
{
	const char** xpm;		// must be standard xpm format

	JXPM(const char* data[])
		:
		xpm(data)
		{ };
};

#endif
