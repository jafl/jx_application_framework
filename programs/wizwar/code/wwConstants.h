/******************************************************************************
 wwConstants.h

	Constants shared by clients and server.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_wwConstants
#define _H_wwConstants

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

const JIndex kWWAllPlayersIndex = 0;

// color list, map layout

const JSize kWWMinPlayerCount = 2;
const JSize kWWMaxPlayerCount = 8;

#endif
