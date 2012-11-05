/******************************************************************************
 CMGetSourceFileList.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetSourceFileList
#define _H_CMGetSourceFileList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"
#include <JBroadcaster.h>

class CMFileListDir;

class CMGetSourceFileList : public CMCommand, virtual public JBroadcaster
{
public:

	CMGetSourceFileList(const JCharacter* cmd, CMFileListDir* fileList);

	virtual	~CMGetSourceFileList();

	CMFileListDir*	GetFileList();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CMFileListDir*	itsFileList;			// not owned
	JBoolean		itsNeedRedoOnFirstStop;

private:

	// not allowed

	CMGetSourceFileList(const CMGetSourceFileList& source);
	const CMGetSourceFileList& operator=(const CMGetSourceFileList& source);

};


/******************************************************************************
 GetFileList

 ******************************************************************************/

inline CMFileListDir*
CMGetSourceFileList::GetFileList()
{
	return itsFileList;
}

#endif
