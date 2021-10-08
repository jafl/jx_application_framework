/******************************************************************************
 JXTEBlinkCaretTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTEBlinkCaretTask
#define _H_JXTEBlinkCaretTask

#include "jx-af/jx/JXIdleTask.h"

class JXTEBase;

class JXTEBlinkCaretTask : public JXIdleTask
{
public:

	JXTEBlinkCaretTask(JXTEBase* te);

	~JXTEBlinkCaretTask() override;

	void	ShouldBlink(const bool blink);
	void	Reset();
	void	Perform(const Time delta, Time* maxSleepTime) override;

private:

	JXTEBase*	itsTE;			// owns us
	bool		itsActiveFlag;
};


/******************************************************************************
 ShouldBlink

 ******************************************************************************/

inline void
JXTEBlinkCaretTask::ShouldBlink
	(
	const bool blink
	)
{
	itsActiveFlag = blink;
	Reset();
}

#endif
