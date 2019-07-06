/******************************************************************************
 GDBGetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetInitArgs
#define _H_GDBGetInitArgs

#include "CMGetInitArgs.h"

class JXInputField;

class GDBGetInitArgs : public CMGetInitArgs
{
public:

	GDBGetInitArgs(JXInputField* argInput);

	virtual	~GDBGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JXInputField*	itsArgInput;

private:

	// not allowed

	GDBGetInitArgs(const GDBGetInitArgs& source);
	const GDBGetInitArgs& operator=(const GDBGetInitArgs& source);

};

#endif
