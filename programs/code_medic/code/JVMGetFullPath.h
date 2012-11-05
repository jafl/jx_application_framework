/******************************************************************************
 JVMGetFullPath.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetFullPath
#define _H_JVMGetFullPath

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetFullPath.h"

class JVMGetFullPath : public CMGetFullPath
{
public:

	JVMGetFullPath(const JCharacter* fileName, const JIndex lineIndex = 0);

	virtual	~JVMGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetFullPath(const JVMGetFullPath& source);
	const JVMGetFullPath& operator=(const JVMGetFullPath& source);
};

#endif
