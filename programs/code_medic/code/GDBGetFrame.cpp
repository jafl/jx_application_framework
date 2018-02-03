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
	CMGetFrame("-stack-info-frame"),
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

	JIndexRange r;
	if (framePattern.Match(data, &r))
		{
		std::istringstream stream(data.GetCString());
		stream.seekg(r.last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString* s;
		JIndex frameIndex;
		if (!GDBLink::ParseMap(stream, &map))
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
		(CMGetLink())->Log("GDBGetFrame failed to match");
		}
}
