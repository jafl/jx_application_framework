/******************************************************************************
 SCVdepISource.cpp

					The Voltage Controlled Current Source Class

	BASE CLASS = SCVdepSource

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "SCVdepISource.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kCoeffPrefix = "Ydep";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVdepISource::SCVdepISource
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JIndex		posDepNode,
	const JIndex		negDepNode,
	const JFloat		coeff
	)
	:
	SCVdepSource(kVdepISource, theCircuit, name, posNode, negNode,
				 posDepNode, negDepNode, coeff)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

SCVdepISource::SCVdepISource
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCVdepSource(input, vers, kVdepISource, theCircuit)
{
}

// netlist parser

SCVdepISource::SCVdepISource
	(
	std::istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCVdepSource(input, kVdepISource, theCircuit)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVdepISource::~SCVdepISource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCVdepISource::SCVdepISource
	(
	const SCVdepISource&	source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCVdepSource(source, theCircuit, nodeMap,
				 theCircuit->GetUniqueCoeffName(kCoeffPrefix))
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

SCComponent*
SCVdepISource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCVdepISource* newComponent = new SCVdepISource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	<value> = <current>

 ******************************************************************************/

void
SCVdepISource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCVdepSource::GetAuxEquations(lhsList, rhsList, varList);

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
