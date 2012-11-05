/******************************************************************************
 CMGetStack.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetStack
#define _H_CMGetStack

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class JTree;
class CMStackWidget;

class CMGetStack : public CMCommand
{
public:

	CMGetStack(const JCharacter* cmd, JTree* tree, CMStackWidget* widget);

	virtual	~CMGetStack();

	JTree*			GetTree();
	CMStackWidget*	GetWidget();

protected:

	virtual void	HandleFailure();

private:

	JTree*			itsTree;	// not owned
	CMStackWidget*	itsWidget;	// not owned

private:

	// not allowed

	CMGetStack(const CMGetStack& source);
	const CMGetStack& operator=(const CMGetStack& source);
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline JTree*
CMGetStack::GetTree()
{
	return itsTree;
}

/******************************************************************************
 GetWidget

 ******************************************************************************/

inline CMStackWidget*
CMGetStack::GetWidget()
{
	return itsWidget;
}

#endif
