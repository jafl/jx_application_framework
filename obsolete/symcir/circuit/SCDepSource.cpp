/******************************************************************************
 SCDepSource.cpp

							The Dependent Source Class

	We don't implement PrintToNetlist because the coefficient value comes
	after the information describing the controlling signal.

	BASE CLASS = SCLinearComp

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCDepSource.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "scNetlistUtil.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCDepSource::SCDepSource
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode,
	const JFloat			coeff
	)
	:
	SCLinearComp(type, theCircuit, name, posNode, negNode)
{
	SCDepSourceX();
	itsCoeffValue = coeff;
}

SCDepSource::SCDepSource
	(
	std::istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, vers, type, theCircuit)
{
	SCDepSourceX();
	input >> *itsCoeffSymbol >> itsCoeffValue;
}

// netlist parser -- derived classes must parse the coefficient value

SCDepSource::SCDepSource
	(
	std::istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, type, theCircuit)
{
	SCDepSourceX();
}

// private

void
SCDepSource::SCDepSourceX()
{
	itsCoeffSymbol = new JString;
	assert( itsCoeffSymbol != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCDepSource::~SCDepSource()
{
	delete itsCoeffSymbol;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCDepSource::SCDepSource
	(
	const SCDepSource&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap,
	const JString&			coeffSymbol
	)
	:
	SCLinearComp(source, theCircuit, nodeMap)
{
	itsCoeffSymbol = new JString(coeffSymbol);
	assert( itsCoeffSymbol != NULL );

	itsCoeffValue = source.itsCoeffValue;
}

/******************************************************************************
 SetCoeffSymbol (protected)

 ******************************************************************************/

void
SCDepSource::SetCoeffSymbol
	(
	const JCharacter* symbol
	)
{
	*itsCoeffSymbol = symbol;
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCDepSource::StreamOut
	(
	std::ostream& output
	)
	const
{
	SCLinearComp::StreamOut(output);
	output << ' ' << *itsCoeffSymbol;
	output << ' ' << itsCoeffValue;
}

/******************************************************************************
 GetParserVariables

 ******************************************************************************/

void
SCDepSource::GetParserVariables
	(
	SCCircuitVarList* varList
	)
	const
{
	SCLinearComp::GetParserVariables(varList);
	varList->AddVariable(GetValueSymbol(), 0.0, kJFalse);
	varList->AddVariable(*itsCoeffSymbol, itsCoeffValue, kJTrue);
}

/******************************************************************************
 Cast to SCDepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCDepSource*
SCDepSource::CastToSCDepSource()
{
	return this;
}

const SCDepSource*
SCDepSource::CastToSCDepSource()
	const
{
	return this;
}
