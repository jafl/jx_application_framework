/******************************************************************************
 GDBGetLocalVars.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetLocalVars
#define _H_GDBGetLocalVars

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetLocalVars.h"

class CMVarNode;

class GDBGetLocalVars : public CMGetLocalVars
{
public:

	GDBGetLocalVars(CMVarNode* rootNode);

	virtual	~GDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);

	// not allowed

	GDBGetLocalVars(const GDBGetLocalVars& source);
	const GDBGetLocalVars& operator=(const GDBGetLocalVars& source);
};

#endif
