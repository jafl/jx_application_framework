/******************************************************************************
 JCharacterRange.h

	Light-weight class for type safety.

	Copyright (C) 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JCharacterRange
#define _H_JCharacterRange

#include <JIndexRange.h>

class JCharacterRange : public JIndexRange
{
public:

	JCharacterRange()
		:
		JIndexRange()
	{ };

	JCharacterRange
		(
		const JIndex firstIndex,
		const JIndex lastIndex
		)
		:
		JIndexRange(firstIndex, lastIndex)
	{ };
};

#endif
