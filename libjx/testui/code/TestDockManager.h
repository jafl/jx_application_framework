/******************************************************************************
 TestDockManager.h

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDockManager
#define _H_TestDockManager

#include <JXDockManager.h>

class TestDockManager : public JXDockManager
{
public:

	TestDockManager(JXDisplay* display);

	virtual ~TestDockManager();

	virtual	JBoolean	CanDockAll() const;
	virtual void		DockAll();

private:

	// not allowed

	TestDockManager(const TestDockManager& source);
	const TestDockManager& operator=(const TestDockManager& source);
};

#endif
