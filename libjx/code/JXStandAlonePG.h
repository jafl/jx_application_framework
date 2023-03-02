/******************************************************************************
 JXStandAlonePG.h

	Interface for the JXStandAlonePG class.

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXStandAlonePG
#define _H_JXStandAlonePG

#include "JXProgressDisplay.h"
#include <boost/fiber/condition_variable.hpp>

class JXPGDirectorBase;
class JXProgressContinueWorkTask;

class JXStandAlonePG : public JXProgressDisplay
{
public:

	JXStandAlonePG();

	~JXStandAlonePG() override;

	bool	ProcessContinuing() override;
	void	ProcessFinished() override;

	void	RaiseWhenUpdate(const bool raise = true);

protected:

	void	ProcessBeginning(const ProcessType processType,
							 const JSize stepCount,
							 const JString& message,
							 const bool allowCancel,
							 const bool modal) override;

private:

	JXPGDirectorBase*	itsProgressDirector;
	bool				itsRaiseWindowFlag;
	JSize				itsStepCount;

	JXProgressContinueWorkTask*			itsContinueTask;
	boost::fibers::condition_variable	itsCondition;
	boost::fibers::mutex				itsMutex;
	bool								itsContinueFlag;
};


/******************************************************************************
 RaiseWhenUpdate

	This controls whether or not the progress window is raised every time
	it is updated.  This is mainly useful if a process creates new windows,
	because it can then be very hard for the user to find the progress
	window and cancel the process.

 ******************************************************************************/

inline void
JXStandAlonePG::RaiseWhenUpdate
	(
	const bool raise
	)
{
	itsRaiseWindowFlag = raise;
}

#endif
