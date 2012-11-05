/******************************************************************************
 CBDSSFinishSaveTask.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDSSFinishSaveTask
#define _H_CBDSSFinishSaveTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirectSaveAction.h>

class CBTextDocument;

class CBDSSFinishSaveTask : public JXDirectSaveAction
{
public:

	CBDSSFinishSaveTask(CBTextDocument* doc);

	virtual ~CBDSSFinishSaveTask();

	virtual void	Save(const JString& fullName);

private:

	CBTextDocument*	itsDoc;		// not owned

private:

	// not allowed

	CBDSSFinishSaveTask(const CBDSSFinishSaveTask& source);
	const CBDSSFinishSaveTask& operator=(const CBDSSFinishSaveTask& source);
};

#endif
