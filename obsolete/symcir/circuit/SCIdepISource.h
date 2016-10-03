/******************************************************************************
 SCIdepISource.h

	Interface for SCIdepISource class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCIdepISource
#define _H_SCIdepISource

#include "SCIdepSource.h"

class SCIdepISource : public SCIdepSource
{
public:

	SCIdepISource(SCCircuit* theCircuit, const JCharacter* name,
				  const JIndex posNode, const JIndex negNode,
				  const JIndex depCompIndex, const JFloat coeff);
	SCIdepISource(const SCIdepISource& source, SCCircuit* theCircuit,
				  const JArray<JIndex>* nodeMap);

	// construct from private file

	SCIdepISource(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCIdepISource(istream& input, SCCircuit* theCircuit);

	virtual ~SCIdepISource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCIdepISource(const SCIdepISource& source);
	const SCIdepISource& operator=(const SCIdepISource& source);
};

#endif
