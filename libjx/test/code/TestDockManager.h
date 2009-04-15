/******************************************************************************
 TestDockManager.h

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TestDockManager
#define _H_TestDockManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
