/******************************************************************************
 JTextProgressDisplay.h

	Interface for the JTextProgressDisplay class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextProgressDisplay
#define _H_JTextProgressDisplay

#include "jx-af/jcore/JProgressDisplay.h"
#include "jx-af/jcore/jSignal.h"

class JTextProgressDisplay : public JProgressDisplay
{
public:

	JTextProgressDisplay();

	~JTextProgressDisplay();

	bool	IncrementProgress(const JString& message = JString::empty) override;
	bool	IncrementProgress(const JSize delta) override;
	bool	IncrementProgress(const JString& message, const JSize delta) override;
	void	ProcessFinished() override;
	void	DisplayBusyCursor() override;

protected:

	void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
							 const JString& message, const bool allowCancel,
							 const bool allowBackground) override;

	bool	CheckForCancel() override;

private:

	j_sig_func	itsOldSigIntHandler;
};

#endif
