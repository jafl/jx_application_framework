/******************************************************************************
 LLDBGetFullPath.h

	Copyright � 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetFullPath
#define _H_LLDBGetFullPath

#include "CMGetFullPath.h"

class LLDBGetFullPath : public CMGetFullPath
{
public:

	LLDBGetFullPath(const JCharacter* fileName, const JIndex lineIndex = 0);

	virtual	~LLDBGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetFullPath(const LLDBGetFullPath& source);
	const LLDBGetFullPath& operator=(const LLDBGetFullPath& source);
};

#endif
