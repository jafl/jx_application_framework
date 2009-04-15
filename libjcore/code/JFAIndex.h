/******************************************************************************
 JFAIndex.h

	Index class for JFileArray class

	Copyright © 1994 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFAIndex
#define _H_JFAIndex

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

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

	JBoolean
	IsValid()
		const
	{
		return JConvertToBoolean( itsIndex != kInvalidIndex );
	};

private:

	JIndex itsIndex;
};

#endif
