/******************************************************************************
 CMAdjustLineTableToTextTask.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMAdjustLineTableToTextTask
#define _H_CMAdjustLineTableToTextTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class CMLineIndexTable;

class CMAdjustLineTableToTextTask : public JXUrgentTask
{
public:

	CMAdjustLineTableToTextTask(CMLineIndexTable* table);

	virtual ~CMAdjustLineTableToTextTask();

	virtual void	Perform();

private:

	CMLineIndexTable*	itsTable;	// not owned

private:

	// not allowed

	CMAdjustLineTableToTextTask(const CMAdjustLineTableToTextTask& source);
	const CMAdjustLineTableToTextTask& operator=(const CMAdjustLineTableToTextTask& source);
};

#endif
