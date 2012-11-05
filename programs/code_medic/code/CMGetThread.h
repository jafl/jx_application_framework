/******************************************************************************
 CMGetThread.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetThread
#define _H_CMGetThread

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMThreadsWidget;

class CMGetThread : public CMCommand
{
public:

	CMGetThread(const JCharacter* cmd, CMThreadsWidget* widget);

	virtual	~CMGetThread();

	CMThreadsWidget*	GetWidget();

protected:

	virtual void	HandleFailure();

private:

	CMThreadsWidget*	itsWidget;		// not owned

private:

	// not allowed

	CMGetThread(const CMGetThread& source);
	const CMGetThread& operator=(const CMGetThread& source);
};


/******************************************************************************
 GetWidget

 ******************************************************************************/

inline CMThreadsWidget*
CMGetThread::GetWidget()
{
	return itsWidget;
}

#endif
