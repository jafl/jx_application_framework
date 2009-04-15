/******************************************************************************
 JXPartition.h

	Interface for the JXPartition class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPartition
#define _H_JXPartition

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>
#include <JPartition.h>

class JXPartition : public JXWidgetSet, public JPartition
{
public:

	virtual ~JXPartition();

	JXContainer*	GetCompartment(const JIndex index) const;
	JBoolean		GetCompartmentIndex(JXContainer* compartment,
										JIndex* index) const;

	JBoolean	InsertCompartment(const JIndex index, const JCoordinate size,
								  const JCoordinate minSize,
								  JXContainer** newCompartment);
	JBoolean	PrependCompartment(const JCoordinate size,
								   const JCoordinate minSize,
								   JXContainer** newCompartment);
	JBoolean	AppendCompartment(const JCoordinate size,
								  const JCoordinate minSize,
								  JXContainer** newCompartment);

	JBoolean	GetMinCompartmentSize(JXContainer* compartment, JCoordinate* minSize) const;
	JCoordinate	GetMinCompartmentSize(JXContainer* compartment) const;
	void		SetMinCompartmentSize(JXContainer* compartment, const JCoordinate minSize);

	JBoolean	GetElasticCompartment(JXContainer** compartment) const;
	void		SetElasticCompartment(JXContainer* compartment);

	void	DeleteCompartment(JXContainer* compartment);

protected:

	JXPartition(const JArray<JCoordinate>& sizes, const JIndex elasticIndex,
				const JArray<JCoordinate>& minSizes, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	void					CreateInitialCompartments();
	JPtrArray<JXContainer>*	GetCompartments();

	virtual void	CreateCompartmentObject(const JIndex index,
											const JCoordinate position,
											const JCoordinate size);
	virtual void	DeleteCompartmentObject(const JIndex index);

	virtual JXWidgetSet*	CreateCompartment(const JIndex index,
											  const JCoordinate position,
											  const JCoordinate size) = 0;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	JPtrArray<JXContainer>*	itsCompartments;

private:

	// not allowed

	JXPartition(const JXPartition& source);
	const JXPartition& operator=(const JXPartition& source);
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
	return itsCompartments->NthElement(index);
}

/******************************************************************************
 GetCompartmentIndex

	Returns kJTrue if the given JXContainer is a compartment.

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
JXPartition::PrependCompartment
	(
	const JCoordinate	size,
	const JCoordinate	minSize,
	JXContainer**		newCompartment
	)
{
	return InsertCompartment(1, size, minSize, newCompartment);
}

inline JBoolean
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
