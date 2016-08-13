/******************************************************************************
 SCIACSource.cpp

							The Current AC Source Class

	BASE CLASS = SCACSource

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCIACSource.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCIACSource::SCIACSource
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JFloat		magnitude,
	const JFloat		phase
	)
	:
	SCACSource(kIACSource, theCircuit, name, posNode, negNode, magnitude, phase)
{
}

SCIACSource::SCIACSource
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCACSource(input, vers, kIACSource, theCircuit)
{
}

// netlist parser

SCIACSource::SCIACSource
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCACSource(input, kIACSource, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCIACSource::~SCIACSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCIACSource::SCIACSource
	(
	const SCIACSource&		source,
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
SCIACSource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCIACSource* newComponent = new SCIACSource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	<value> = <current>

 ******************************************************************************/

void
SCIACSource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCACSource::GetAuxEquations(lhsList, rhsList, varList);

	JString* lhs = new JString(GetValueSymbol());
	assert( lhs != NULL );
	lhsList->Append(lhs);

	JString* rhs = new JString(GetCurrentSymbol());
	assert( rhs != NULL );
	rhsList->Append(rhs);

	JString* var = new JString(GetValueSymbol());
	assert( var != NULL );
	varList->Append(var);
}
