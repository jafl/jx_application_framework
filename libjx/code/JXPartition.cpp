/******************************************************************************
 JXPartition.cpp

	Base class for JX for a set of compartments arranged horizontally or vertically.

	This is not a JXScrollableWidget because adjustable partitions would
	be pointless in that case.  One only needs partitions when there is
	a limited amount of space available.

	This class implements CreateCompartmentObject() and DeleteCompartmentObject().
	Derived classes must implement GetTotalSize(), SetCompartmentSizes(),
	and the following routines:

		CreateCompartment
			Create a JXWidgetSet to be used as a compartment object.

	BASE CLASS = JXWidgetSet, JPartition

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPartition.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

	*** derived class must create compartments

 ******************************************************************************/

JXPartition::JXPartition
	(
	const JArray<JCoordinate>&	sizes,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minSizes,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h),
	JPartition(sizes, elasticIndex, minSizes)
{
	itsCompartments = new JPtrArray<JXContainer>(JPtrArrayT::kForgetAll);
	assert( itsCompartments != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPartition::~JXPartition()
{
	delete itsCompartments;		// objects deleted by JXContainer
}

/******************************************************************************
 Min comparment size

 ******************************************************************************/

JCoordinate
JXPartition::GetMinCompartmentSize
	(
	JXContainer* compartment
	)
	const
{
	JIndex i;
	const JBoolean found = GetCompartmentIndex(compartment, &i);
	assert( found );
	return JPartition::GetMinCompartmentSize(i);
}

JBoolean
JXPartition::GetMinCompartmentSize
	(
	JXContainer*	compartment,
	JCoordinate*	minSize
	)
	const
{
	JIndex i;
	if (GetCompartmentIndex(compartment, &i))
		{
		*minSize = JPartition::GetMinCompartmentSize(i);
		return kJTrue;
		}
	else
		{
		*minSize = -1;
		return kJFalse;
		}
}

void
JXPartition::SetMinCompartmentSize
	(
	JXContainer*		compartment,
	const JCoordinate	minSize
	)
{
	JIndex i;
	if (GetCompartmentIndex(compartment, &i))
		{
		JPartition::SetMinCompartmentSize(i, minSize);
		}
}

/******************************************************************************
 Elastic compartment

 ******************************************************************************/

JBoolean
JXPartition::GetElasticCompartment
	(
	JXContainer** compartment
	)
	const
{
	JIndex i;
	if (GetElasticIndex(&i))
		{
		*compartment = GetCompartment(i);
		return kJTrue;
		}
	else
		{
		*compartment = NULL;
		return kJFalse;
		}
}

void
JXPartition::SetElasticCompartment
	(
	JXContainer* compartment
	)
{
	JIndex i;
	if (GetCompartmentIndex(compartment, &i))
		{
		SetElasticIndex(i);
		}
}

/******************************************************************************
 DeleteCompartment

 ******************************************************************************/

void
JXPartition::DeleteCompartment
	(
	JXContainer* compartment
	)
{
	JIndex i;
	if (GetCompartmentIndex(compartment, &i))
		{
		JPartition::DeleteCompartment(i);
		}
}

/******************************************************************************
 CreateInitialCompartments (protected)

	*** Only for use by constructors!  Must not be called more than once.

 ******************************************************************************/

void
JXPartition::CreateInitialCompartments()
{
	assert( itsCompartments->IsEmpty() );

	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate position         = 0;
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		JCoordinate size = GetCompartmentSize(i);
		assert( size >= JPartition::GetMinCompartmentSize(i) );

		JXWidgetSet* compartment = CreateCompartment(i, position, size);
		itsCompartments->Append(compartment);

		position += size + kDragRegionSize;
		}
}

/******************************************************************************
 InsertCompartment

	Adjusts the adjacent compartments to make space and then creates a new
	compartment.  Returns kJFalse if there isn't enough space available for
	at least minSize.

 ******************************************************************************/

JBoolean
JXPartition::InsertCompartment
	(
	const JIndex		index,
	const JCoordinate	reqSize,
	const JCoordinate	minSize,
	JXContainer**		newCompartment
	)
{
	if (JPartition::InsertCompartment(index, reqSize, minSize))
		{
		*newCompartment = GetCompartment(index);
		return kJTrue;
		}
	else
		{
		*newCompartment = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 CreateCompartmentObject (virtual protected)

 ******************************************************************************/

void
JXPartition::CreateCompartmentObject
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	JXContainer* newCompartment = CreateCompartment(index, position, size);
	itsCompartments->InsertAtIndex(index, newCompartment);
}

/******************************************************************************
 DeleteCompartmentObject (virtual protected)

 ******************************************************************************/

void
JXPartition::DeleteCompartmentObject
	(
	const JIndex index
	)
{
	itsCompartments->DeleteElement(index);
}

/******************************************************************************
 ApertureResized (virtual protected)

	Adjust the sizes of the compartments.

 ******************************************************************************/

void
JXPartition::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidgetSet::ApertureResized(dw,dh);
	PTBoundsChanged();
}
