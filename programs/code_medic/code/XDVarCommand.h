/******************************************************************************
 XDVarCommand.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDVarCommand
#define _H_XDVarCommand

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
