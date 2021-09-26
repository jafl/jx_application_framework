/******************************************************************************
 SmileyBitmaps.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_SmileyBitmaps
#define _H_SmileyBitmaps

#include <jx-af/jcore/JConstBitmap.h>

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
