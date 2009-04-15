/***************************************************************************************
 scNetlistUtil.h

	Interface for scNetlistUtil.cc

	Copyright © 1995 by John Lindal. All rights reserved.

 ***************************************************************************************/

#ifndef _H_scNetlistUtil
#define _H_scNetlistUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

JFloat GetSpiceMultiplier(const JCharacter* type);

#endif
