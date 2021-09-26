/******************************************************************************
 JCollection.h

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JCollection
#define _H_JCollection

#include "jx-af/jcore/JBroadcaster.h"

class JCollection : virtual public JBroadcaster
{
public:

	JCollection();
	JCollection(const JCollection& source);

	virtual ~JCollection();

	const JCollection& operator=(const JCollection& source);

	JSize	GetElementCount() const;
	bool	IsEmpty() const;
	bool	IndexValid(const JIndex index) const;
	JIndex	GetIndexFromEnd(const JIndex index) const;

	virtual JString	ToString() const override;

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

	Returns true if there are no items in the JCollection.

 ******************************************************************************/

inline bool
JCollection::IsEmpty()
	const
{
	return itsElementCount == 0;
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline bool
JCollection::IndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= itsElementCount;
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
