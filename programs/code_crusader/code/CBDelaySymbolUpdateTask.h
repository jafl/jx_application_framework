/******************************************************************************
 CBDelaySymbolUpdateTask.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDelaySymbolUpdateTask
#define _H_CBDelaySymbolUpdateTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class CBProjectDocument;

class CBDelaySymbolUpdateTask : public JXIdleTask
{
public:

	CBDelaySymbolUpdateTask(CBProjectDocument* projDoc);

	virtual ~CBDelaySymbolUpdateTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	CBProjectDocument*	itsProjDoc;

private:

	// not allowed

	CBDelaySymbolUpdateTask(const CBDelaySymbolUpdateTask& source);
	const CBDelaySymbolUpdateTask& operator=(const CBDelaySymbolUpdateTask& source);
};

#endif
