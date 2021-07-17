/******************************************************************************
 JTextProgressDisplay.h

	Interface for the JTextProgressDisplay class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextProgressDisplay
#define _H_JTextProgressDisplay

#include "JProgressDisplay.h"
#include "jSignal.h"

class JTextProgressDisplay : public JProgressDisplay
{
public:

	JTextProgressDisplay();

	virtual ~JTextProgressDisplay();

	virtual bool	IncrementProgress(const JString& message = JString::empty);
	virtual bool	IncrementProgress(const JSize delta);
	virtual bool	IncrementProgress(const JString& message,
										  const JSize delta);
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool allowBackground);

	virtual bool	CheckForCancel();

private:

	j_sig_func*	itsOldSigIntHandler;

private:

	// not allowed

	JTextProgressDisplay(const JTextProgressDisplay& source);
	const JTextProgressDisplay& operator=(const JTextProgressDisplay& source);
};

#endif
