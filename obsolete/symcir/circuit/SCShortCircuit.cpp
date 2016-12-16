/******************************************************************************
 SCShortCircuit.cpp

							The Short Circuit Class

	Instead of using Spice's hack of a zero magnitude voltage source as the
	controlling current for an IdepSource, we do it right and use an explicit
	short circuit.  Obviously, se don't provide a netlist constructor.

	BASE CLASS = SCLinearComp

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCShortCircuit.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCShortCircuit::SCShortCircuit
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode
	)
	:
	SCLinearComp(kShortCircuit, theCircuit, name, posNode, negNode)
{
}

SCShortCircuit::SCShortCircuit
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCLinearComp(input, vers, kShortCircuit, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCShortCircuit::~SCShortCircuit()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCShortCircuit::SCShortCircuit
	(
	const SCShortCircuit&	source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCLinearComp(source, theCircuit, nodeMap)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

SCComponent*
SCShortCircuit::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCShortCircuit* newComponent = new SCShortCircuit(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 PrintToNetlist

	We have to print ourselves as a zero magnitude AC voltage source.

 ******************************************************************************/

void
SCShortCircuit::PrintToNetlist
	(
	std::ostream& output
	)
	const
{
	SCLinearComp::PrintToNetlist(output);
	output << "\tAC\t0.0\t0.0";
}

/******************************************************************************
 GetAuxEquations

	Vpos - Vneg = 0

 ******************************************************************************/

void
SCShortCircuit::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCLinearComp::GetAuxEquations(lhsList, rhsList, varList);

	SCCircuit* theCircuit = GetCircuit();

	JString* lhs = new JString;
	assert( lhs != NULL );
	*lhs  = theCircuit->GetNodeName(GetPositiveNode());
	*lhs += " - ";
	*lhs += theCircuit->GetNodeName(GetNegativeNode());

	lhsList->Append(lhs);

	JString* rhs = new JString("0");
	assert( rhs != NULL );

	rhsList->Append(rhs);
}
