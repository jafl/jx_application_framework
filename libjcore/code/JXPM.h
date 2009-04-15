/******************************************************************************
 JXPM.h

	Defines struct for passing XPM format data to JImage.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPM
#define _H_JXPM

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

struct JXPM
{
	char** xpm;		// must be standard xpm format

	JXPM(char* data[])
		:
		xpm(data)
		{ };
};

#endif
