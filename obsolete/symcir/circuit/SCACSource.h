/******************************************************************************
 SCACSource.h

	Interface for SCACSource class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCACSource
#define _H_SCACSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCLinearComp.h"

class SCACSource : public SCLinearComp
{
public:

	SCACSource(const SCComponentType type, SCCircuit* theCircuit, const JCharacter* name,
			   const JIndex posNode, const JIndex negNode,
			   const JFloat magnitude, const JFloat phase);
	SCACSource(const SCACSource& source, SCCircuit* theCircuit,
			   const JArray<JIndex>* nodeMap);

	// construct from private file

	SCACSource(istream& input, const JFileVersion vers, const SCComponentType type,
			   SCCircuit* theCircuit);

	// construct from netlist

	SCACSource(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCACSource();

	JFloat	GetMagnitude() const;
	void	SetMagnitude(const JFloat magnitude);

	JFloat	GetPhase() const;
	void	SetPhase(const JFloat phase);

	virtual void	StreamOut(ostream& output) const;
	virtual void	PrintToNetlist(ostream& output) const;
	virtual void	GetParserVariables(SCCircuitVarList* varList) const;

	// provides safe downcasting

	virtual SCACSource*			CastToSCACSource();
	virtual const SCACSource*	CastToSCACSource() const;

private:

	JFloat	itsMagnitude;
	JFloat	itsPhase;

private:

	// not allowed

	SCACSource(const SCACSource& source);
	const SCACSource& operator=(const SCACSource& source);
};

/******************************************************************************
 GetMagnitude

 ******************************************************************************/

inline JFloat
SCACSource::GetMagnitude()
	const
{
	return itsMagnitude;
}

/******************************************************************************
 SetMagnitude

 ******************************************************************************/

inline void
SCACSource::SetMagnitude
	(
	const JFloat magnitude
	)
{
	itsMagnitude = magnitude;
}

/******************************************************************************
 GetPhase

 ******************************************************************************/

inline JFloat
SCACSource::GetPhase()
	const
{
	return itsPhase;
}

/******************************************************************************
 SetPhase

 ******************************************************************************/

inline void
SCACSource::SetPhase
	(
	const JFloat phase
	)
{
	itsPhase = phase;
}

#endif
