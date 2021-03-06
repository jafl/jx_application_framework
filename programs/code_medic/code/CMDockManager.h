/******************************************************************************
 CMDockManager.h

	Copyright (C) 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDockManager
#define _H_CMDockManager

#include <JXDockManager.h>

class CMDockManager : public JXDockManager
{
public:

	CMDockManager();

	virtual ~CMDockManager();

	virtual	JBoolean	CanDockAll() const;
	virtual void		DockAll();

private:

	// not allowed

	CMDockManager(const CMDockManager& source);
	const CMDockManager& operator=(const CMDockManager& source);
};

#endif
