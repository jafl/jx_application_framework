/******************************************************************************
 JFAID.h

	ID class for JFileArray class

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JFAID
#define _H_JFAID

#include "jTypes.h"

// Since ID's are permanent, using only 32 bits guarantees that files can be
// transferred from 64 to 32 bit machines.  GetUniqueID() will never return
// a value that can't be used on a 32 bit machine.

typedef JUInt32 JFAID_t;

class JFAID
{
public:

	static const JFAID_t kInvalidID;	// = 0
	static const JFAID_t kMinID;		// = 1
	static const JFAID_t kMaxID;		// = kJUInt32Max - 1	// avoid rollover

public:

	JFAID(const JFAID_t anID = kInvalidID)
		:
		itsID( anID )
	{ };

	JFAID_t
	GetID()
		const
	{
		return itsID;
	};

	void
	SetID
		(
		const JFAID_t newID
		)
	{
		itsID = newID;
	};

	bool
	IsValid()
		const
	{
		return kMinID <= itsID && itsID <= kMaxID;
	};

private:

	JFAID_t itsID;
};

#endif
