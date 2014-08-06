/******************************************************************************
 SCResistor.h

	Interface for SCResistor class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCResistor
#define _H_SCResistor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCPassiveLinearComp.h"

class SCResistor : public SCPassiveLinearComp
{
public:

	SCResistor(SCCircuit* theCircuit, const JCharacter* name,
			   const JIndex posNode, const JIndex negNode, const JFloat value);
	SCResistor(const SCResistor& source, SCCircuit* theCircuit,
			   const JArray<JIndex>* nodeMap);

	// construct from private file

	SCResistor(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCResistor(istream& input, SCCircuit* theCircuit);

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
