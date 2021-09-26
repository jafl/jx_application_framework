/******************************************************************************
 JFAIndex.h

	Index class for JFileArray class

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JFAIndex
#define _H_JFAIndex

#include "jx-af/jcore/jTypes.h"

class JFAIndex
{
public:

	static const JIndex kInvalidIndex;	// = 0

public:

	JFAIndex(const JIndex anIndex = kInvalidIndex)
		:
		itsIndex( anIndex )
	{ };

	JIndex
	GetIndex()
		const
	{
		return itsIndex;
	};

	void
	SetIndex
		(
		const JIndex newIndex
		)
	{
		itsIndex = newIndex;
	};

	bool
	IsValid()
		const
	{
		return itsIndex != kInvalidIndex;
	};

private:

	JIndex itsIndex;
};

#endif
