/******************************************************************************
 CMGetThreads.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetThreads
#define _H_CMGetThreads

#include "CMCommand.h"

class CMThreadsWidget;

class CMGetThreads : public CMCommand
{
public:

	CMGetThreads(const JString& cmd, CMThreadsWidget* widget);

	virtual	~CMGetThreads();

	CMThreadsWidget*	GetWidget();

protected:

	virtual void	HandleFailure();

private:

	CMThreadsWidget*	itsWidget;		// not owned

private:

	// not allowed

	CMGetThreads(const CMGetThreads& source);
	const CMGetThreads& operator=(const CMGetThreads& source);
};


/******************************************************************************
 GetWidget

 ******************************************************************************/

inline CMThreadsWidget*
CMGetThreads::GetWidget()
{
	return itsWidget;
}

#endif
