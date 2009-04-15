/******************************************************************************
 JCollection.h

	Copyright © 1994 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JCollection
#define _H_JCollection

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>

class JCollection : virtual public JBroadcaster
{
public:

	JCollection();
	JCollection(const JCollection& source);

	virtual ~JCollection();

	const JCollection& operator=(const JCollection& source);

	JSize		GetElementCount() const;
	JBoolean	IsEmpty() const;
	JBoolean	IndexValid(const JIndex index) const;
	JIndex		GetIndexFromEnd(const JIndex index) const;

protected:

	void	ElementAdded();
	void	ElementRemoved();
	void	SetElementCount(const JSize newElementCount);

private:

	JSize itsElementCount;
};


/******************************************************************************
 GetElementCount

 ******************************************************************************/

inline JSize
JCollection::GetElementCount()
	const
{
	return itsElementCount;
}

/******************************************************************************
 IsEmpty

	Returns kJTrue if there are no items in the JCollection.

 ******************************************************************************/

inline JBoolean
JCollection::IsEmpty()
	const
{
	return JConvertToBoolean( itsElementCount == 0 );
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JCollection::IndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsElementCount );
}

/******************************************************************************
 ElementAdded (protected)

 ******************************************************************************/

inline void
JCollection::ElementAdded()
{
	itsElementCount++;
}

/******************************************************************************
 SetElementCount (protected)

 ******************************************************************************/

inline void
JCollection::SetElementCount
	(
	const JSize newElementCount
	)
{
	itsElementCount = newElementCount;
}

#endif
