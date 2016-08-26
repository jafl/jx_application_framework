/******************************************************************************
 JExprNodeList.h

	Interface for the JExprNodeList class

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JExprNodeList
#define _H_JExprNodeList

#include <JContainer.h>
#include <JDecisionType.h>
#include <JFunctionType.h>
#include <JArray.h>

class JDecision;
class JFunction;

const JIndex kJExprRootNode = 0;

enum JExprNodeType
{
	kJDecisionNode,
	kJFunctionNode
};

class JExprNodeList : public JContainer
{
public:

	JExprNodeList(JDecision* theDecision);
	JExprNodeList(JFunction* theFunction);

	virtual ~JExprNodeList();

	JExprNodeType	GetNodeType(const JIndex index) const;
	JIndex			GetParentIndex(const JIndex index) const;

	JDecision*		GetDecision(const JIndex index) const;
	JDecisionType	GetDecisionType(const JIndex index) const;

	JFunction*		GetFunction(const JIndex index) const;
	JFunctionType	GetFunctionType(const JIndex index) const;

	JBoolean		Find(JDecision* theDecision, JIndex* index) const;
	JBoolean		Find(JFunction* theFunction, JIndex* index) const;

	// called by JFunction and JDecision

	void	RecurseNodesForDecision(const JIndex parentNode, JDecision* theDecision);
	void	RecurseNodesForFunction(const JIndex parentNode, JFunction* theFunction);

private:

	struct Node
	{
		JExprNodeType	type;	// determines which of d or f is not NULL
		JDecision*		d;
		JFunction*		f;
		JIndex			parent;

		Node()
		:
		d( NULL ),
		f( NULL ),
		parent( 0 )
		{ };
	};

private:

	JArray<Node>*	itsNodes;

private:

	void	ExprNodeListX();

	// not allowed

	JExprNodeList(const JExprNodeList& source);
	const JExprNodeList& operator=(const JExprNodeList& source);
};

#endif
