/******************************************************************************
 SCVdepVSource.h

	Interface for SCVdepVSource class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCVdepVSource
#define _H_SCVdepVSource

#include "SCVdepSource.h"

class SCVdepVSource : public SCVdepSource
{
public:

	SCVdepVSource(SCCircuit* theCircuit, const JCharacter* name,
				  const JIndex posNode, const JIndex negNode,
				  const JIndex posDepNode, const JIndex negDepNode,
				  const JFloat coeff);
	SCVdepVSource(const SCVdepVSource& source, SCCircuit* theCircuit,
				  const JArray<JIndex>* nodeMap = NULL);

	// construct from private file

	SCVdepVSource(std::istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCVdepVSource(std::istream& input, SCCircuit* theCircuit);

	virtual ~SCVdepVSource();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap = NULL) const;

private:

	// not allowed

	SCVdepVSource(const SCVdepVSource& source);
	const SCVdepVSource& operator=(const SCVdepVSource& source);
};

#endif
