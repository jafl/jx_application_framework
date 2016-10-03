/******************************************************************************
 SCVdepVSource.cpp

					The Voltage Controlled Voltage Source Class

	BASE CLASS = SCVdepSource

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCVdepVSource.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kCoeffPrefix = "Adep";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVdepVSource::SCVdepVSource
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
	SCVdepSource(kVdepVSource, theCircuit, name, posNode, negNode,
				 posDepNode, negDepNode, coeff)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

SCVdepVSource::SCVdepVSource
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCVdepSource(input, vers, kVdepVSource, theCircuit)
{
}

// netlist parser

SCVdepVSource::SCVdepVSource
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCVdepSource(input, kVdepVSource, theCircuit)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVdepVSource::~SCVdepVSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCVdepVSource::SCVdepVSource
	(
	const SCVdepVSource&	source,
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
SCVdepVSource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCVdepVSource* newComponent = new SCVdepVSource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	Vpos - Vneg = <value>

 ******************************************************************************/

void
SCVdepVSource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCVdepSource::GetAuxEquations(lhsList, rhsList, varList);

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

	JString* var = new JString(GetValueSymbol());
	assert( var != NULL );

	varList->Append(var);
}
