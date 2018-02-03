/******************************************************************************
 SCIACSource.h

	Interface for SCIACSource class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCIACSource
#define _H_SCIACSource

#include "SCACSource.h"

class SCIACSource : public SCACSource
{
public:

	SCIACSource(SCCircuit* theCircuit, const JCharacter* name,
				const JIndex posNode, const JIndex negNode,
				const JFloat magnitude, const JFloat phase);
	SCIACSource(const SCIACSource& source, SCCircuit* theCircuit,
				const JArray<JIndex>* nodeMap);

	// construct from private file

	SCIACSource(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCIACSource(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCIACSource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;

private:

	// not allowed

	SCIACSource(const SCIACSource& source);
	const SCIACSource& operator=(const SCIACSource& source);
};

#endif
