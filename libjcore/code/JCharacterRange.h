/******************************************************************************
 JCharacterRange.h

	Light-weight class for type safety.

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_JCharacterRange
#define _H_JCharacterRange

#include "JRange.h"

class JCharacterRange : public JRange<JIndex>
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

	explicit JCharacterRange
		(
		const JIndexRange& r
		)
		:
		JIndexRange(r)
	{ };
};

#endif
