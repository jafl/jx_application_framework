/******************************************************************************
 XDGetInitArgs.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetInitArgs
#define _H_XDGetInitArgs

#include "CMGetInitArgs.h"

class JXInputField;

class XDGetInitArgs : public CMGetInitArgs
{
public:

	XDGetInitArgs(JXInputField* argInput);

	virtual	~XDGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	XDGetInitArgs(const XDGetInitArgs& source);
	const XDGetInitArgs& operator=(const XDGetInitArgs& source);

};

#endif
