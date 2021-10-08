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

	~TestDockManager() override;

	bool	CanDockAll() const override;
	void	DockAll() override;
};

#endif
