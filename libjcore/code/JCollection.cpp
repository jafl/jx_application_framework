/******************************************************************************
 JCollection.cpp

							The JCollection Class

	Abstract class for a collection of objects arranged in some kind of
	data structure. The common ground for collections is that they
	contain zero or more elements.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1994 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JCollection.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JCollection::JCollection()
{
	itsElementCount = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JCollection::~JCollection()
{
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
	itsElementCount = source.itsElementCount;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JCollection&
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

	itsElementCount = source.itsElementCount;

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

	return GetElementCount() - index + 1;
}

/******************************************************************************
 ElementRemoved (protected)

	Not inline because it uses assert

 ******************************************************************************/

void
JCollection::ElementRemoved()
{
	assert( !IsEmpty() );

	itsElementCount--;
}
