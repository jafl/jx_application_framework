/******************************************************************************
 JVMGetSourceFileList.h

	Copyright (C) 2009 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetSourceFileList
#define _H_JVMGetSourceFileList

#include "CMGetSourceFileList.h"

class CMFileListDir;

class JVMGetSourceFileList : public CMGetSourceFileList
{
public:

	JVMGetSourceFileList(CMFileListDir* fileList);

	virtual	~JVMGetSourceFileList();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	void	ScanDirectory(const JCharacter* path);

	// not allowed

	JVMGetSourceFileList(const JVMGetSourceFileList& source);
	const JVMGetSourceFileList& operator=(const JVMGetSourceFileList& source);

};

#endif
