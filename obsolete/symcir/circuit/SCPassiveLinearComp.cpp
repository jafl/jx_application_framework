/******************************************************************************
 SCPassiveLinearComp.cpp

	BASE CLASS = SCLinearComp

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCPassiveLinearComp.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"
#include "scNetlistUtil.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCPassiveLinearComp::SCPassiveLinearComp
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode,
	const JFloat			value
	)
	:
	SCLinearComp(type, theCircuit, name, posNode, negNode)
{
	itsValue = value;
}

SCPassiveLinearComp::SCPassiveLinearComp
	(
	istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, vers, type, theCircuit)
{
	input >> itsValue;
}

// netlist parser

SCPassiveLinearComp::SCPassiveLinearComp
	(
	istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, type, theCircuit)
{
	input >> itsValue;
	const JString multStr = JReadUntilws(input);
	itsValue *= GetSpiceMultiplier(multStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCPassiveLinearComp::~SCPassiveLinearComp()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCPassiveLinearComp::SCPassiveLinearComp
	(
	const SCPassiveLinearComp&		source,
	SCCircuit*						theCircuit,
	const JArray<JIndex>*			nodeMap
	)
	:
	SCLinearComp(source, theCircuit, nodeMap)
{
	itsValue = source.itsValue;
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCPassiveLinearComp::StreamOut
	(
	ostream& output
	)
	const
{
	SCLinearComp::StreamOut(output);
	output << ' ' << itsValue;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCPassiveLinearComp::PrintToNetlist
	(
	ostream& output
	)
	const
{
	SCLinearComp::PrintToNetlist(output);
	output << '\t' << itsValue;
}

/******************************************************************************
 GetAuxEquations

	<current> = (Vpos - Vneg) * (<admittance>)
		or
	(<current>) * (<impedance>) = Vpos - Vneg

	We choose the one that doesn't require division so that the component's
	value can safely be set to zero.

 ******************************************************************************/

void
SCPassiveLinearComp::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCLinearComp::GetAuxEquations(lhsList, rhsList, varList);

	SCCircuit* theCircuit = GetCircuit();

	JString* lhs = new JString(GetCurrentSymbol());
	assert( lhs != NULL );

	lhsList->Append(lhs);

	JString* rhs = new JString();
	assert( rhs != NULL );
	*rhs += theCircuit->GetNodeName(GetPositiveNode());
	*rhs += " - ";
	*rhs += theCircuit->GetNodeName(GetNegativeNode());

	rhsList->Append(rhs);

	const JString Z = GetImpedanceSymbol();
	JIndex i;
	if (!Z.LocateSubstring("/", &i))
		{
		lhs->Prepend("(");
		*lhs += ") * (";
		*lhs += Z;
		*lhs += ")";
		}
	else
		{
		const JString Y = GetAdmittanceSymbol();
		assert( !Y.LocateSubstring("/", &i) );
		rhs->Prepend("(");
		*rhs += ") * (";
		*rhs += Y;
		*rhs += ")";
		}
}

/******************************************************************************
 GetParserVariables

 ******************************************************************************/

void
SCPassiveLinearComp::GetParserVariables
	(
	SCCircuitVarList* varList
	)
	const
{
	SCLinearComp::GetParserVariables(varList);
	varList->AddVariable(GetValueSymbol(), itsValue, kJTrue);
}

/******************************************************************************
 Cast to SCPassiveLinearComp*

	Not inline because they are virtual

 ******************************************************************************/

SCPassiveLinearComp*
SCPassiveLinearComp::CastToSCPassiveLinearComp()
{
	return this;
}

const SCPassiveLinearComp*
SCPassiveLinearComp::CastToSCPassiveLinearComp()
	const
{
	return this;
}
