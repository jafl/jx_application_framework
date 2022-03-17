/******************************************************************************
 TestIdleTask.h

	Interface for the TestIdleTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestIdleTask
#define _H_TestIdleTask

#include <jx-af/jx/JXIdleTask.h>

class TestIdleTask : public JXIdleTask
{
public:

	TestIdleTask();

	void	Perform(const Time delta, Time* maxSleepTime) override;

private:

	JSize	itsCounter;
};

#endif
