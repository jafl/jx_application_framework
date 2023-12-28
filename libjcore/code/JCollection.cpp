/******************************************************************************
 JCollection.cpp

							The JCollection Class

	Abstract class for a collection of objects arranged in some kind of
	data structure. The common ground for collections is that they
	contain zero or more items.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#include "JCollection.h"
#include "JString.h"
#include <sstream>
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JCollection::JCollection()
{
	itsItemCount = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JCollection::~JCollection()
{
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JCollection::ToString()
	const
{
	std::ostringstream s;
	JBroadcaster::ToString().Print(s);
	s << " (" << itsItemCount << " items)";
	return JString(s.str());
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JCollection::JCollection
	(
	const JCollection& source
	)
	:
	JBroadcaster(source)
{
	itsItemCount = source.itsItemCount;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

JCollection&
JCollection::operator=
	(
	const JCollection& source
	)
{
	if (this == &source)
	{
		return *this;
	}

	JBroadcaster::operator=(source);

	itsItemCount = source.itsItemCount;

	return *this;
}

/******************************************************************************
 GetIndexFromEnd

 ******************************************************************************/

JIndex
JCollection::GetIndexFromEnd
	(
	const JIndex index
	)
	const
{
	assert( IndexValid(index) );	// avoid wrap of unsigned value

	return GetItemCount() - index + 1;
}

/******************************************************************************
 ItemRemoved (protected)

	Not inline because it uses assert

 ******************************************************************************/

void
JCollection::ItemRemoved()
{
	assert( !IsEmpty() );

	itsItemCount--;
}
