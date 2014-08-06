/******************************************************************************
 SCPassiveLinearComp.h

	Interface for SCPassiveLinearComp class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCPassiveLinearComp
#define _H_SCPassiveLinearComp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCLinearComp.h"

class SCPassiveLinearComp : public SCLinearComp
{
public:

	SCPassiveLinearComp(const SCComponentType type, SCCircuit* theCircuit,
						const JCharacter* name,
						const JIndex posNode, const JIndex negNode,
						const JFloat value);
	SCPassiveLinearComp(const SCPassiveLinearComp& source, SCCircuit* theCircuit,
						const JArray<JIndex>* nodeMap);

	// construct from private file

	SCPassiveLinearComp(istream& input, const JFileVersion vers, const SCComponentType type,
						SCCircuit* theCircuit);

	// construct from netlist

	SCPassiveLinearComp(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCPassiveLinearComp();

	JFloat	GetValue() const;
	void	SetValue(const JFloat value);

	virtual void	GetAuxEquations(JPtrArray<JString>* lhsList,
									JPtrArray<JString>* rhsList,
									JPtrArray<JString>* varList) const;
	virtual void	GetParserVariables(SCCircuitVarList* varList) const;
	virtual void	StreamOut(ostream& output) const;
	virtual void	PrintToNetlist(ostream& output) const;

	virtual JString	GetImpedanceSymbol() const = 0;
	virtual JString	GetAdmittanceSymbol() const = 0;

	// provides safe downcasting

	virtual SCPassiveLinearComp*		CastToSCPassiveLinearComp();
	virtual const SCPassiveLinearComp*	CastToSCPassiveLinearComp() const;

private:

	JFloat	itsValue;

private:

	// not allowed

	SCPassiveLinearComp(const SCPassiveLinearComp& source);
	const SCPassiveLinearComp& operator=(const SCPassiveLinearComp& source);
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JFloat
SCPassiveLinearComp::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

inline void
SCPassiveLinearComp::SetValue
	(
	const JFloat value
	)
{
	itsValue = value;
}

#endif
