/******************************************************************************
 SCResistor.cpp

							The Resistor Class

	BASE CLASS = SCPassiveLinearComp

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCResistor.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCResistor::SCResistor
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JFloat		value
	)
	:
	SCPassiveLinearComp(kResistor, theCircuit, name, posNode, negNode, value)
{
}

SCResistor::SCResistor
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCPassiveLinearComp(input, vers, kResistor, theCircuit)
{
}

// netlist parser

SCResistor::SCResistor
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCPassiveLinearComp(input, kResistor, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCResistor::~SCResistor()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCResistor::SCResistor
	(
	const SCResistor&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCPassiveLinearComp(source, theCircuit, nodeMap)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

SCComponent*
SCResistor::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCResistor* newComponent = new SCResistor(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetImpedanceSymbol

 ******************************************************************************/

JString
SCResistor::GetImpedanceSymbol()
	const
{
	return GetValueSymbol();
}

/******************************************************************************
 GetAdmittanceSymbol

 ******************************************************************************/

JString
SCResistor::GetAdmittanceSymbol()
	const
{
	JString Y = "1/(";
	Y += GetValueSymbol();
	Y += ")";
	return Y;
}
