/******************************************************************************
 CMUpdateCommandsTableWidth.h

	Interface for the CMUpdateCommandsTableWidth class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMUpdateCommandsTableWidth
#define _H_CMUpdateCommandsTableWidth

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class CMEditCommandsTable;

class CMUpdateCommandsTableWidth : public JXUrgentTask
{
public:

	CMUpdateCommandsTableWidth(CMEditCommandsTable* table);

	virtual ~CMUpdateCommandsTableWidth();

	virtual void	Perform();

private:

	CMEditCommandsTable*	itsTable;		// we don't own this

private:

	// not allowed

	CMUpdateCommandsTableWidth(const CMUpdateCommandsTableWidth& source);
	const CMUpdateCommandsTableWidth& operator=(const CMUpdateCommandsTableWidth& source);
};

#endif
