/******************************************************************************
 JXTEBlinkCaretTask.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTEBlinkCaretTask
#define _H_JXTEBlinkCaretTask

#include <JXIdleTask.h>

class JXTEBase;

class JXTEBlinkCaretTask : public JXIdleTask
{
public:

	JXTEBlinkCaretTask(JXTEBase* te);

	virtual ~JXTEBlinkCaretTask();

	void			ShouldBlink(const JBoolean blink);
	void			Reset();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXTEBase*	itsTE;			// owns us
	JBoolean	itsActiveFlag;

private:

	// not allowed

	JXTEBlinkCaretTask(const JXTEBlinkCaretTask& source);
	const JXTEBlinkCaretTask& operator=(const JXTEBlinkCaretTask& source);
};


/******************************************************************************
 ShouldBlink

 ******************************************************************************/

inline void
JXTEBlinkCaretTask::ShouldBlink
	(
	const JBoolean blink
	)
{
	itsActiveFlag = blink;
	Reset();
}

#endif
