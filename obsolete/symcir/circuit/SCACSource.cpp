/******************************************************************************
 SCACSource.cpp

							The Independent AC Source Class

	BASE CLASS = SCLinearComp

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "SCACSource.h"
#include "SCCircuitVarList.h"
#include "scNetlistUtil.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCACSource::SCACSource
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode,
	const JFloat			magnitude,
	const JFloat			phase
	)
	:
	SCLinearComp(type, theCircuit, name, posNode, negNode)
{
	itsMagnitude = magnitude;
	itsPhase     = phase;
}

SCACSource::SCACSource
	(
	std::istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, vers, type, theCircuit)
{
	input >> itsMagnitude >> itsPhase;
}

// netlist parser

SCACSource::SCACSource
	(
	std::istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCLinearComp(input, type, theCircuit)
{
	const JString subType = JReadUntilws(input);

	input >> itsMagnitude;
	const JString multStr = JReadUntilws(input);
	itsMagnitude *= GetSpiceMultiplier(multStr);

	if (subType == "AC")
		{
		input >> itsPhase;
		}
	else
		{
		itsMagnitude = 0.0;
		std::cerr << subType << " source \"" << GetName() << "\" turned off" << std::endl;
		if (subType != "DC")
			{
			JIgnoreLine(input);
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCACSource::~SCACSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCACSource::SCACSource
	(
	const SCACSource&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCLinearComp(source, theCircuit, nodeMap)
{
	itsMagnitude = source.itsMagnitude;
	itsPhase     = source.itsPhase;
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCACSource::StreamOut
	(
	std::ostream& output
	)
	const
{
	SCLinearComp::StreamOut(output);
	output << ' ' << itsMagnitude;
	output << ' ' << itsPhase;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCACSource::PrintToNetlist
	(
	std::ostream& output
	)
	const
{
	SCLinearComp::PrintToNetlist(output);
	output << "\tAC";
	output << '\t' << itsMagnitude;
	output << '\t' << itsPhase;
}

/******************************************************************************
 GetParserVariables

 ******************************************************************************/

void
SCACSource::GetParserVariables
	(
	SCCircuitVarList* varList
	)
	const
{
	SCLinearComp::GetParserVariables(varList);
	varList->AddVariable(GetValueSymbol(), 0.0, kJFalse);
}

/******************************************************************************
 Cast to SCACSource*

	Not inline because they are virtual

 ******************************************************************************/

SCACSource*
SCACSource::CastToSCACSource()
{
	return this;
}

const SCACSource*
SCACSource::CastToSCACSource()
	const
{
	return this;
}
