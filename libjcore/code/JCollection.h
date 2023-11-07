/******************************************************************************
 JCollection.h

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JCollection
#define _H_JCollection

#include "JBroadcaster.h"

class JCollection : virtual public JBroadcaster
{
public:

	JCollection();
	JCollection(const JCollection& source);

	~JCollection() override;

	JCollection& operator=(const JCollection& source);

	JSize	GetItemCount() const;
	bool	IsEmpty() const;
	bool	IndexValid(const JIndex index) const;
	JIndex	GetIndexFromEnd(const JIndex index) const;

	JString	ToString() const override;

protected:

	void	ItemAdded();
	void	ItemRemoved();
	void	SetItemCount(const JSize newItemCount);

private:

	JSize itsItemCount;
};


/******************************************************************************
 GetItemCount

 ******************************************************************************/

inline JSize
JCollection::GetItemCount()
	const
{
	return itsItemCount;
}

/******************************************************************************
 IsEmpty

	Returns true if there are no items in the JCollection.

 ******************************************************************************/

inline bool
JCollection::IsEmpty()
	const
{
	return itsItemCount == 0;
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
	return 1 <= index && index <= itsItemCount;
}

/******************************************************************************
 ItemAdded (protected)

 ******************************************************************************/

inline void
JCollection::ItemAdded()
{
	itsItemCount++;
}

/******************************************************************************
 SetItemCount (protected)

 ******************************************************************************/

inline void
JCollection::SetItemCount
	(
	const JSize newItemCount
	)
{
	itsItemCount = newItemCount;
}

#endif
