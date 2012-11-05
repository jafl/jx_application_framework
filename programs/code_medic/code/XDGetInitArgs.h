/******************************************************************************
 XDGetInitArgs.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetInitArgs
#define _H_XDGetInitArgs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetInitArgs.h"

class JXInputField;

class XDGetInitArgs : public CMGetInitArgs
{
public:

	XDGetInitArgs(JXInputField* argInput);

	virtual	~XDGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDGetInitArgs(const XDGetInitArgs& source);
	const XDGetInitArgs& operator=(const XDGetInitArgs& source);

};

#endif
