/******************************************************************************
 LLDBGetAssembly.h

	Copyright � 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetAssembly
#define _H_LLDBGetAssembly

#include "CMGetAssembly.h"

class CMSourceDirector;

class LLDBGetAssembly : public CMGetAssembly
{
public:

	LLDBGetAssembly(CMSourceDirector* dir);

	virtual	~LLDBGetAssembly();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetAssembly(const LLDBGetAssembly& source);
	const LLDBGetAssembly& operator=(const LLDBGetAssembly& source);

};

#endif
