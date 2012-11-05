/******************************************************************************
 GDBGetSourceFileList.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetSourceFileList
#define _H_GDBGetSourceFileList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetSourceFileList.h"

class CMFileListDir;

class GDBGetSourceFileList : public CMGetSourceFileList
{
public:

	GDBGetSourceFileList(CMFileListDir* fileList);

	virtual	~GDBGetSourceFileList();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	GDBGetSourceFileList(const GDBGetSourceFileList& source);
	const GDBGetSourceFileList& operator=(const GDBGetSourceFileList& source);

};

#endif
