/******************************************************************************
 JPolygon.h

	Someday, this might turn into a real class.  Right now, there doesn't
	seem to be any point in doing so, however.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPolygon
#define _H_JPolygon

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>

typedef JArray<JPoint>	JPolygon;

#endif
