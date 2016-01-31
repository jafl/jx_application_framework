/******************************************************************************
 LLDBGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetThread.h"
#include "CMThreadsWidget.h"
#include "LLDBGetThreads.h"
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThread::LLDBGetThread
	(
	CMThreadsWidget* widget
	)
	:
	CMGetThread("set width 0\ninfo threads", widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetThread::~LLDBGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetThread::HandleSuccess
	(
	const JString& data
	)
{
	JIndex currentThreadIndex = 0;

	const std::string s(data.GetCString(), data.GetLength());
	std::istringstream input(s);
	JString line;
	while (1)
		{
		line = JReadLine(input);
		if (input.eof() || input.fail())
			{
			break;
			}

		line.TrimWhitespace();
		if (!line.IsEmpty() && line.GetFirstCharacter() == '*')
			{
			line.RemoveSubstring(1,1);
			line.TrimWhitespace();
			if (LLDBGetThreads::ExtractThreadIndex(line, &currentThreadIndex))
				{
				break;
				}
			}
		}

	(GetWidget())->FinishedLoading(currentThreadIndex);
}
