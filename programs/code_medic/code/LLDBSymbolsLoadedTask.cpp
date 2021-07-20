/******************************************************************************
 LLDBSymbolsLoadedTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBSymbolsLoadedTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBSymbolsLoadedTask::LLDBSymbolsLoadedTask
	(
	const JString& fileName
	)
	:
	itsFileName(fileName)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBSymbolsLoadedTask::~LLDBSymbolsLoadedTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBSymbolsLoadedTask::Perform()
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->SymbolsLoaded(itsFileName);
		}
}
