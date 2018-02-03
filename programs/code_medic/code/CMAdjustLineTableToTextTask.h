/******************************************************************************
 CMAdjustLineTableToTextTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMAdjustLineTableToTextTask
#define _H_CMAdjustLineTableToTextTask

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
