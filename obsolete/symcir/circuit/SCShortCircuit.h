/******************************************************************************
 SCShortCircuit.h

	Interface for SCShortCircuit class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCShortCircuit
#define _H_SCShortCircuit

#include "SCLinearComp.h"

class SCShortCircuit : public SCLinearComp
{
public:

	SCShortCircuit(SCCircuit* theCircuit, const JCharacter* name,
				   const JIndex posNode, const JIndex negNode);
	SCShortCircuit(const SCShortCircuit& source, SCCircuit* theCircuit,
				   const JArray<JIndex>* nodeMap);

	// construct from private file

	SCShortCircuit(istream& input, const JFileVersion vers, SCCircuit* theCircuit);

	virtual ~SCShortCircuit();

	virtual void			GetAuxEquations(JPtrArray<JString>* lhsList,
											JPtrArray<JString>* rhsList,
											JPtrArray<JString>* varList) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit, const JArray<JIndex>* nodeMap) const;
	virtual void			PrintToNetlist(ostream& output) const;

private:

	// not allowed

	SCShortCircuit(const SCShortCircuit& source);
	const SCShortCircuit& operator=(const SCShortCircuit& source);
};

#endif
