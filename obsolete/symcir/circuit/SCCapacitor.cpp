/******************************************************************************
 SCCapacitor.cpp

							The Capacitor Class

	BASE CLASS = SCPassiveLinearComp

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCCapacitor.h"
#include "SCCircuit.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCCapacitor::SCCapacitor
	(
	SCCircuit*			theCircuit,
	const JCharacter*	name,
	const JIndex		posNode,
	const JIndex		negNode,
	const JFloat		value
	)
	:
	SCPassiveLinearComp(kCapacitor, theCircuit, name, posNode, negNode, value)
{
}

SCCapacitor::SCCapacitor
	(
	istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
	:
	SCPassiveLinearComp(input, vers, kCapacitor, theCircuit)
{
}

// netlist parser

SCCapacitor::SCCapacitor
	(
	istream&	input,
	SCCircuit*	theCircuit
	)
	:
	SCPassiveLinearComp(input, kCapacitor, theCircuit)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCCapacitor::~SCCapacitor()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCCapacitor::SCCapacitor
	(
	const SCCapacitor&		source,
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
SCCapacitor::Copy
	(
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	const
{
	SCCapacitor* newComponent = new SCCapacitor(*this, theCircuit, nodeMap);
	assert( newComponent != NULL );
	return newComponent;
}

/******************************************************************************
 GetImpedanceSymbol

 ******************************************************************************/

JString
SCCapacitor::GetImpedanceSymbol()
	const
{
	JString Z = "1/(s * (";
	Z += GetValueSymbol();
	Z += "))";
	return Z;
}

/******************************************************************************
 GetAdmittanceSymbol

 ******************************************************************************/

JString
SCCapacitor::GetAdmittanceSymbol()
	const
{
	JString Y = "s * (";
	Y += GetValueSymbol();
	Y += ")";
	return Y;
}
