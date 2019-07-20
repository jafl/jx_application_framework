/******************************************************************************
 GDBGetCompletions.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GDBGetCompletions
#define _H_GDBGetCompletions

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class GDBGetCompletions : public CMGetCompletions
{
public:

	GDBGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~GDBGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CMCommandInput*			itsInput;
	CMCommandOutputDisplay*	itsHistory;

private:

	static JString	BuildCommand(const JString& prefix);

	// not allowed

	GDBGetCompletions(const GDBGetCompletions& source);
	const GDBGetCompletions& operator=(const GDBGetCompletions& source);
};

#endif
