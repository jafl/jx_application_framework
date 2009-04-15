/******************************************************************************
 JTextProgressDisplay.h

	Interface for the JTextProgressDisplay class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTextProgressDisplay
#define _H_JTextProgressDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JProgressDisplay.h>
#include <jSignal.h>

class JTextProgressDisplay : public JProgressDisplay
{
public:

	JTextProgressDisplay();

	virtual ~JTextProgressDisplay();

	virtual JBoolean	IncrementProgress(const JCharacter* message = NULL);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta);
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JCharacter* message, const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual JBoolean	CheckForCancel();

private:

	j_sig_func*	itsOldSigIntHandler;

private:

	// not allowed

	JTextProgressDisplay(const JTextProgressDisplay& source);
	const JTextProgressDisplay& operator=(const JTextProgressDisplay& source);
};

#endif
