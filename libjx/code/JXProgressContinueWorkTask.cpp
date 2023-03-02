/******************************************************************************
 JXProgressContinueWorkTask.cpp

	Gives time to the working fiber when there are no events to process.

	BASE CLASS = JXIdleTask

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "JXProgressContinueWorkTask.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXProgressContinueWorkTask::JXProgressContinueWorkTask
	(
	boost::fibers::condition_variable*	condition,
	boost::fibers::mutex*				mutex,
	bool*								continueFlag
	)
	:
	JXIdleTask(0),
	itsCondition(condition),
	itsMutex(mutex),
	itsContinueFlag(continueFlag)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXProgressContinueWorkTask::~JXProgressContinueWorkTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXProgressContinueWorkTask::Perform
	(
	const Time delta
	)
{
	std::unique_lock lock(*itsMutex);
	*itsContinueFlag = true;
	lock.unlock();

	itsCondition->notify_one();
	boost::this_fiber::yield();
}
