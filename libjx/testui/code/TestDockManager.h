/******************************************************************************
 TestDockManager.h

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDockManager
#define _H_TestDockManager

#include <jx-af/jx/JXDockManager.h>

class TestDockManager : public JXDockManager
{
public:

	TestDockManager(JXDisplay* display);

	virtual ~TestDockManager();

	virtual	bool	CanDockAll() const;
	virtual void	DockAll();
};

#endif
