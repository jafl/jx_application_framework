/******************************************************************************
 LLDBSymbolsLoadedTask.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBSymbolsLoadedTask
#define _H_LLDBSymbolsLoadedTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>
#include <JString.h>

class LLDBSymbolsLoadedTask : public JXUrgentTask
{
public:

	LLDBSymbolsLoadedTask(const JCharacter* fileName);

	virtual ~LLDBSymbolsLoadedTask();

	virtual void	Perform();

private:

	JString	itsFileName;

private:

	// not allowed

	LLDBSymbolsLoadedTask(const LLDBSymbolsLoadedTask& source);
	const LLDBSymbolsLoadedTask& operator=(const LLDBSymbolsLoadedTask& source);
};

#endif
