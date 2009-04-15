/******************************************************************************
 HelpMenuBitmaps.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_HelpMenuBitmaps
#define _H_HelpMenuBitmaps

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JConstBitmap.h>

const JSize kHelpIconCount      = 11;
const JIndex kGreyOutIcon1Index = 9;
const JIndex kGreyOutIcon2Index = 10;

extern JConstBitmap kHelpIcon[ kHelpIconCount ];

#endif
