/******************************************************************************
 JPartition.h

	Interface for the JPartition class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPartition
#define _H_JPartition

#include "JArray.h"

class JPartition
{
public:

	// should be odd so user can find midpoint
	static const JCoordinate kDragRegionSize = 5;

public:

	virtual ~JPartition();

	JSize	GetCompartmentCount() const;

	JCoordinate	GetCompartmentSize(const JIndex index) const;
	bool	SetCompartmentSize(const JIndex index, const JCoordinate reqSize);

	const JArray<JCoordinate>&	GetCompartmentSizes() const;
	void						SetCompartmentSizes(const JArray<JCoordinate>& sizes);

	JCoordinate	GetMinTotalSize() const;
	JCoordinate	GetMinCompartmentSize(const JIndex index) const;
	void		SetMinCompartmentSize(const JIndex index, const JCoordinate minSize);

	const JArray<JCoordinate>&	GetMinCompartmentSizes() const;
	void						SetMinCompartmentSizes(const JArray<JCoordinate>& sizes);

	bool	GetElasticIndex(JIndex* index) const;
	void		SetElasticIndex(const JIndex index);

	bool	FindCompartment(const JCoordinate coord, JIndex* index) const;
	void		DeleteCompartment(const JIndex index);

	void		ReadGeometry(std::istream& input);
	void		WriteGeometry(std::ostream& output) const;

protected:

	JPartition(const JArray<JCoordinate>& sizes, const JIndex elasticIndex,
			   const JArray<JCoordinate>& minSizes);

	bool	InsertCompartment(const JIndex index, const JCoordinate size,
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
	virtual void		UpdateCompartmentSizes() = 0;
	virtual bool	SaveGeometryForLater(const JArray<JCoordinate>& sizes) = 0;

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

	bool	CreateSpace(const JArray<JCoordinate>& origSizes,
							const JArray<JCoordinate>& minSizes,
							const JIndex elasticIndex,
							const JCoordinate reqSize, const JCoordinate minSize,
							JArray<JCoordinate>* newSizes,
							JCoordinate* newSpace) const;
	void		FillSpace(const JArray<JCoordinate>& origSizes,
						  const JIndex elasticIndex, const JCoordinate fillSize,
						  JArray<JCoordinate>* newSizes) const;

	// not allowed

	JPartition(const JPartition&) = delete;
	JPartition& operator=(const JPartition&) = delete;
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

inline bool
JPartition::GetElasticIndex
	(
	JIndex* index
	)
	const
{
	*index = itsElasticIndex;
	return itsElasticIndex > 0;
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

inline const JArray<JCoordinate>&
JPartition::GetMinCompartmentSizes()
	const
{
	return *itsMinSizes;
}

inline void
JPartition::SetMinCompartmentSizes
	(
	const JArray<JCoordinate>& sizes
	)
{
	*itsMinSizes = sizes;
}

#endif
