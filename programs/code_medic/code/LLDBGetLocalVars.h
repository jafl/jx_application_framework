/******************************************************************************
 LLDBGetLocalVars.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetLocalVars
#define _H_LLDBGetLocalVars

#include "CMGetLocalVars.h"

class CMVarNode;

class LLDBGetLocalVars : public CMGetLocalVars
{
public:

	LLDBGetLocalVars(CMVarNode* rootNode);

	virtual	~LLDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);

	// not allowed

	LLDBGetLocalVars(const LLDBGetLocalVars& source);
	const LLDBGetLocalVars& operator=(const LLDBGetLocalVars& source);
};

#endif
