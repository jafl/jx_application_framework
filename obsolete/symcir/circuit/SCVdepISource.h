/******************************************************************************
 SCVdepISource.h

	Interface for SCVdepISource class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVdepISource
#define _H_SCVdepISource

#include "SCVdepSource.h"

class SCVdepISource : public SCVdepSource
{
public:

	SCVdepISource(SCCircuit* theCircuit, const JCharacter* name,
				  const JIndex posNode, const JIndex negNode,
				  const JIndex posDepNode, const JIndex negDepNode,
				  const JFloat coeff);
	SCVdepISource(const SCVdepISource& source, SCCircuit* theCircuit,
				  const JArray<JIndex>* nodeMap);

	// construct from private file

	SCVdepISource(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCVdepISource(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCVdepISource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCVdepISource(const SCVdepISource& source);
	const SCVdepISource& operator=(const SCVdepISource& source);
};

#endif
