/******************************************************************************
 SCVdepSource.cpp

						The Voltage Controlled Source Class

	BASE CLASS = SCDepSource

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCVdepSource.h"
#include "SCCircuit.h"
#include "scNetlistUtil.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVdepSource::SCVdepSource
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode,
	const JIndex			posDepNode,
	const JIndex			negDepNode,
	const JFloat			coeff
	)
	:
	SCDepSource(type, theCircuit, name, posNode, negNode, coeff)
{
	assert( theCircuit->NodeIndexValid(posDepNode) );
	assert( theCircuit->NodeIndexValid(negDepNode) );

	SCVdepSourceX();

	itsPosDepNode = posDepNode;
	itsNegDepNode = negDepNode;
}

SCVdepSource::SCVdepSource
	(
	std::istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCDepSource(input, vers, type, theCircuit)
{
	SCVdepSourceX();
	input >> itsPosDepNode >> itsNegDepNode;
}

// netlist parser

SCVdepSource::SCVdepSource
	(
	std::istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCDepSource(input, type, theCircuit)
{
	SCVdepSourceX();

	const JString posNodeName = JReadUntilws(input);
	itsPosDepNode = theCircuit->FindAddNode(posNodeName);

	const JString negNodeName = JReadUntilws(input);
	itsNegDepNode = theCircuit->FindAddNode(negNodeName);

	JFloat coeff;
	input >> coeff;
	const JString multStr = JReadUntilws(input);
	coeff *= GetSpiceMultiplier(multStr);
	SetCoeffValue(coeff);
}

// private

void
SCVdepSource::SCVdepSourceX()
{
	ListenTo(GetCircuit()->GetNodeList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVdepSource::~SCVdepSource()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCVdepSource::SCVdepSource
	(
	const SCVdepSource&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap,
	const JString&			coeffSymbol
	)
	:
	SCDepSource(source, theCircuit, nodeMap, coeffSymbol)
{
	if (nodeMap != NULL)
		{
		itsPosDepNode = nodeMap->GetElement(source.itsPosDepNode);
		itsNegDepNode = nodeMap->GetElement(source.itsNegDepNode);
		}
	else
		{
		itsPosDepNode = source.itsPosDepNode;
		itsNegDepNode = source.itsNegDepNode;
		}

	assert( theCircuit->NodeIndexValid(itsPosDepNode) );
	assert( theCircuit->NodeIndexValid(itsNegDepNode) );

	ListenTo(GetCircuit()->GetNodeList());
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCVdepSource::StreamOut
	(
	std::ostream& output
	)
	const
{
	SCDepSource::StreamOut(output);
	output << ' ' << itsPosDepNode;
	output << ' ' << itsNegDepNode;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCVdepSource::PrintToNetlist
	(
	std::ostream& output
	)
	const
{
	SCCircuit* theCircuit = GetCircuit();

	SCDepSource::PrintToNetlist(output);
	output << '\t' << theCircuit->GetNodeIndexForNetlist(itsPosDepNode);
	output << '\t' << theCircuit->GetNodeIndexForNetlist(itsNegDepNode);
	output << '\t' << GetCoeffValue();
}

/******************************************************************************
 RequiresNode

 ******************************************************************************/

JBoolean
SCVdepSource::RequiresNode
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean(index == itsPosDepNode || index == itsNegDepNode ||
							 SCDepSource::RequiresNode(index));
}

/******************************************************************************
 GetAuxEquations

	<value> = (<coeff>) * (Vdeppos - Vdepneg)

 ******************************************************************************/

void
SCVdepSource::GetAuxEquations
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

	JString* rhs = new JString("(");
	assert( rhs != NULL );
	*rhs += GetCoeffSymbol();
	*rhs += ") * (";
	*rhs += theCircuit->GetNodeName(itsPosDepNode);
	*rhs += " - ";
	*rhs += theCircuit->GetNodeName(itsNegDepNode);
	*rhs += ")";

	rhsList->Append(rhs);
}

/******************************************************************************
 Receive (protected)

	Listen for changes that affect our nodes.

 ******************************************************************************/

void
SCVdepSource::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JCollection* nodeList = GetCircuit()->GetNodeList();

	if (sender == nodeList && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
		assert( info != NULL );
		info->AdjustIndex(&itsPosDepNode);
		info->AdjustIndex(&itsNegDepNode);
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementsRemoved))
		{
		if (!nodeList->IsEmpty())
			{
			const JOrderedSetT::ElementsRemoved* info =
				dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
			assert( info != NULL );
			const JBoolean posOK = info->AdjustIndex(&itsPosDepNode);
			assert( posOK );
			const JBoolean negOK = info->AdjustIndex(&itsNegDepNode);
			assert( negOK );
			}
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );
		info->AdjustIndex(&itsPosDepNode);
		info->AdjustIndex(&itsNegDepNode);
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
		assert( info != NULL );
		info->AdjustIndex(&itsPosDepNode);
		info->AdjustIndex(&itsNegDepNode);
		}

	SCDepSource::Receive(sender, message);
}

/******************************************************************************
 Cast to SCVdepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCVdepSource*
SCVdepSource::CastToSCVdepSource()
{
	return this;
}

const SCVdepSource*
SCVdepSource::CastToSCVdepSource()
	const
{
	return this;
}
