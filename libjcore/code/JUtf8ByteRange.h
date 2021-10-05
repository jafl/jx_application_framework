/******************************************************************************
 JUtf8ByteRange.h

	Light-weight class for type safety.

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_JUtf8ByteRange
#define _H_JUtf8ByteRange

#include "jx-af/jcore/JRange.h"

class JUtf8ByteRange : public JRange<JIndex>
{
public:

	using JRange::JRange;

	explicit JUtf8ByteRange
		(
		const JIndexRange& r
		)
		:
		JIndexRange(r)
	{ };
};

#endif
