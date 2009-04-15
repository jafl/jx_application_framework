/******************************************************************************
 SmileyBitmaps.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_SmileyBitmaps
#define _H_SmileyBitmaps

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JConstBitmap.h>

enum
{
	kHappySmileyIndex = 0,
	kAmusedSmileyIndex,
	kNeutralSmileyIndex,
	kSadSmileyIndex,

	kSmileyBitmapCount
};

extern JConstBitmap kSmileyBitmap[ kSmileyBitmapCount ];

extern unsigned char kHappySmileyData[];
extern unsigned char kAmusedSmileyData[];
extern unsigned char kNeutralSmileyData[];
extern unsigned char kSadSmileyData[];

#endif
