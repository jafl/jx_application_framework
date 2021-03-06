/******************************************************************************
 XDGetFullPath.h

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetFullPath
#define _H_XDGetFullPath

#include "CMGetFullPath.h"

class XDGetFullPath : public CMGetFullPath
{
public:

	XDGetFullPath(const JCharacter* fileName, const JIndex lineIndex = 0);

	virtual	~XDGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDGetFullPath(const XDGetFullPath& source);
	const XDGetFullPath& operator=(const XDGetFullPath& source);
};

#endif
