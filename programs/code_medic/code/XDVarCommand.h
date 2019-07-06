/******************************************************************************
 XDVarCommand.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDVarCommand
#define _H_XDVarCommand

#include "CMVarCommand.h"

class CMVarNode;

class XDVarCommand : public CMVarCommand
{
public:

	XDVarCommand(const JString& cmd);

	virtual	~XDVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;

private:

	// not allowed

	XDVarCommand(const XDVarCommand& source);
	const XDVarCommand& operator=(const XDVarCommand& source);
};

#endif
