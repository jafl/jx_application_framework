/******************************************************************************
 LLDBGetCompletions.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetCompletions
#define _H_LLDBGetCompletions

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetCompletions.h"

class CMCommandInput;
class CMCommandOutputDisplay;

class LLDBGetCompletions : public CMGetCompletions
{
public:

	LLDBGetCompletions(CMCommandInput* input, CMCommandOutputDisplay* history);

	virtual	~LLDBGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data);

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
