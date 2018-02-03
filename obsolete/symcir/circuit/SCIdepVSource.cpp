/******************************************************************************
 SCIdepVSource.cpp

					The Current Controlled Voltage Source Class

	BASE CLASS = SCIdepSource

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "SCIdepVSource.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kCoeffPrefix = "Zdep";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCIdepVSource::SCIdepVSource
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JIndex		depCompIndex,
	const JFloat		coeff
	)
	:
	SCIdepSource(kIdepVSource, theCircuit, name, posNode, negNode,
			   depCompIndex, coeff)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

SCIdepVSource::SCIdepVSource
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCIdepSource(input, vers, kIdepVSource, theCircuit)
{
}

// netlist parser

SCIdepVSource::SCIdepVSource
	(
	std::istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCIdepSource(input, kIdepVSource, theCircuit)
{
	const JString coeffSymbol = theCircuit->GetUniqueCoeffName(kCoeffPrefix);
	SetCoeffSymbol(coeffSymbol);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCIdepVSource::~SCIdepVSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCIdepVSource::SCIdepVSource
	(
	const SCIdepVSource&	source,
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
SCIdepVSource::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCIdepVSource* newComponent = new SCIdepVSource(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetAuxEquations

	Vpos - Vneg = <value>

 ******************************************************************************/

void
SCIdepVSource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCIdepSource::GetAuxEquations(lhsList, rhsList, varList);

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
