/******************************************************************************
 JXPartition.h

	Interface for the JXPartition class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPartition
#define _H_JXPartition

#include "jx-af/jx/JXWidgetSet.h"
#include <jx-af/jcore/JPartition.h>

class JXPartition : public JXWidgetSet, public JPartition
{
public:

	virtual ~JXPartition();

	JXContainer*	GetCompartment(const JIndex index) const;
	bool			GetCompartmentIndex(JXContainer* compartment,
										JIndex* index) const;

	bool	InsertCompartment(const JIndex index, const JCoordinate size,
							  const JCoordinate minSize,
							  JXContainer** newCompartment);
	bool	PrependCompartment(const JCoordinate size,
							   const JCoordinate minSize,
							   JXContainer** newCompartment);
	bool	AppendCompartment(const JCoordinate size,
							  const JCoordinate minSize,
							  JXContainer** newCompartment);

	bool		GetMinCompartmentSize(JXContainer* compartment, JCoordinate* minSize) const;
	JCoordinate	GetMinCompartmentSize(JXContainer* compartment) const;
	void		SetMinCompartmentSize(JXContainer* compartment, const JCoordinate minSize);

	bool	GetElasticCompartment(JXContainer** compartment) const;
	void	SetElasticCompartment(JXContainer* compartment);

	void	DeleteCompartment(JXContainer* compartment);

protected:

	JXPartition(const JArray<JCoordinate>& sizes, const JIndex elasticIndex,
				const JArray<JCoordinate>& minSizes, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	void					CreateInitialCompartments();
	JPtrArray<JXContainer>*	GetCompartments();

	void	CreateCompartmentObject(const JIndex index,
											const JCoordinate position,
											const JCoordinate size) override;
	void	DeleteCompartmentObject(const JIndex index) override;

	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size) = 0;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

	void	RestoreGeometry(const JArray<JCoordinate>& sizes);

private:

	JPtrArray<JXContainer>*	itsCompartments;
};


/******************************************************************************
 GetCompartment

 ******************************************************************************/

inline JXContainer*
JXPartition::GetCompartment
	(
	const JIndex index
	)
	const
{
	return itsCompartments->GetElement(index);
}

/******************************************************************************
 GetCompartmentIndex

	Returns true if the given JXContainer is a compartment.

 ******************************************************************************/

inline bool
JXPartition::GetCompartmentIndex
	(
	JXContainer*	compartment,
	JIndex*			index
	)
	const
{
	return itsCompartments->Find(compartment, index);
}

/******************************************************************************
 GetCompartments (protected)

 ******************************************************************************/

inline JPtrArray<JXContainer>*
JXPartition::GetCompartments()
{
	return itsCompartments;
}

/******************************************************************************
 Add compartment

 ******************************************************************************/

inline bool
JXPartition::PrependCompartment
	(
	const JCoordinate	size,
	const JCoordinate	minSize,
	JXContainer**		newCompartment
	)
{
	return InsertCompartment(1, size, minSize, newCompartment);
}

inline bool
JXPartition::AppendCompartment
	(
	const JCoordinate	size,
	const JCoordinate	minSize,
	JXContainer**		newCompartment
	)
{
	return InsertCompartment(GetCompartmentCount()+1,
							 size, minSize, newCompartment);
}

#endif
