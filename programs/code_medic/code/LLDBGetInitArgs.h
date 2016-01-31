/******************************************************************************
 LLDBGetInitArgs.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetInitArgs
#define _H_LLDBGetInitArgs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetInitArgs.h"

class JXInputField;

class LLDBGetInitArgs : public CMGetInitArgs
{
public:

	LLDBGetInitArgs(JXInputField* argInput);

	virtual	~LLDBGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JXInputField*	itsArgInput;

private:

	// not allowed

	LLDBGetInitArgs(const LLDBGetInitArgs& source);
	const LLDBGetInitArgs& operator=(const LLDBGetInitArgs& source);

};

#endif
