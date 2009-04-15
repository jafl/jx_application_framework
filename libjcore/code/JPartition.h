/******************************************************************************
 JPartition.h

	Interface for the JPartition class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPartition
#define _H_JPartition

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>

class JPartition
{
public:

	enum
	{
		kDragRegionSize = 5		// should be odd so user can find midpoint
	};

public:

	virtual ~JPartition();

	JSize	GetCompartmentCount() const;

	JCoordinate	GetCompartmentSize(const JIndex index) const;
	JBoolean	SetCompartmentSize(const JIndex index, const JCoordinate reqSize);

	const JArray<JCoordinate>&	GetCompartmentSizes() const;
	void						SetCompartmentSizes(const JArray<JCoordinate>& sizes);

	JCoordinate	GetMinTotalSize() const;
	JCoordinate	GetMinCompartmentSize(const JIndex index) const;
	void		SetMinCompartmentSize(const JIndex index, const JCoordinate minSize);

	JBoolean	GetElasticIndex(JIndex* index) const;
	void		SetElasticIndex(const JIndex index);

	JBoolean	FindCompartment(const JCoordinate coord, JIndex* index) const;
	void		DeleteCompartment(const JIndex index);

	void		ReadGeometry(istream& input);
	void		WriteGeometry(ostream& output) const;

protected:

	JPartition(const JArray<JCoordinate>& sizes, const JIndex elasticIndex,
			   const JArray<JCoordinate>& minSizes);

	JBoolean	InsertCompartment(const JIndex index, const JCoordinate size,
								  const JCoordinate minSize);

	void	SetElasticSize();

	void	PrepareToDrag(const JCoordinate coord,
						  JCoordinate* minDragCoord, JCoordinate* maxDragCoord);
	void	AdjustCompartmentsAfterDrag(const JCoordinate coord);

	void	PrepareToDragAll(const JCoordinate coord,
							 JCoordinate* minDragCoord, JCoordinate* maxDragCoord);
	void	AdjustCompartmentsAfterDragAll(const JCoordinate coord);

	virtual void	PTBoundsChanged();	// derived classes can override

	virtual JCoordinate	GetTotalSize() const = 0;
	virtual void		SetCompartmentSizes() = 0;

	virtual void		CreateCompartmentObject(const JIndex index,
												const JCoordinate position,
												const JCoordinate size) = 0;
	virtual void		DeleteCompartmentObject(const JIndex index) = 0;

private:

	JArray<JCoordinate>*	itsSizes;
	JIndex					itsElasticIndex;	// 0 => all expand equally
	JArray<JCoordinate>*	itsMinSizes;

	// used during drag

	JIndex		itsDragIndex;
	JCoordinate	itsStartCoord;
	JCoordinate	itsDragMin, itsDragMax;

private:

	JBoolean	CreateSpace(const JArray<JCoordinate>& origSizes,
							const JArray<JCoordinate>& minSizes,
							const JIndex elasticIndex,
							const JCoordinate reqSize, const JCoordinate minSize,
							JArray<JCoordinate>* newSizes,
							JCoordinate* newSpace) const;
	void		FillSpace(const JArray<JCoordinate>& origSizes,
						  const JIndex elasticIndex, const JCoordinate fillSize,
						  JArray<JCoordinate>* newSizes) const;

	// not allowed

	JPartition(const JPartition& source);
	const JPartition& operator=(const JPartition& source);
};

/******************************************************************************
 GetCompartmentCount

 ******************************************************************************/

inline JSize
JPartition::GetCompartmentCount()
	const
{
	return itsSizes->GetElementCount();
}

/******************************************************************************
 GetElasticIndex

 ******************************************************************************/

inline JBoolean
JPartition::GetElasticIndex
	(
	JIndex* index
	)
	const
{
	*index = itsElasticIndex;
	return JConvertToBoolean( itsElasticIndex > 0 );
}

/******************************************************************************
 SetElasticIndex

	Use index=0 to make all of the compartments shrink and expand by
	the same amount.

 ******************************************************************************/

inline void
JPartition::SetElasticIndex
	(
	const JIndex index
	)
{
	itsElasticIndex = index;
}

/******************************************************************************
 Get comparment size

 ******************************************************************************/

inline JCoordinate
JPartition::GetCompartmentSize
	(
	const JIndex index
	)
	const
{
	return itsSizes->GetElement(index);
}

inline const JArray<JCoordinate>&
JPartition::GetCompartmentSizes()
	const
{
	return *itsSizes;
}

/******************************************************************************
 Min comparment size

 ******************************************************************************/

inline JCoordinate
JPartition::GetMinCompartmentSize
	(
	const JIndex index
	)
	const
{
	return itsMinSizes->GetElement(index);
}

inline void
JPartition::SetMinCompartmentSize
	(
	const JIndex		index,
	const JCoordinate	minSize
	)
{
	itsMinSizes->SetElement(index, minSize);
}

#endif
