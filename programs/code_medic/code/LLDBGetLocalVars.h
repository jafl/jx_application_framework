/******************************************************************************
 LLDBGetLocalVars.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetLocalVars
#define _H_LLDBGetLocalVars

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetLocalVars.h"

class CMVarNode;

class LLDBGetLocalVars : public CMGetLocalVars
{
public:

	LLDBGetLocalVars(CMVarNode* rootNode);

	virtual	~LLDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);

	// not allowed

	LLDBGetLocalVars(const LLDBGetLocalVars& source);
	const LLDBGetLocalVars& operator=(const LLDBGetLocalVars& source);
};

#endif
