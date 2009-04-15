/******************************************************************************
 JXStandAlonePG.h

	Interface for the JXStandAlonePG class.

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXStandAlonePG
#define _H_JXStandAlonePG

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXProgressDisplay.h>

class JXPGDirectorBase;

class JXStandAlonePG : public JXProgressDisplay
{
public:

	JXStandAlonePG();

	virtual ~JXStandAlonePG();

	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();

	void	RaiseWhenUpdate(const JBoolean raise = kJTrue);

protected:

	virtual void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JCharacter* message,
									 const JBoolean allowCancel,
									 const JBoolean allowBackground);

private:

	JIndex				itsWindowIndex;
	JXPGDirectorBase*	itsProgressDirector;
	JBoolean			itsRaiseWindowFlag;
	JSize				itsStepCount;

private:

	// not allowed

	JXStandAlonePG(const JXStandAlonePG& source);
	const JXStandAlonePG& operator=(const JXStandAlonePG& source);
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
	const JBoolean raise
	)
{
	itsRaiseWindowFlag = raise;
}

#endif
