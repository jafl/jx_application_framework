/******************************************************************************
 JFileID.h

	Copyright © 2001 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFileID
#define _H_JFileID

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JOrderedSet.h>
#include <sys/types.h>

// Since ID's are permanent, using only 32 bits guarantees that files can be
// transferred from 64 to 32 bit machines.  GetUniqueID() will never return
// a value that can't be used on a 32 bit machine.

class JFileID
{
	friend JBoolean operator==(const JFileID& lhs, const JFileID& rhs);
	friend JBoolean operator!=(const JFileID& lhs, const JFileID& rhs);

public:

	JFileID();
	JFileID(const JCharacter* fullName);

	JBoolean	IsValid() const;
	void		SetFileName(const JCharacter* fullName);

	static JOrderedSetT::CompareResult
		Compare(const JFileID& id1, const JFileID& id2);

private:

	JBoolean	itsValidFlag;
	dev_t		itsDevice;
	ino_t		itsINode;
};


/******************************************************************************
 IsValid

 ******************************************************************************/

inline JBoolean
JFileID::IsValid()
	const
{
	return itsValidFlag;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

inline JBoolean
operator!=
	(
	const JFileID& lhs,
	const JFileID& rhs
	)
{
	return JNegate(lhs == rhs);
}

#endif
