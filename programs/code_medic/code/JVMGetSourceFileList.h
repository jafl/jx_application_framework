/******************************************************************************
 JVMGetSourceFileList.h

	Copyright (C) 2009 by John Lindal.

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

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);

	// not allowed

	JVMGetSourceFileList(const JVMGetSourceFileList& source);
	const JVMGetSourceFileList& operator=(const JVMGetSourceFileList& source);

};

#endif
