/******************************************************************************
 GDBAnalyzeCore.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBAnalyzeCore
#define _H_GDBAnalyzeCore

#include "CMCommand.h"

class GDBAnalyzeCore : public CMCommand
{
public:

	GDBAnalyzeCore(const JString& cmd);

	virtual	~GDBAnalyzeCore();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBAnalyzeCore(const GDBAnalyzeCore& source);
	const GDBAnalyzeCore& operator=(const GDBAnalyzeCore& source);
};

#endif
