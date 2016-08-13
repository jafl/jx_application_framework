/******************************************************************************
 SCVACSource.cpp

							The Voltage AC Source Class

	BASE CLASS = SCACSource

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCVACSource.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVACSource::SCVACSource
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JFloat		magnitude,
	const JFloat		phase
	)
	:
	SCACSource(kVACSource, theCircuit, name, posNode, negNode, magnitude, phase)
{
}

SCVACSource::SCVACSource
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCACSource(input, vers, kVACSource, theCircuit)
{
}

// netlist parser

SCVACSource::SCVACSource
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCACSource(input, kVACSource, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVACSource::~SCVACSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCVACSource::SCVACSource
	(
	const SCVACSource&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCACSource(source, theCircuit, nodeMap)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

SCComponent*
SCVACSource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCVACSource* newComponent = new SCVACSource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	Vpos - Vneg = <value>

 ******************************************************************************/

void
SCVACSource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCACSource::GetAuxEquations(lhsList, rhsList, varList);

	SCCircuit* theCircuit = GetCircuit();

	JString* lhs = new JString;
	assert( lhs != NULL );
	*lhs  = theCircuit->GetNodeName(GetPositiveNode());
	*lhs += " - ";
	*lhs += theCircuit->GetNodeName(GetNegativeNode());

	lhsList->Append(lhs);

	JString* rhs = new JString(GetValueSymbol());
	assert( rhs != NULL );

	rhsList->Append(rhs);
}
