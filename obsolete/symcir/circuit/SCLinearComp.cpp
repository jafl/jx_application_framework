/******************************************************************************
 SCLinearComp.cpp

	This class defines the requirements for all linear (2 terminal) components.

	Derived classes will probably need to override:

	GetAuxEquations
		Provide pairs of expressions that must be equal and
		variables that need to be solved for.

	GetParserVariables
		Derived classes must add all symbols other than the current through
		the component.

	BASE CLASS = SCComponent

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCLinearComp.h"
#include "SCCircuit.h"
#include "SCCircuitVarList.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JSize kTerminalCount = 2;

static const JCharacter* kCurrentSymbolPrefix = "B";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCLinearComp::SCLinearComp
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name,
	const JIndex			posNode,
	const JIndex			negNode
	)
	:
	SCComponent(type, theCircuit, name)
{
	assert( theCircuit->NodeIndexValid(posNode) );
	assert( theCircuit->NodeIndexValid(negNode) );

	SCLinearCompX();
	SetValueSymbol(GetName());

	itsPosNode = posNode;
	itsNegNode = negNode;
}

SCLinearComp::SCLinearComp
	(
	istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCComponent(input, vers, type, theCircuit)
{
	SCLinearCompX();
	input >> *itsValueSymbol >> *itsCurrentSymbol;
	input >> itsPosNode >> itsNegNode;
}

// netlist parser

SCLinearComp::SCLinearComp
	(
	istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	SCComponent(input, type, theCircuit)
{
	SCLinearCompX();
	SetValueSymbol(GetName());

	const JString posNodeName = JReadUntilws(input);
	itsPosNode = theCircuit->FindAddNode(posNodeName);

	const JString negNodeName = JReadUntilws(input);
	itsNegNode = theCircuit->FindAddNode(negNodeName);
}

// private

void
SCLinearComp::SCLinearCompX()
{
	itsValueSymbol = new JString;
	assert( itsValueSymbol != NULL );

	itsCurrentSymbol = new JString;
	assert( itsCurrentSymbol != NULL );

	ListenTo(GetCircuit()->GetNodeList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCLinearComp::~SCLinearComp()
{
	delete itsValueSymbol;
	delete itsCurrentSymbol;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCLinearComp::SCLinearComp
	(
	const SCLinearComp&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	SCComponent(source, theCircuit, nodeMap)
{
	itsValueSymbol = new JString(*(source.itsValueSymbol));
	assert( itsValueSymbol != NULL );

	itsCurrentSymbol = new JString(*(source.itsCurrentSymbol));
	assert( itsCurrentSymbol != NULL );

	if (nodeMap != NULL)
		{
		itsPosNode = nodeMap->GetElement(source.itsPosNode);
		itsNegNode = nodeMap->GetElement(source.itsNegNode);
		}
	else
		{
		itsPosNode = source.itsPosNode;
		itsNegNode = source.itsNegNode;
		}

	assert( theCircuit->NodeIndexValid(itsPosNode) );
	assert( theCircuit->NodeIndexValid(itsNegNode) );

	ListenTo(GetCircuit()->GetNodeList());
}

/******************************************************************************
 GetTerminalCount

	Not inline because it is virtual

 ******************************************************************************/

JSize
SCLinearComp::GetTerminalCount()
	const
{
	return kTerminalCount;
}

/******************************************************************************
 GetTerminal

	Not inline because it is virtual

 ******************************************************************************/

JIndex
SCLinearComp::GetTerminal
	(
	const JIndex index
	)
	const
{
	assert( index == 1 || index == 2 );

	if (index == 1)
		{
		return itsPosNode;
		}
	else
		{
		return itsNegNode;
		}
}

/******************************************************************************
 SetTerminal

	Not inline because it is virtual

 ******************************************************************************/

void
SCLinearComp::SetTerminal
	(
	const JIndex index,
	const JIndex node
	)
{
	assert( index == 1 || index == 2 );

	if (index == 1)
		{
		itsPosNode = node;
		}
	else
		{
		itsNegNode = node;
		}
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCLinearComp::StreamOut
	(
	ostream& output
	)
	const
{
	SCComponent::StreamOut(output);
	output << ' ' << *itsValueSymbol;
	output << ' ' << *itsCurrentSymbol;
	output << ' ' << itsPosNode;
	output << ' ' << itsNegNode;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCLinearComp::PrintToNetlist
	(
	ostream& output
	)
	const
{
	SCCircuit* theCircuit = GetCircuit();

	SCComponent::PrintToNetlist(output);
	output << '\t' << theCircuit->GetNodeIndexForNetlist(itsPosNode);
	output << '\t' << theCircuit->GetNodeIndexForNetlist(itsNegNode);
}

/******************************************************************************
 GetAuxEquations

 ******************************************************************************/

void
SCLinearComp::GetAuxEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	JString* var = new JString(*itsCurrentSymbol);
	assert( var != NULL );

	varList->Append(var);
}

/******************************************************************************
 GetParserVariables

 ******************************************************************************/

void
SCLinearComp::GetParserVariables
	(
	SCCircuitVarList* varList
	)
	const
{
	varList->AddVariable(GetCurrentSymbol(), 0.0, kJFalse);
}

/******************************************************************************
 Cast to SCLinearComp*

	Not inline because they are virtual

 ******************************************************************************/

SCLinearComp*
SCLinearComp::CastToSCLinearComp()
{
	return this;
}

const SCLinearComp*
SCLinearComp::CastToSCLinearComp()
	const
{
	return this;
}

/******************************************************************************
 SetValueSymbol

 ******************************************************************************/

void
SCLinearComp::SetValueSymbol
	(
	const JCharacter* symbol
	)
{
	*itsValueSymbol = symbol;

	*itsCurrentSymbol  = kCurrentSymbolPrefix;
	*itsCurrentSymbol += symbol;
}

/******************************************************************************
 RequiresNode

 ******************************************************************************/

JBoolean
SCLinearComp::RequiresNode
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean(index == itsPosNode || index == itsNegNode ||
							 SCComponent::RequiresNode(index));
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes that affect our nodes.

 ******************************************************************************/

void
SCLinearComp::Receive
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
		info->AdjustIndex(&itsPosNode);
		info->AdjustIndex(&itsNegNode);
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementsRemoved))
		{
		if (!nodeList->IsEmpty())
			{
			const JOrderedSetT::ElementsRemoved* info =
				dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
			assert( info != NULL );
			const JBoolean posOK = info->AdjustIndex(&itsPosNode);
			assert( posOK );
			const JBoolean negOK = info->AdjustIndex(&itsNegNode);
			assert( negOK );
			}
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );
		info->AdjustIndex(&itsPosNode);
		info->AdjustIndex(&itsNegNode);
		}

	else if (sender == nodeList && message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
		assert( info != NULL );
		info->AdjustIndex(&itsPosNode);
		info->AdjustIndex(&itsNegNode);
		}

	SCComponent::Receive(sender, message);
}
