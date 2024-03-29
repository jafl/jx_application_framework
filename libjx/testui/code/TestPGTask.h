/******************************************************************************
 TestPGTask.h

	Interface for the TestPGTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPGTask
#define _H_TestPGTask

#include <jx-af/jx/JXIdleTask.h>

class JProgressDisplay;

class TestPGTask : public JXIdleTask
{
public:

	TestPGTask(const bool fixedLength);

	~TestPGTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	JProgressDisplay*	itsPG;
	JIndex				itsCounter;
};

#endif
