/******************************************************************************
 SCVdepISource.h

	Interface for SCVdepISource class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVdepISource
#define _H_SCVdepISource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	SCVdepISource(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCVdepISource(istream& input, SCCircuit* theCircuit);

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
