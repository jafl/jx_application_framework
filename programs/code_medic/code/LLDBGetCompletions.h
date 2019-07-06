/******************************************************************************
 LLDBGetCompletions.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetCompletions
#define _H_LLDBGetCompletions

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class LLDBGetCompletions : public CMGetCompletions
{
public:

	LLDBGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~LLDBGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CMCommandInput*			itsInput;
	CMCommandOutputDisplay*	itsHistory;

private:

	// not allowed

	LLDBGetCompletions(const LLDBGetCompletions& source);
	const LLDBGetCompletions& operator=(const LLDBGetCompletions& source);
};

#endif
