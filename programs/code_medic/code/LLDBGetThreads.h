/******************************************************************************
 LLDBGetThreads.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetThreads
#define _H_LLDBGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThreads.h"

class JTree;

class LLDBGetThreads : public CMGetThreads
{
public:

	LLDBGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~LLDBGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JTree*	itsTree;	// not owned

private:

	// not allowed

	LLDBGetThreads(const LLDBGetThreads& source);
	const LLDBGetThreads& operator=(const LLDBGetThreads& source);
};

#endif
