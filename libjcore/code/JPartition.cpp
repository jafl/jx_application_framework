/******************************************************************************
 JPartition.cpp

	System independent base class for a set of compartments arranged
	horizontally or vertically.  We cannot store the actual compartment
	objects because they will be derived from a system dependent base class.

	*** The size of each compartment must be positive and at least equal
		to the corresponding minimum size *at all times*.  It is the client's
		responsibility to insure that the enclosing window will never
		get too small.

	The default behavior when the entire partition is resized:
	(To change this, override PTBoundsChanged().)

	  * If all compartments are elastic, each one changes size by the
		same amount.
	  * If a single compartment is designated as elastic, only this
		one changes size.

	Derived classes must implement the following routines:

		GetTotalSize
			Return the total size of the partition.

		SetCompartmentSizes
			Adjust the size of each compartment object to match our values.

		CreateCompartmentObject
			Create a compartment object and insert it at the specified index.

		DeleteCompartmentObject
			Delete the specified compartment object.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPartition.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JCoordinate kDragRegionHalfSize = JPartition::kDragRegionSize/2;

const JFileVersion kCurrentGeometryDataVersion = 1;
const JCharacter kGeometryDataEndDelimiter     = '\1';

	// version 1: removed elastic index and min sizes

/******************************************************************************
 Constructor (protected)

	*** derived class must create compartments

	'sizes' contains the initial sizes of the compartments.  If a single
	compartment is elastic, then the size for this compartment is calculated
	rather than being retrieved from the array.

	Specify elasticIndex=0 if you want all compartments to be elastic.

 ******************************************************************************/

JPartition::JPartition
	(
	const JArray<JCoordinate>&	sizes,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minSizes
	)
{
	const JSize compartmentCount = sizes.GetElementCount();
	assert( compartmentCount == minSizes.GetElementCount() );
	assert( elasticIndex <= compartmentCount );

	itsSizes = new JArray<JCoordinate>(sizes);
	assert( itsSizes != NULL );

	itsMinSizes = new JArray<JCoordinate>(minSizes);
	assert( itsMinSizes != NULL );

	itsElasticIndex = elasticIndex;
	itsDragIndex    = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPartition::~JPartition()
{
	delete itsSizes;
	delete itsMinSizes;
}

/******************************************************************************
 SetCompartmentSize

	Adjusts the adjacent compartments to make space.

	Returns kJFalse if there isn't enough space available.

 ******************************************************************************/

JBoolean
JPartition::SetCompartmentSize
	(
	const JIndex		index,
	const JCoordinate	reqSize
	)
{
	const JCoordinate origSize = GetCompartmentSize(index);

	JArray<JCoordinate> newSizes;
	JCoordinate trueSize;
	if (reqSize > origSize &&
		CreateSpace(*itsSizes, *itsMinSizes, itsElasticIndex,
					reqSize - origSize, reqSize - origSize,
					&newSizes, &trueSize))
		{
		assert( trueSize == reqSize - origSize );
		newSizes.SetElement(index, reqSize);

		*itsSizes = newSizes;
		SetCompartmentSizes();
		return kJTrue;
		}
	else if (reqSize < origSize)
		{
		itsSizes->SetElement(index, reqSize);
		FillSpace(*itsSizes, itsElasticIndex, origSize - reqSize, &newSizes);
		*itsSizes = newSizes;
		SetCompartmentSizes();
		return kJTrue;
		}
	else
		{
		return JI2B(reqSize == origSize);
		}
}

/******************************************************************************
 SetCompartmentSizes

	Adjust the size of each compartment.

 ******************************************************************************/

void
JPartition::SetCompartmentSizes
	(
	const JArray<JCoordinate>& sizes
	)
{
	assert( GetCompartmentCount() == sizes.GetElementCount() );

	*itsSizes = sizes;
	SetElasticSize();
	SetCompartmentSizes();
}

/******************************************************************************
 GetMinTotalSize

 ******************************************************************************/

JCoordinate
JPartition::GetMinTotalSize()
	const
{
	const JSize compartmentCount = GetCompartmentCount();

	JCoordinate minSize = kDragRegionSize * (compartmentCount-1);
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		minSize += GetMinCompartmentSize(i);
		}

	return minSize;
}

/******************************************************************************
 FindCompartment

	If the point is between compartments i and i+1, the index returned is i.

 ******************************************************************************/

