/******************************************************************************
 cbUtil.h

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_cbUtil
#define _H_cbUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "jTypes.h"

void	CBCleanUpAfterRename(const JCharacter* origFullName,
							 const JCharacter* newFullName);

#endif
