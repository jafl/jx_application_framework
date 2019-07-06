/******************************************************************************
 LLDBGetThreads.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThreads
#define _H_LLDBGetThreads

#include "CMGetThreads.h"

class JTree;

class LLDBGetThreads : public CMGetThreads
{
public:

	LLDBGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~LLDBGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;	// not owned

private:

	// not allowed

	LLDBGetThreads(const LLDBGetThreads& source);
	const LLDBGetThreads& operator=(const LLDBGetThreads& source);
};

#endif
