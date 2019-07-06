/******************************************************************************
 LLDBGetInitArgs.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetInitArgs
#define _H_LLDBGetInitArgs

#include "CMGetInitArgs.h"

class JXInputField;

class LLDBGetInitArgs : public CMGetInitArgs
{
public:

	LLDBGetInitArgs(JXInputField* argInput);

	virtual	~LLDBGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JXInputField*	itsArgInput;

private:

	// not allowed

	LLDBGetInitArgs(const LLDBGetInitArgs& source);
	const LLDBGetInitArgs& operator=(const LLDBGetInitArgs& source);

};

#endif
