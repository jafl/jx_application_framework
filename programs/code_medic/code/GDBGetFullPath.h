/******************************************************************************
 GDBGetFullPath.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetFullPath
#define _H_GDBGetFullPath

#include "CMGetFullPath.h"

class GDBGetFullPath : public CMGetFullPath
{
public:

	GDBGetFullPath(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~GDBGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	static JString	BuildCommand(const JString& fileName);

	// not allowed

	GDBGetFullPath(const GDBGetFullPath& source);
	const GDBGetFullPath& operator=(const GDBGetFullPath& source);
};

#endif
