/******************************************************************************
 CMGetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetInitArgs
#define _H_CMGetInitArgs

#include "CMCommand.h"

class CMGetInitArgs : public CMCommand
{
public:

	CMGetInitArgs(const JString& cmd);

	virtual	~CMGetInitArgs();

private:

	// not allowed

	CMGetInitArgs(const CMGetInitArgs& source);
	const CMGetInitArgs& operator=(const CMGetInitArgs& source);

};

#endif
