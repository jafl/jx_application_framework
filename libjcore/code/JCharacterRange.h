/******************************************************************************
 JCharacterRange.h

	Light-weight class for type safety.

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_JCharacterRange
#define _H_JCharacterRange

#include "jx-af/jcore/JRange.h"

class JCharacterRange : public JRange<JIndex>
{
public:

	using JRange::JRange;

	explicit JCharacterRange
		(
		const JIndexRange& r
		)
		:
		JIndexRange(r)
	{ };
};

#endif
