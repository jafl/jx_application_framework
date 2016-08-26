/******************************************************************************
 SCInductor.h

	Interface for SCInductor class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCInductor
#define _H_SCInductor

#include "SCPassiveLinearComp.h"

class SCInductor : public SCPassiveLinearComp
{
public:

	SCInductor(SCCircuit* theCircuit, const JCharacter* name,
			   const JIndex posNode, const JIndex negNode, const JFloat value);
	SCInductor(const SCInductor& source, SCCircuit* theCircuit,
			   const JArray<JIndex>* nodeMap);

	// construct from private file

	SCInductor(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCInductor(istream& input, SCCircuit* theCircuit);

	virtual ~SCInductor();

	virtual JString			GetImpedanceSymbol() const;
	virtual JString			GetAdmittanceSymbol() const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCInductor(const SCInductor& source);
	const SCInductor& operator=(const SCInductor& source);
};

#endif
