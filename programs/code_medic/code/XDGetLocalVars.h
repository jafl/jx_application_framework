/******************************************************************************
 XDGetLocalVars.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetLocalVars
#define _H_XDGetLocalVars

#include "CMGetLocalVars.h"

class CMVarNode;

class XDGetLocalVars : public CMGetLocalVars
{
public:

	XDGetLocalVars(CMVarNode* rootNode);

	virtual	~XDGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	// not allowed

	XDGetLocalVars(const XDGetLocalVars& source);
	const XDGetLocalVars& operator=(const XDGetLocalVars& source);
};

#endif
