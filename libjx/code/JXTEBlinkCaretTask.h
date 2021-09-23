/******************************************************************************
 JXTEBlinkCaretTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTEBlinkCaretTask
#define _H_JXTEBlinkCaretTask

#include "JXIdleTask.h"

class JXTEBase;

class JXTEBlinkCaretTask : public JXIdleTask
{
public:

	JXTEBlinkCaretTask(JXTEBase* te);

	virtual ~JXTEBlinkCaretTask();

	void			ShouldBlink(const bool blink);
	void			Reset();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

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
