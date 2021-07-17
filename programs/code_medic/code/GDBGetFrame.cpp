/******************************************************************************
 GDBGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetFrame.h"
#include "CMStackWidget.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetFrame::GDBGetFrame
	(
	CMStackWidget* widget
	)
	:
	CMGetFrame(JString("-stack-info-frame", JString::kNoCopy)),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetFrame::~GDBGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern = "\\bframe=\\{";

void
GDBGetFrame::HandleSuccess
	(
	const JString& cmdData
	)
{
	const JString& data = GetLastResult();

	const JStringMatch m = framePattern.Match(data, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
		{
		std::istringstream stream(data.GetBytes());
		stream.seekg(m.GetUtf8ByteRange().last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString* s;
		JIndex frameIndex;
		if (!GDBLink::ParseMap(stream, &map))
			{
			CMGetLink()->Log("invalid data map");
			}
		else if (!map.GetElement("level", &s))
			{
			CMGetLink()->Log("missing frame index");
			}
		else if (!s->ConvertToUInt(&frameIndex))
			{
			CMGetLink()->Log("frame index is not integer");
			}
		else
			{
			itsWidget->SelectFrame(frameIndex);
			}
		}
	else
		{
		CMGetLink()->Log("GDBGetFrame failed to match");
		}
}
