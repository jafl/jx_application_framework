/******************************************************************************
 JPartition.cpp

	System independent base class for a set of compartments arranged
	horizontally or vertically.  We cannot store the actual compartment
	objects because they will be derived from a system dependent base class.

	*** The size of each compartment must be positive and at least equal
		to the corresponding minimum size *at all times*.  It is the client's
		responsibility to ensure that the enclosing window will never
		get too small.

	The default behavior when the entire partition is resized:
	(To change this, override PTBoundsChanged().)

	  * If all compartments are elastic, each one changes size by the
		same amount.
	  * If a single compartment is designated as elastic, only this
		one changes size.

	Derived classes must implement the following functions:

		GetTotalSize
			Return the total size of the partition.

		UpdateCompartmentSizes
			Adjust the size of each compartment object to match our values.

		CreateCompartmentObject
			Create a compartment object and insert it at the specified index.

		DeleteCompartmentObject
			Delete the specified compartment object.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996-2024 by John Lindal.

 ******************************************************************************/

#include "JPartition.h"
#include "jStreamUtil.h"
#include <numeric>
#include "jAssert.h"

const JCoordinate kDragRegionHalfSize = JPartition::kDragRegionSize/2;

const JFileVersion kCurrentGeometryDataVersion = 1;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

	// version 1: removed elastic index and min sizes

// JBroadcaster message types

const JUtf8Byte* JPartition::kBeginResizeCompartments = "BeginResizeCompartments::JPartition";
const JUtf8Byte* JPartition::kEndResizeCompartments   = "EndResizeCompartments::JPartition";

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
	:
	itsElasticIndex(elasticIndex),
	itsDragIndex(0)
{
	const JSize compartmentCount = sizes.GetItemCount();
	assert( compartmentCount == minSizes.GetItemCount() );
	assert( elasticIndex <= compartmentCount );

	itsSizes    = jnew JArray<JCoordinate>(sizes);
	itsMinSizes = jnew JArray<JCoordinate>(minSizes);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPartition::~JPartition()
{
	jdelete itsSizes;
	jdelete itsMinSizes;
}

/******************************************************************************
 SetCompartmentSize

	Adjusts the adjacent compartments to make space.

	Returns false if there isn't enough space available.

 ******************************************************************************/

bool
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
		newSizes.SetItem(index, reqSize);

		*itsSizes = newSizes;
		UpdateCompartmentSizes();
		return true;
	}
	else if (reqSize < origSize)
	{
		itsSizes->SetItem(index, reqSize);
		FillSpace(*itsSizes, itsElasticIndex, origSize - reqSize, &newSizes);
		*itsSizes = newSizes;
		UpdateCompartmentSizes();
		return true;
	}
	else
	{
		return reqSize == origSize;
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
	assert( GetCompartmentCount() == sizes.GetItemCount() );

	*itsSizes = sizes;
	SetElasticSize();
	UpdateCompartmentSizes();
}

/******************************************************************************
 GetMinTotalSize

 ******************************************************************************/

JCoordinate
JPartition::GetMinTotalSize()
	const
{
	return std::accumulate(begin(*itsMinSizes), end(*itsMinSizes),
						   kDragRegionSize * (GetCompartmentCount()-1));
}

/******************************************************************************
 FindCompartment

	If the point is between compartments i and i+1, the index returned is i.

 ******************************************************************************/

bool
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
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 InsertCompartment (protected)

	Adjusts the adjacent compartments to make space.

	Returns false if there isn't enough space available for at least minSize.

 ******************************************************************************/

bool
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
			position += newSizes.GetItem(i) + kDragRegionSize;
		}

		CreateCompartmentObject(index, position, trueSize);
		newSizes.InsertItemAtIndex(index, trueSize);
		itsMinSizes->InsertItemAtIndex(index, minSize);

		if (index <= itsElasticIndex)
		{
			itsElasticIndex++;
		}

		*itsSizes = newSizes;
		UpdateCompartmentSizes();
		return true;
	}
	else
	{
		return false;
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
	const JCoordinate fillSize   = itsSizes->GetItem(index) + kDragRegionSize;

	itsSizes->RemoveItem(index);
	itsMinSizes->RemoveItem(index);
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
	UpdateCompartmentSizes();
}

/******************************************************************************
 CreateSpace (private)

	Shrinks the compartments to create extra space.  If one of the compartments
	is elastic, this is shrunk first.

	Returns false if there isn't enough space available for at least minSize.

 ******************************************************************************/

