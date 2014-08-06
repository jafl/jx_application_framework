/******************************************************************************
 SCVdepVSource.h

	Interface for SCVdepVSource class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVdepVSource
#define _H_SCVdepVSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	SCVdepVSource(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCVdepVSource(istream& input, SCCircuit* theCircuit);

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
