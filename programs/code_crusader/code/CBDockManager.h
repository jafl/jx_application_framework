/******************************************************************************
 CBDockManager.h

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDockManager
#define _H_CBDockManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDockManager.h>

class CBDockManager : public JXDockManager
{
public:

	CBDockManager();

	virtual ~CBDockManager();

	virtual	JBoolean	CanDockAll() const;
	virtual void		DockAll();

private:

	// not allowed

	CBDockManager(const CBDockManager& source);
	const CBDockManager& operator=(const CBDockManager& source);
};

#endif