bool
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
		return true;
	}

	// try shrinking the elastic compartment first

	if (elasticIndex > 0)
	{
		const JCoordinate size    = origSizes.GetItem(elasticIndex);
		const JCoordinate minSize = minSizes.GetItem(elasticIndex);

		const JCoordinate availSpace = size - minSize;
		if (availSpace >= reqSize)
		{
			*newSizes = origSizes;
			newSizes->SetItem(elasticIndex, size - reqSize);
			*newSpace = reqSize;
			return true;
		}
	}

	// distribute the load among all the other compartments
	// (shrink each to min, then expand to fill whatever space is left)

	const JSize compartmentCount = origSizes.GetItemCount();

	JCoordinate currSize = 0;
	JCoordinate minSize  = 0;
	for (JIndex i=1; i<=compartmentCount; i++)
	{
		currSize += origSizes.GetItem(i);
		minSize  += minSizes.GetItem(i);
	}

	if (currSize < minSize + minReqSize)
	{
		return false;
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

	return true;
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

		const JCoordinate size = newSizes->GetItem(elasticIndex);
		newSizes->SetItem(elasticIndex, size + fillSize);
	}
	else
	{
		// distribute the space among all the compartments

		const JSize compartmentCount = origSizes.GetItemCount();

		JCoordinate dsizeMax = fillSize/compartmentCount;
		if (fillSize % compartmentCount > 0)
		{
			dsizeMax++;
		}

		JCoordinate usedSize = 0;
		for (JIndex i=1; i<=compartmentCount; i++)
		{
			const JCoordinate size = newSizes->GetItem(i);
			JCoordinate newSize    = size + dsizeMax;
			if (usedSize + newSize - size > fillSize)
			{
				newSize = size + fillSize - usedSize;
			}
			usedSize += newSize - size;
			newSizes->SetItem(i, newSize);
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
		itsSizes->SetItem(itsElasticIndex, elasticSize);
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

	Broadcast(BeginResizeCompartments());
}

/******************************************************************************
 AdjustCompartmentsAfterDrag (protected)

	Shift space from one compartment to the other.

 ******************************************************************************/

void
JPartition::AdjustCompartmentsAfterDrag
	(
	const JCoordinate coord
	)
{
	itsSizes->SetItem(itsDragIndex,
						 coord - itsDragMin - kDragRegionHalfSize);
	itsSizes->SetItem(itsDragIndex + 1,
						 itsDragMax - coord - kDragRegionHalfSize - 1);

	UpdateCompartmentSizes();

	Broadcast(EndResizeCompartments());
}

/******************************************************************************
 PrepareToDragAll (protected)

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

	Broadcast(BeginResizeCompartments());
}

/******************************************************************************
 AdjustCompartmentsAfterDragAll (protected)

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
		JSize count = origSizes.GetItemCount();
		for (i=count; i>itsDragIndex; i--)
		{
			origSizes.RemoveItem(i);
			minSizes.RemoveItem(i);
		}

		JArray<JCoordinate> newSizes;
		const JCoordinate reqSize = itsStartCoord - coord;
		JCoordinate newSpace;
		const bool ok = CreateSpace(origSizes, minSizes, 0, reqSize, reqSize,
										&newSizes, &newSpace);
		assert( ok );

		for (i=1; i<=itsDragIndex; i++)
		{
			itsSizes->SetItem(i, newSizes.GetItem(i));
		}
		itsSizes->SetItem(itsDragIndex+1,
						  itsSizes->GetItem(itsDragIndex+1) + reqSize);

		UpdateCompartmentSizes();
	}

	// compress compartments after itsDragIndex

	else if (coord > itsStartCoord)
	{
		const JSize compartmentCount = GetCompartmentCount();

		JArray<JCoordinate> origSizes = *itsSizes;
		JArray<JCoordinate> minSizes  = *itsMinSizes;
		for (i=1; i<=itsDragIndex; i++)
		{
			origSizes.RemoveItem(1);
			minSizes.RemoveItem(1);
		}

		JArray<JCoordinate> newSizes;
		const JCoordinate reqSize = coord - itsStartCoord;
		JCoordinate newSpace;
		const bool ok = CreateSpace(origSizes, minSizes, 0, reqSize, reqSize,
										&newSizes, &newSpace);
		assert( ok );

		itsSizes->SetItem(itsDragIndex,
						  itsSizes->GetItem(itsDragIndex) + reqSize);
		for (i=itsDragIndex+1; i<=compartmentCount; i++)
		{
			itsSizes->SetItem(i, newSizes.GetItem(i-itsDragIndex));
		}

		UpdateCompartmentSizes();
	}

	Broadcast(EndResizeCompartments());
}

/******************************************************************************
 PTBoundsChanged (virtual protected)

	Adjust the sizes of the compartments after our bounds changed.

 ******************************************************************************/

void
JPartition::PTBoundsChanged()
{
	JCoordinate delta =
		GetTotalSize() -
		kDragRegionSize * (GetCompartmentCount()-1) -
		std::accumulate(begin(*itsSizes), end(*itsSizes), 0);

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
			const bool ok = CreateSpace(*itsSizes, *itsMinSizes, itsElasticIndex,
										-delta, -delta, &newSizes, &trueDelta);
			assert( ok );
		}
		*itsSizes = newSizes;
		UpdateCompartmentSizes();
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
	std::istream& input
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
	if (count != GetCompartmentCount())
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
		sizes.AppendItem(size);
	}

	if (!SaveGeometryForLater(sizes) &&
		!(itsElasticIndex == 0 && totalSize != GetTotalSize()))
	{
		SetCompartmentSizes(sizes);
	}

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
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentGeometryDataVersion;
	output << ' ' << GetCompartmentCount();
	output << ' ' << GetTotalSize();

	for (auto v : *itsSizes)
	{
		output << ' ' << v;
	}

	output << kGeometryDataEndDelimiter;
}
