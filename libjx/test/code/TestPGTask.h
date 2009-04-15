/******************************************************************************
 TestPGTask.h

	Interface for the TestPGTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPGTask
#define _H_TestPGTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class JProgressDisplay;

class TestPGTask : public JXIdleTask
{
public:

	TestPGTask(const JBoolean fixedLength);

	virtual ~TestPGTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JProgressDisplay*	itsPG;
	JIndex				itsCounter;

private:

	// not allowed

	TestPGTask(const TestPGTask& source);
	const TestPGTask& operator=(const TestPGTask& source);
};

#endif
