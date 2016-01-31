/******************************************************************************
 LLDBGetSourceFileList.h

	Copyright © 2016 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetSourceFileList
#define _H_LLDBGetSourceFileList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetSourceFileList.h"

class CMFileListDir;

class LLDBGetSourceFileList : public CMGetSourceFileList
{
public:

	LLDBGetSourceFileList(CMFileListDir* fileList);

	virtual	~LLDBGetSourceFileList();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetSourceFileList(const LLDBGetSourceFileList& source);
	const LLDBGetSourceFileList& operator=(const LLDBGetSourceFileList& source);

};

#endif
