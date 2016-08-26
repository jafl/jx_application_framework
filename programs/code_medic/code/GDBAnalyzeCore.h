/******************************************************************************
 GDBAnalyzeCore.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBAnalyzeCore
#define _H_GDBAnalyzeCore

#include "CMCommand.h"

class GDBAnalyzeCore : public CMCommand
{
public:

	GDBAnalyzeCore(const JCharacter* cmd);

	virtual	~GDBAnalyzeCore();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBAnalyzeCore(const GDBAnalyzeCore& source);
	const GDBAnalyzeCore& operator=(const GDBAnalyzeCore& source);
};

#endif
