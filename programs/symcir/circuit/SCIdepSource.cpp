/******************************************************************************
 SCIdepSource.cpp

						The Current Controlled Source Class

	We only allow dependence on a ShortCircuit because this is what Spice requires.

	BASE CLASS = SCDepSource

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCIdepSource.h"
#include "SCCircuit.h"
#include "scNetlistUtil.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCIdepSource::SCIdepSource
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode,
	const JIndex			depCompIndex,
	const JFloat			coeff
	)
	:
	SCDepSource(type, theCircuit, name, posNode, negNode, coeff)
{
#ifndef NDEBUG

	if (depCompIndex > 0)
		{
		SCComponent* depComp = theCircuit->GetComponent(depCompIndex);
		assert( depComp->GetType() == kShortCircuit );
		}

#endif

	SCIdepSourceX();
	itsDepCompIndex = depCompIndex;
}

SCIdepSource::SCIdepSource
	(
	istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCDepSource(input, vers, type, theCircuit)
{
	SCIdepSourceX();
	input >> itsDepCompIndex;
}

// netlist parser

SCIdepSource::SCIdepSource
	(
	istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCDepSource(input, type, theCircuit)
{
	SCIdepSourceX();

	const JString depCompName = JReadUntilws(input);
	const JBoolean found = theCircuit->FindComponent(depCompName, &itsDepCompIndex);
	if (!(found && (theCircuit->GetComponent(itsDepCompIndex))->GetType() == kShortCircuit))
		{
		cerr << "Unable to find controlling component \"" << depCompName;
		cerr << "\" for \"" << GetName() << '"' << endl;
		itsDepCompIndex = 0;
		}

	JFloat coeff;
	input >> coeff;
	const JString multStr = JReadUntilws(input);
	coeff *= GetSpiceMultiplier(multStr);
	SetCoeffValue(coeff);
}

// private

void
SCIdepSource::SCIdepSourceX()
{
	ListenTo((GetCircuit())->GetComponentList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCIdepSource::~SCIdepSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCIdepSource::SCIdepSource
	(
	const SCIdepSource&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap,
	const JString&			coeffSymbol
	)
	:
	SCDepSource(source, theCircuit, nodeMap, coeffSymbol)
{
	itsDepCompIndex = source.itsDepCompIndex;
	assert( (theCircuit->GetComponent(itsDepCompIndex))->GetType() == kShortCircuit );

	ListenTo((GetCircuit())->GetComponentList());
}

/******************************************************************************
 GetDepCompIndex

	Returns kJTrue if we are working correctly.

 ******************************************************************************/

JBoolean
SCIdepSource::GetDepCompIndex
	(
	JIndex* compIndex
	)
	const
{
	*compIndex = itsDepCompIndex;
	return JConvertToBoolean(itsDepCompIndex != 0);
}

/******************************************************************************
 SetDepCompIndex

 ******************************************************************************/

void
SCIdepSource::SetDepCompIndex
	(
	const JIndex compIndex
	)
{
#ifndef NDEBUG

	if (compIndex > 0)
		{
		SCComponent* depComp = (GetCircuit())->GetComponent(compIndex);
		assert( depComp->GetType() == kShortCircuit );
		}

#endif

	itsDepCompIndex = compIndex;
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCIdepSource::StreamOut
	(
	ostream& output
	)
	const
{
	SCDepSource::StreamOut(output);
	output << ' ' << itsDepCompIndex;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCIdepSource::PrintToNetlist
	(
	ostream& output
	)
	const
{
	SCDepSource::PrintToNetlist(output);
	output << '\t' << ((GetCircuit())->GetComponent(itsDepCompIndex))->GetName();
	output << '\t' << GetCoeffValue();
}

/******************************************************************************
 RequiresComponent

 ******************************************************************************/

JBoolean
SCIdepSource::RequiresComponent
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean(index == itsDepCompIndex ||
							 SCDepSource::RequiresComponent(index));
}

/******************************************************************************
 GetAuxEquations

	if we are on:  <value> = (<coeff>) * (<controlling current>)
	otherwise:     <value> = 0

 ******************************************************************************/

void
SCIdepSource::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	SCDepSource::GetAuxEquations(lhsList, rhsList, varList);

	SCCircuit* theCircuit = GetCircuit();

	JString* lhs = new JString(GetValueSymbol());
	assert( lhs != NULL );

	lhsList->Append(lhs);

	JString* rhs = new JString();
	assert( rhs != NULL );

	if (itsDepCompIndex != 0)
		{
		*rhs  = "(";
		*rhs += GetCoeffSymbol();
		*rhs += ") * (";
		SCLinearComp* depComp = (theCircuit->GetComponent(itsDepCompIndex))->CastToSCLinearComp();
		assert( depComp != NULL );
		*rhs += depComp->GetCurrentSymbol();
		*rhs += ")";
		}
	else
		{
		*rhs = "0";
		}

	rhsList->Append(rhs);
}

/******************************************************************************
 Receive (protected)

	Listen for changes that affect our controlling component.

 ******************************************************************************/

void
SCIdepSource::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JBroadcaster* compList = (GetCircuit())->GetComponentList();

	if (sender == compList && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast(const JOrderedSetT::ElementsInserted*, &message);
		assert( info != NULL );
		info->AdjustIndex(&itsDepCompIndex);
		}

	else if (sender == compList && message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast(const JOrderedSetT::ElementsRemoved*, &message);
		assert( info != NULL );
		info->AdjustIndex(&itsDepCompIndex);
		}

	else if (sender == compList && message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast(const JOrderedSetT::ElementMoved*, &message);
		assert( info != NULL );
		info->AdjustIndex(&itsDepCompIndex);
		}

	else if (sender == compList && message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast(const JOrderedSetT::ElementsSwapped*, &message);
		assert( info != NULL );
		info->AdjustIndex(&itsDepCompIndex);
		}

	SCDepSource::Receive(sender, message);
}

/******************************************************************************
 Cast to SCIdepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCIdepSource*
SCIdepSource::CastToSCIdepSource()
{
	return this;
}

const SCIdepSource*
SCIdepSource::CastToSCIdepSource()
	const
{
	return this;
}
