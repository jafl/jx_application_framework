/******************************************************************************
 GDBGetSourceFileList.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetSourceFileList
#define _H_GDBGetSourceFileList

#include "CMGetSourceFileList.h"

class CMFileListDir;

class GDBGetSourceFileList : public CMGetSourceFileList
{
public:

	GDBGetSourceFileList(CMFileListDir* fileList);

	virtual	~GDBGetSourceFileList();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBGetSourceFileList(const GDBGetSourceFileList& source);
	const GDBGetSourceFileList& operator=(const GDBGetSourceFileList& source);

};

#endif
