/******************************************************************************
 JFileID.h

	Copyright (C) 2001 John Lindal.

 ******************************************************************************/

#ifndef _H_JFileID
#define _H_JFileID

#include "jx-af/jcore/JList.h"
#include <sys/types.h>

class JString;

// Since ID's are permanent, using only 32 bits guarantees that files can be
// transferred from 64 to 32 bit machines.  GetUniqueID() will never return
// a value that can't be used on a 32 bit machine.

class JFileID
{
	friend bool operator==(const JFileID& lhs, const JFileID& rhs);
	friend bool operator!=(const JFileID& lhs, const JFileID& rhs);

public:

	JFileID();
	JFileID(const JString& fullName);

	bool	IsValid() const;
	void	SetFileName(const JString& fullName);

	static JListT::CompareResult
		Compare(const JFileID& id1, const JFileID& id2);

private:

	bool	itsValidFlag;
	dev_t		itsDevice;
	ino_t		itsINode;
};


/******************************************************************************
 IsValid

 ******************************************************************************/

inline bool
JFileID::IsValid()
	const
{
	return itsValidFlag;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

inline bool
operator!=
	(
	const JFileID& lhs,
	const JFileID& rhs
	)
{
	return !(lhs == rhs);
}

#endif
