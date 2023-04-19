/******************************************************************************
 JXUrgentFunctionTask.h

	Copyright (C) 2023 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUrgentFunctionTask
#define _H_JXUrgentFunctionTask

#include "JXUrgentTask.h"

class JXUrgentFunctionTask : public JXUrgentTask
{
public:

	JXUrgentFunctionTask(JBroadcaster* target,
						 const std::function<void()>& f);

protected:

	~JXUrgentFunctionTask() override;

	void	Perform() override;

private:

	std::function<void()>	itsFunction;
};

#endif
