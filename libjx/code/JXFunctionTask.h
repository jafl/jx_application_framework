/******************************************************************************
 JXFunctionTask.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFunctionTask
#define _H_JXFunctionTask

#include "JXIdleTask.h"
#include <functional>

class JString;

class JXFunctionTask : public JXIdleTask
{
public:

	JXFunctionTask(const Time period, const std::function<void()>& f,
				   const JString& name, const bool oneShot = false);

	~JXFunctionTask() override;

	JString	ToString() const override;

protected:

	void	Perform(const Time delta) override;

private:

	std::function<void()>	itsFunction;
	JString*				itsName;
	const bool				itsIsOneShotFlag;

	bool*	itsDeletedFlag;
};

#endif
