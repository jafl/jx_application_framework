/******************************************************************************
 LLDBGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetFrame.h"
#include "CMStackWidget.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetFrame::LLDBGetFrame
	(
	CMStackWidget* widget
	)
	:
	CMGetFrame("-stack-info-frame"),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFrame::~LLDBGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern = "\\bframe=\\{";

void
LLDBGetFrame::HandleSuccess
	(
	const JString& cmdData
	)
{
/*
	const JString& data = GetLastResult();

	JIndexRange r;
	if (framePattern.Match(data, &r))
		{
		std::istringstream stream(data.GetCString());
		stream.seekg(r.last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString* s;
		JIndex frameIndex;
		if (!LLDBLink::ParseMap(stream, &map))
			{
			(CMGetLink())->Log("invalid data map");
			}
		else if (!map.GetElement("level", &s))
			{
			(CMGetLink())->Log("missing frame index");
			}
		else if (!s->ConvertToUInt(&frameIndex))
			{
			(CMGetLink())->Log("frame index is not integer");
			}
		else
			{
			itsWidget->SelectFrame(frameIndex);
			}
		}
	else
		{
		(CMGetLink())->Log("LLDBGetFrame failed to match");
		}
*/
}
