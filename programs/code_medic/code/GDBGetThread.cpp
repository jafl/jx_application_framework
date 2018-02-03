/******************************************************************************
 GDBGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetThread.h"
#include "CMThreadsWidget.h"
#include "GDBGetThreads.h"
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetThread::GDBGetThread
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

GDBGetThread::~GDBGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetThread::HandleSuccess
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
			if (GDBGetThreads::ExtractThreadIndex(line, &currentThreadIndex))
				{
				break;
				}
			}
		}

	GetWidget()->FinishedLoading(currentThreadIndex);
}
