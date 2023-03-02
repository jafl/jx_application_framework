/******************************************************************************
 JXProgressContinueWorkTask.h

	Interface for the JXProgressContinueWorkTask class

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXProgressContinueWorkTask
#define _H_JXProgressContinueWorkTask

#include "JXIdleTask.h"
#include <boost/fiber/condition_variable.hpp>

class JXProgressContinueWorkTask : public JXIdleTask
{
public:

	JXProgressContinueWorkTask(boost::fibers::condition_variable* condition,
							   boost::fibers::mutex* mutex,
							   bool* continueFlag);

	~JXProgressContinueWorkTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	// owned by JXStandAlonePG

	boost::fibers::condition_variable*	itsCondition;
	boost::fibers::mutex*				itsMutex;
	bool*								itsContinueFlag;
};

#endif
