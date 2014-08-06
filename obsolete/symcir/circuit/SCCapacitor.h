/******************************************************************************
 SCCapacitor.h

	Interface for SCCapacitor class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCCapacitor
#define _H_SCCapacitor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCPassiveLinearComp.h"

class SCCapacitor : public SCPassiveLinearComp
{
public:

	SCCapacitor(SCCircuit* theCircuit, const JCharacter* name,
				const JIndex posNode, const JIndex negNode, const JFloat value);
	SCCapacitor(const SCCapacitor& source, SCCircuit* theCircuit,
				const JArray<JIndex>* nodeMap);

	// construct from private file

	SCCapacitor(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCCapacitor(istream& input, SCCircuit* theCircuit);

	virtual ~SCCapacitor();

	virtual JString			GetImpedanceSymbol() const;
	virtual JString			GetAdmittanceSymbol() const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCCapacitor(const SCCapacitor& source);
	const SCCapacitor& operator=(const SCCapacitor& source);
};

#endif
