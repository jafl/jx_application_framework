/******************************************************************************
 SCInductor.cpp

							The Inductor Class

	BASE CLASS = SCPassiveLinearComp

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCInductor.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCInductor::SCInductor
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JFloat		value
	)
	:
	SCPassiveLinearComp(kInductor, theCircuit, name, posNode, negNode, value)
{
}

SCInductor::SCInductor
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCPassiveLinearComp(input, vers, kInductor, theCircuit)
{
}

// netlist parser

SCInductor::SCInductor
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCPassiveLinearComp(input, kInductor, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCInductor::~SCInductor()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCInductor::SCInductor
	(
	const SCInductor&		source,
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
SCInductor::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCInductor* newComponent = new SCInductor(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetImpedanceSymbol

 ******************************************************************************/

JString
SCInductor::GetImpedanceSymbol()
	const
{
	JString Z = "s * (";
	Z += GetValueSymbol();
	Z += ")";
	return Z;
}

/******************************************************************************
 GetAdmittanceSymbol

 ******************************************************************************/

JString
SCInductor::GetAdmittanceSymbol()
	const
{
	JString Y = "1/(s * (";
	Y += GetValueSymbol();
	Y += "))";
	return Y;
}
