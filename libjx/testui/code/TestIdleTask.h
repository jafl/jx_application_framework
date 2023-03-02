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

protected:

	void	Perform(const Time delta) override;

private:

	JSize	itsCounter;
};

#endif
