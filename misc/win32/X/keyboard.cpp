/******************************************************************************
 keyboard.cpp
 
	Keyboard event & mapping functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include <stdlib.h>
#include <assert.h>

/******************************************************************************
 XRefreshKeyboardMapping

	Not supported.

 ******************************************************************************/

int
XRefreshKeyboardMapping
	(
	XMappingEvent *event_map
	)
{
	return Success;
}
