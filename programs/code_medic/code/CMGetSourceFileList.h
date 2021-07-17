/******************************************************************************
 CMGetSourceFileList.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetSourceFileList
#define _H_CMGetSourceFileList

#include "CMCommand.h"
#include <JBroadcaster.h>

class CMFileListDir;

class CMGetSourceFileList : public CMCommand, virtual public JBroadcaster
{
public:

	CMGetSourceFileList(const JString& cmd, CMFileListDir* fileList);

	virtual	~CMGetSourceFileList();

	CMFileListDir*	GetFileList();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CMFileListDir*	itsFileList;			// not owned
	bool		itsNeedRedoOnFirstStop;

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
