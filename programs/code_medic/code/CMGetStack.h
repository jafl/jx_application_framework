/******************************************************************************
 CMGetStack.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetStack
#define _H_CMGetStack

#include "CMCommand.h"

class JTree;
class CMStackWidget;

class CMGetStack : public CMCommand
{
public:

	CMGetStack(const JString& cmd, JTree* tree, CMStackWidget* widget);

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
