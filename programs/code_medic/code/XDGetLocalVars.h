/******************************************************************************
 XDGetLocalVars.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetLocalVars
#define _H_XDGetLocalVars

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetLocalVars.h"

class CMVarNode;

class XDGetLocalVars : public CMGetLocalVars
{
public:

	XDGetLocalVars(CMVarNode* rootNode);

	virtual	~XDGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMVarNode*	itsRootNode;	// not owned

private:

	// not allowed

	XDGetLocalVars(const XDGetLocalVars& source);
	const XDGetLocalVars& operator=(const XDGetLocalVars& source);
};

#endif
