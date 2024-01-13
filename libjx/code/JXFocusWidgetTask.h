/******************************************************************************
 JXFocusWidgetTask.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFocusWidgetTask
#define _H_JXFocusWidgetTask

#include "JXIdleTask.h"

class JXWidget;

class JXFocusWidgetTask : public JXIdleTask
{
public:

	static void	Focus(JXWidget* widget);

protected:

	JXFocusWidgetTask(JXWidget* widget);

	~JXFocusWidgetTask() override;

	void	Perform(const Time delta) override;

private:

	JXWidget* itsWidget;
};

#endif
