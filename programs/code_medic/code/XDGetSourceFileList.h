/******************************************************************************
 XDGetSourceFileList.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetSourceFileList
#define _H_XDGetSourceFileList

#include "CMGetSourceFileList.h"

class CMFileListDir;

class XDGetSourceFileList : public CMGetSourceFileList
{
public:

	XDGetSourceFileList(CMFileListDir* fileList);

	virtual	~XDGetSourceFileList();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ScanDirectory(const JString& path);

	// not allowed

	XDGetSourceFileList(const XDGetSourceFileList& source);
	const XDGetSourceFileList& operator=(const XDGetSourceFileList& source);

};

#endif
