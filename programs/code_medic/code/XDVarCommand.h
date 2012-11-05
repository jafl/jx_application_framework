/******************************************************************************
 XDVarCommand.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDVarCommand
#define _H_XDVarCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMVarCommand.h"

class CMVarNode;

class XDVarCommand : public CMVarCommand
{
public:

	XDVarCommand(const JCharacter* cmd);

	virtual	~XDVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	CMVarNode*	itsRootNode;

private:

	// not allowed

	XDVarCommand(const XDVarCommand& source);
	const XDVarCommand& operator=(const XDVarCommand& source);
};

#endif
