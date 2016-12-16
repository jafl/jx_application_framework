/******************************************************************************
 SCIdepISource.cpp

					The Current Controlled Current Source Class

	BASE CLASS = SCIdepSource

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCIdepISource.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kCoeffPrefix = "Bdep";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCIdepISource::SCIdepISource
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JIndex		depCompIndex,
	const JFloat		coeff
	)
	:
	SCIdepSource(kIdepISource, theCircuit, name, posNode, negNode,
			   depCompIndex, coeff)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

SCIdepISource::SCIdepISource
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCIdepSource(input, vers, kIdepISource, theCircuit)
{
}

// netlist parser

SCIdepISource::SCIdepISource
	(
	std::istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCIdepSource(input, kIdepISource, theCircuit)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCIdepISource::~SCIdepISource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCIdepISource::SCIdepISource
	(
	const SCIdepISource&	source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCIdepSource(source, theCircuit, nodeMap,
				 theCircuit->GetUniqueCoeffName(kCoeffPrefix))
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

SCComponent*
SCIdepISource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCIdepISource* newComponent = new SCIdepISource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	<value> = <current>

 ******************************************************************************/

void
SCIdepISource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCIdepSource::GetAuxEquations(lhsList, rhsList, varList);

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
