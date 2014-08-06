/******************************************************************************
 SCIdepVSource.h

	Interface for SCIdepVSource class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCIdepVSource
#define _H_SCIdepVSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	SCIdepVSource(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	// construct from netlist

	SCIdepVSource(istream& input, SCCircuit* theCircuit);

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
