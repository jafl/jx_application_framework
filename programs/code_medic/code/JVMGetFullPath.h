/******************************************************************************
 JVMGetFullPath.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFullPath
#define _H_JVMGetFullPath

#include "CMGetFullPath.h"

class JVMGetFullPath : public CMGetFullPath
{
public:

	JVMGetFullPath(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~JVMGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetFullPath(const JVMGetFullPath& source);
	const JVMGetFullPath& operator=(const JVMGetFullPath& source);
};

#endif
