/******************************************************************************
 GDBGetFullPath.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetFullPath
#define _H_GDBGetFullPath

#include "CMGetFullPath.h"

class GDBGetFullPath : public CMGetFullPath
{
public:

	GDBGetFullPath(const JCharacter* fileName, const JIndex lineIndex = 0);

	virtual	~GDBGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	static JString	BuildCommand(const JCharacter* fileName);

	// not allowed

	GDBGetFullPath(const GDBGetFullPath& source);
	const GDBGetFullPath& operator=(const GDBGetFullPath& source);
};

#endif
