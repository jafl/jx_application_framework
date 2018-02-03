/******************************************************************************
 JExprNodeList.cpp

	This class converts a tree of JDecisions and JFunctions into a list.
	We do not own the JDecisions and JFunctions.  We only store pointers
	to them.

	To do:

		Add RecurseNodes() functions to build the list in breadth-first
		order.  To build it, we simply add the root to the list and then
		ask each node in the list to append its childred.  We stop when
		we hit the end of the list.

	BASE CLASS = JContainer

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JExprNodeList.h>

#include <JDecision.h>
#include <JBooleanDecision.h>
#include <JFunctionComparison.h>
#include <JBooleanNOT.h>

#include <JFunction.h>
#include <JFunctionWithArgs.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JExprNodeList::JExprNodeList
	(
	JDecision* theDecision
	)
	:
	JContainer()
{
	ExprNodeListX();
	RecurseNodesForDecision(kJExprRootNode, theDecision);
}

JExprNodeList::JExprNodeList
	(
	JFunction* theFunction
	)
	:
	JContainer()
{
	ExprNodeListX();
	RecurseNodesForFunction(kJExprRootNode, theFunction);
}

// private

void
JExprNodeList::ExprNodeListX()
{
	itsNodes = jnew JArray<Node>(10);
	assert( itsNodes != NULL );

	InstallOrderedSet(itsNodes);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExprNodeList::~JExprNodeList()
{
	jdelete itsNodes;
}

/******************************************************************************
 GetNodeType

 ******************************************************************************/

JExprNodeType
JExprNodeList::GetNodeType
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	return node.type;
}

/******************************************************************************
 GetParentIndex

 ******************************************************************************/

JIndex
JExprNodeList::GetParentIndex
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	return node.parent;
}

/******************************************************************************
 GetDecision

 ******************************************************************************/

JDecision*
JExprNodeList::GetDecision
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	assert( node.type == kJDecisionNode );
	return node.d;
}

/******************************************************************************
 GetDecisionType

 ******************************************************************************/

JDecisionType
JExprNodeList::GetDecisionType
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	assert( node.type == kJDecisionNode );
	return (node.d)->GetType();
}

/******************************************************************************
 GetFunction

 ******************************************************************************/

JFunction*
JExprNodeList::GetFunction
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	assert( node.type == kJFunctionNode );
	return node.f;
}

/******************************************************************************
 GetFunctionType

 ******************************************************************************/

JFunctionType
JExprNodeList::GetFunctionType
	(
	const JIndex index
	)
	const
{
	const Node node = itsNodes->GetElement(index);
	assert( node.type == kJFunctionNode );
	return (node.f)->GetType();
}

/******************************************************************************
 Find (JDecision)

 ******************************************************************************/

JBoolean
JExprNodeList::Find
	(
	JDecision*	theDecision,
	JIndex*		index
	)
	const
{
	const JSize nodeCount = itsNodes->GetElementCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		const Node node = itsNodes->GetElement(i);
		if (node.type == kJDecisionNode && node.d == theDecision)
			{
			*index = i;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 Find (JFunction)

 ******************************************************************************/

JBoolean
JExprNodeList::Find
	(
	JFunction*	theFunction,
	JIndex*		index
	)
	const
{
	const JSize nodeCount = itsNodes->GetElementCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		const Node node = itsNodes->GetElement(i);
		if (node.type == kJFunctionNode && node.f == theFunction)
			{
			*index = i;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 RecurseNodesForDecision

 ******************************************************************************/

void
JExprNodeList::RecurseNodesForDecision
	(
	const JIndex	parentNode,
	JDecision*		theDecision
	)
{
	Node node;
	node.type   = kJDecisionNode;
	node.d      = theDecision;
	node.parent = parentNode;
	itsNodes->AppendElement(node);

	const JIndex currentNode = itsNodes->GetElementCount();
	theDecision->BuildNodeList(this, currentNode);
}

/******************************************************************************
 RecurseNodesForFunction

 ******************************************************************************/

void
JExprNodeList::RecurseNodesForFunction
	(
	const JIndex	parentNode,
	JFunction*		theFunction
	)
{
	Node node;
	node.type   = kJFunctionNode;
	node.f      = theFunction;
	node.parent = parentNode;
	itsNodes->AppendElement(node);

	const JIndex currentNode = itsNodes->GetElementCount();
	theFunction->BuildNodeList(this, currentNode);
}
