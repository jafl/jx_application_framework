/******************************************************************************
 JXQuitIfAllDeactTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXQuitIfAllDeactTask
#define _H_JXQuitIfAllDeactTask

#include "JXIdleTask.h"

class JXQuitIfAllDeactTask : public JXIdleTask
{
public:

	JXQuitIfAllDeactTask();

	~JXQuitIfAllDeactTask() override;

protected:

	void	Perform(const Time delta) override;
};

#endif
