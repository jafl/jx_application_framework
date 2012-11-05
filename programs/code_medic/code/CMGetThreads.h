/******************************************************************************
 CMGetThreads.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetThreads
#define _H_CMGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMThreadsWidget;

class CMGetThreads : public CMCommand
{
public:

	CMGetThreads(const JCharacter* cmd, CMThreadsWidget* widget);

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