JBoolean
JPartition::FindCompartment
	(
	const JCoordinate	coord,
	JIndex*				index
	)
	const
{
	JCoordinate position = 0;

	const JSize compartmentCount = GetCompartmentCount();
	for (JIndex i=1; i<compartmentCount; i++)
		{
		position += GetCompartmentSize(i) + kDragRegionSize;
		if (coord <= position)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 InsertCompartment (protected)

	Adjusts the adjacent compartments to make space.

	Returns kJFalse if there isn't enough space available for at least minSize.

 ******************************************************************************/

JBoolean
JPartition::InsertCompartment
	(
	const JIndex		index,
	const JCoordinate	reqSize,
	const JCoordinate	minSize
	)
{
	JArray<JCoordinate> newSizes;
	JCoordinate trueSize;
	if (CreateSpace(*itsSizes, *itsMinSizes, itsElasticIndex,
					reqSize + kDragRegionSize, minSize + kDragRegionSize,
					&newSizes, &trueSize))
		{
		trueSize -= kDragRegionSize;

		JCoordinate position = 0;
		for (JIndex i=1; i<index; i++)
			{
			position += newSizes.GetElement(i) + kDragRegionSize;
			}

		CreateCompartmentObject(index, position, trueSize);
		newSizes.InsertElementAtIndex(index, trueSize);
		itsMinSizes->InsertElementAtIndex(index, minSize);

		if (index <= itsElasticIndex)
			{
			itsElasticIndex++;
			}

		*itsSizes = newSizes;
		SetCompartmentSizes();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 DeleteCompartment

	Remove the compartment and adjust the adjacent compartments to
	fill the gap.

 ******************************************************************************/

void
JPartition::DeleteCompartment
	(
	const JIndex index
	)
{
	const JSize compartmentCount = GetCompartmentCount();
	const JCoordinate fillSize   = itsSizes->GetElement(index) + kDragRegionSize;

	itsSizes->RemoveElement(index);
	itsMinSizes->RemoveElement(index);
	DeleteCompartmentObject(index);

	if (index < itsElasticIndex)
		{
		itsElasticIndex--;
		}
	else if (index == itsElasticIndex && index < compartmentCount)
		{
		itsElasticIndex = index;
		}
	else if (index == itsElasticIndex)
		{
		itsElasticIndex = compartmentCount-1;
		}

	JArray<JCoordinate> newSizes;
	FillSpace(*itsSizes, itsElasticIndex, fillSize, &newSizes);
	*itsSizes = newSizes;
	SetCompartmentSizes();
}

/******************************************************************************
 CreateSpace (private)

	Shrinks the compartments to create extra space.  If one of the compartments
	is elastic, this is shrunk first.

	Returns kJFalse if there isn't enough space available for at least minSize.

 ******************************************************************************/

JBoolean
JPartition::CreateSpace
	(
	const JArray<JCoordinate>&	origSizes,
	const JArray<JCoordinate>&	minSizes,
	const JIndex				elasticIndex,
	const JCoordinate			reqSize,
	const JCoordinate			minReqSize,
	JArray<JCoordinate>*		newSizes,
	JCoordinate*				newSpace
	)
	const
{
	assert( reqSize >= 0 && minReqSize >= 0 && reqSize >= minReqSize );

	*newSpace = 0;
	if (reqSize == 0)
		{
		*newSizes = origSizes;
		return kJTrue;
		}

	// try shrinking the elastic compartment first

	if (elasticIndex > 0)
		{
		const JCoordinate size    = origSizes.GetElement(elasticIndex);
		const JCoordinate minSize = minSizes.GetElement(elasticIndex);

		const JCoordinate availSpace = size - minSize;
		if (availSpace >= reqSize)
			{
			*newSizes = origSizes;
			newSizes->SetElement(elasticIndex, size - reqSize);
			*newSpace = reqSize;
			return kJTrue;
			}
		}

	// distribute the load among all the other compartments
	// (shrink each to min, then expand to fill whatever space is left)

	const JSize compartmentCount = origSizes.GetElementCount();

	JCoordinate currSize = 0;
	JCoordinate minSize  = 0;
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		currSize += origSizes.GetElement(i);
		minSize  += minSizes.GetElement(i);
		}

	if (currSize < minSize + minReqSize)
		{
		return kJFalse;
		}
	else if (currSize < minSize + reqSize)
		{
		*newSizes = minSizes;
		*newSpace = currSize - minSize;
		}
	else
		{
		FillSpace(minSizes, 0, currSize - minSize - reqSize, newSizes);
		*newSpace = reqSize;
		}

	return kJTrue;
}

/******************************************************************************
 FillSpace (private)

	Expands the compartments to remove extra space.  If one of the
	compartments is elastic, this alone is expanded.

 ******************************************************************************/

void
JPartition::FillSpace
	(
	const JArray<JCoordinate>&	origSizes,
	const JIndex				elasticIndex,
	const JCoordinate			fillSize,
	JArray<JCoordinate>*		newSizes
	)
	const
{
	assert( fillSize >= 0 );

	*newSizes = origSizes;
	if (fillSize == 0)
		{
		return;
		}

	if (elasticIndex > 0)
		{
		// expand the elastic compartment

		const JCoordinate size = newSizes->GetElement(elasticIndex);
		newSizes->SetElement(elasticIndex, size + fillSize);
		}
	else
		{
		// distribute the space among all the compartments

		const JSize compartmentCount = origSizes.GetElementCount();

		JCoordinate dsizeMax = fillSize/compartmentCount;
		if (fillSize % compartmentCount > 0)
			{
			dsizeMax++;
			}

		JCoordinate usedSize = 0;
		for (JIndex i=1; i<=compartmentCount; i++)
			{
			const JCoordinate size = newSizes->GetElement(i);
			JCoordinate newSize    = size + dsizeMax;
			if (usedSize + newSize - size > fillSize)
				{
				newSize = size + fillSize - usedSize;
				}
			usedSize += newSize - size;
			newSizes->SetElement(i, newSize);
			}
		}
}

/******************************************************************************
 SetElasticSize (protected)

 ******************************************************************************/

void
JPartition::SetElasticSize()
{
	if (itsElasticIndex > 0)
		{
		const JSize compartmentCount = GetCompartmentCount();
		JCoordinate elasticSize      = GetTotalSize();
		for (JIndex i=1; i<=compartmentCount; i++)
			{
			if (i != itsElasticIndex)
				{
				elasticSize -= GetCompartmentSize(i) + kDragRegionSize;
				}
			}
		itsSizes->SetElement(itsElasticIndex, elasticSize);
		}
}

/******************************************************************************
 PrepareToDrag

	Prepare to drag dividing line between two adjacent compartments.

 ******************************************************************************/

void
JPartition::PrepareToDrag
	(
	const JCoordinate	coord,
	JCoordinate*		minDragCoord,
	JCoordinate*		maxDragCoord
	)
{
	itsDragIndex         = 0;
	itsStartCoord        = 0;
	JCoordinate position = 0;

	const JSize compartmentCount = GetCompartmentCount();
	for (JIndex i=1; i<compartmentCount; i++)
		{
		itsDragMin     = position;
		*minDragCoord  = itsDragMin + GetMinCompartmentSize(i) + kDragRegionHalfSize;
		position      += GetCompartmentSize(i) + kDragRegionSize;
		if (coord <= position)
			{
			itsDragIndex  = i;
			itsDragMax    = position + GetCompartmentSize(i+1);
			*maxDragCoord = itsDragMax -
							(GetMinCompartmentSize(i+1) + kDragRegionHalfSize + 1);
			break;
			}
		}

	assert( itsDragIndex > 0 );
	assert( *maxDragCoord >= *minDragCoord );
}

/******************************************************************************
 AdjustCompartmentsAfterDrag

	Shift space from one compartment to the other.

 ******************************************************************************/

void
JPartition::AdjustCompartmentsAfterDrag
	(
	const JCoordinate coord
	)
{
	itsSizes->SetElement(itsDragIndex,
						 coord - itsDragMin - kDragRegionHalfSize);
	itsSizes->SetElement(itsDragIndex + 1,
						 itsDragMax - coord - kDragRegionHalfSize - 1);

	SetCompartmentSizes();
}

/******************************************************************************
 PrepareToDragAll

	Prepare to drag dividing line between two adjacent compartments and
	allow other compartments to shrink to get more space.

 ******************************************************************************/

void
JPartition::PrepareToDragAll
	(
	const JCoordinate	coord,
	JCoordinate*		minDragCoord,
	JCoordinate*		maxDragCoord
	)
{
JIndex i;

	itsDragIndex         = 0;
	itsDragMin           = 0;
	JCoordinate position = 0;

	const JSize compartmentCount = GetCompartmentCount();
	for (i=1; i<compartmentCount; i++)
		{
		itsDragMin += GetMinCompartmentSize(i) + kDragRegionSize;
		position   += GetCompartmentSize(i) + kDragRegionSize;
		if (coord <= position)
			{
			itsDragIndex   = i;
			itsStartCoord  = position - kDragRegionHalfSize - 1;
			itsDragMin    -= kDragRegionHalfSize - 1;
			break;
			}
		}

	assert( itsDragIndex > 0 );

	itsDragMax = GetTotalSize();
	for (i=compartmentCount; i>itsDragIndex; i--)
		{
		itsDragMax -= GetMinCompartmentSize(i) + kDragRegionSize;
		}
	itsDragMax += kDragRegionHalfSize;

	assert( itsDragMax >= itsDragMin );

	*minDragCoord = itsDragMin;
	*maxDragCoord = itsDragMax;
}

/******************************************************************************
 AdjustCompartmentsAfterDragAll

	Expand one compartment at the expense of all the others.

 ******************************************************************************/

void
JPartition::AdjustCompartmentsAfterDragAll
	(
	const JCoordinate coord
	)
{
JIndex i;

	// compress compartments in front of itsDragIndex+1

	if (coord < itsStartCoord)
		{
		JArray<JCoordinate> origSizes = *itsSizes;
		JArray<JCoordinate> minSizes  = *itsMinSizes;
		JSize count = origSizes.GetElementCount();
		for (i=count; i>itsDragIndex; i--)
			{
			origSizes.RemoveElement(i);
			minSizes.RemoveElement(i);
			}

		JArray<JCoordinate> newSizes;
		const JCoordinate reqSize = itsStartCoord - coord;
		JCoordinate newSpace;
		const JBoolean ok = CreateSpace(origSizes, minSizes, 0, reqSize, reqSize,
										&newSizes, &newSpace);
		assert( ok );

		for (i=1; i<=itsDragIndex; i++)
			{
			itsSizes->SetElement(i, newSizes.GetElement(i));
			}
		itsSizes->SetElement(itsDragIndex+1,
							 itsSizes->GetElement(itsDragIndex+1) + reqSize);

		SetCompartmentSizes();
		}

	// compress compartments after itsDragIndex

	else if (coord > itsStartCoord)
		{
		const JSize compartmentCount = GetCompartmentCount();

		JArray<JCoordinate> origSizes = *itsSizes;
		JArray<JCoordinate> minSizes  = *itsMinSizes;
		for (i=1; i<=itsDragIndex; i++)
			{
			origSizes.RemoveElement(1);
			minSizes.RemoveElement(1);
			}

		JArray<JCoordinate> newSizes;
		const JCoordinate reqSize = coord - itsStartCoord;
		JCoordinate newSpace;
		const JBoolean ok = CreateSpace(origSizes, minSizes, 0, reqSize, reqSize,
										&newSizes, &newSpace);
		assert( ok );

		itsSizes->SetElement(itsDragIndex,
							 itsSizes->GetElement(itsDragIndex) + reqSize);
		for (i=itsDragIndex+1; i<=compartmentCount; i++)
			{
			itsSizes->SetElement(i, newSizes.GetElement(i-itsDragIndex));
			}

		SetCompartmentSizes();
		}
}

/******************************************************************************
 PTBoundsChanged (virtual protected)

	Adjust the sizes of the compartments after our bounds changed.

 ******************************************************************************/

void
JPartition::PTBoundsChanged()
{
	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate delta = GetTotalSize() - kDragRegionSize * (compartmentCount-1);
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		delta -= GetCompartmentSize(i);
		}

	if (delta != 0)
		{
		JArray<JCoordinate> newSizes;
		if (delta > 0)
			{
			FillSpace(*itsSizes, itsElasticIndex, delta, &newSizes);
			}
		else if (delta < 0)
			{
			JCoordinate trueDelta;
			const JBoolean ok = CreateSpace(*itsSizes, *itsMinSizes, itsElasticIndex,
											-delta, -delta, &newSizes, &trueDelta);
			assert( ok );
			}
		*itsSizes = newSizes;
		SetCompartmentSizes();
		}
}

/******************************************************************************
 ReadGeometry

	Read in sizes, min sizes, elastic index and adjust ourselves.

	This only works if the number of compartments and the total size of
	the object are the same.

 ******************************************************************************/

void
JPartition::ReadGeometry
	(
	istream& input
	)
{
JIndex i;

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentGeometryDataVersion)
		{
		JIgnoreUntil(input, kGeometryDataEndDelimiter);
		return;
		}

	JSize count;
	JCoordinate totalSize;
	input >> count >> totalSize;
	if (count != GetCompartmentCount() || totalSize != GetTotalSize())
		{
		JIgnoreUntil(input, kGeometryDataEndDelimiter);
		return;
		}

	if (vers == 0)
		{
		JIndex elasticIndex;
		input >> elasticIndex;

		for (i=1; i<=count; i++)
			{
			JCoordinate minSize;
			input >> minSize;
			}
		}

	JArray<JCoordinate> sizes(count);
	for (i=1; i<=count; i++)
		{
		JCoordinate size;
		input >> size;
		sizes.AppendElement(size);;
		}
	SetCompartmentSizes(sizes);

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

	Write out sizes, min sizes, elastic index.

	We have to write out an ending delimiter that is never used anywhere
	else so we can at least ignore the data if we can't read the given
	version or there is some other error.

 ******************************************************************************/

void
JPartition::WriteGeometry
	(
	ostream& output
	)
	const
{
JIndex i;

	const JSize count = GetCompartmentCount();

	output << ' ' << kCurrentGeometryDataVersion;
	output << ' ' << count;
	output << ' ' << GetTotalSize();

	for (i=1; i<=count; i++)
		{
		output << ' ' << itsSizes->GetElement(i);
		}

	output << kGeometryDataEndDelimiter;
}
