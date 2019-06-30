/******************************************************************************
 CMGetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetThread
#define _H_CMGetThread

#include "CMCommand.h"

class CMThreadsWidget;

class CMGetThread : public CMCommand
{
public:

	CMGetThread(const JString& cmd, CMThreadsWidget* widget);

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
