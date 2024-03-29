/******************************************************************************
 SCVACSource.h

	Interface for SCVACSource class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCVACSource
#define _H_SCVACSource

#include "SCACSource.h"

class SCVACSource : public SCACSource
{
public:

	SCVACSource(SCCircuit* theCircuit, const JCharacter* name,
				const JIndex posNode, const JIndex negNode,
				const JFloat magnitude, const JFloat phase);
	SCVACSource(const SCVACSource& source, SCCircuit* theCircuit,
				const JArray<JIndex>* nodeMap);

	// construct from private file

	SCVACSource(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCVACSource(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCVACSource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCVACSource(const SCVACSource& source);
	const SCVACSource& operator=(const SCVACSource& source);
};

#endif
