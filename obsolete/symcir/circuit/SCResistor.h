/******************************************************************************
 SCResistor.h

	Interface for SCResistor class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCResistor
#define _H_SCResistor

#include "SCPassiveLinearComp.h"

class SCResistor : public SCPassiveLinearComp
{
public:

	SCResistor(SCCircuit* theCircuit, const JCharacter* name,
			   const JIndex posNode, const JIndex negNode, const JFloat value);
	SCResistor(const SCResistor& source, SCCircuit* theCircuit,
			   const JArray<JIndex>* nodeMap);

	// construct from private file

	SCResistor(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCResistor(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCResistor();

	virtual JString			GetImpedanceSymbol() const;
	virtual JString			GetAdmittanceSymbol() const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCResistor(const SCResistor& source);
	const SCResistor& operator=(const SCResistor& source);
};

#endif
