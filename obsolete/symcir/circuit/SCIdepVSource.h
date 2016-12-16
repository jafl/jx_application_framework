/******************************************************************************
 SCIdepVSource.h

	Interface for SCIdepVSource class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCIdepVSource
#define _H_SCIdepVSource

#include "SCIdepSource.h"

class SCIdepVSource : public SCIdepSource
{
public:

	SCIdepVSource(SCCircuit* theCircuit, const JCharacter* name,
				  const JIndex posNode, const JIndex negNode,
				  const JIndex depCompIndex, const JFloat coeff);
	SCIdepVSource(const SCIdepVSource& source, SCCircuit* theCircuit,
				  const JArray<JIndex>* nodeMap);

	// construct from private file

	SCIdepVSource(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCIdepVSource(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCIdepVSource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCIdepVSource(const SCIdepVSource& source);
	const SCIdepVSource& operator=(const SCIdepVSource& source);
};

#endif
