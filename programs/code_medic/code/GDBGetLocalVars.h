/******************************************************************************
 GDBGetLocalVars.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetLocalVars
#define _H_GDBGetLocalVars

#include "CMGetLocalVars.h"

class CMVarNode;

class GDBGetLocalVars : public CMGetLocalVars
{
public:

	GDBGetLocalVars(CMVarNode* rootNode);

	virtual	~GDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);

	// not allowed

	GDBGetLocalVars(const GDBGetLocalVars& source);
	const GDBGetLocalVars& operator=(const GDBGetLocalVars& source);
};

#endif
