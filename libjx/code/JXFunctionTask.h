/******************************************************************************
 JXFunctionTask.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFunctionTask
#define _H_JXFunctionTask

#include "JXIdleTask.h"

class JXFunctionTask : public JXIdleTask
{
public:

	JXFunctionTask(const Time period, const std::function<void()>& f,
				   const bool oneShot = false);

	~JXFunctionTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	std::function<void()>	itsFunction;
	const bool				itsIsOneShotFlag;
};

#endif
