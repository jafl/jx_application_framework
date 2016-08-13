/******************************************************************************
 GDBGetMemory.cpp

	BASE CLASS = CMGetMemory

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetMemory.h"
#include "CMMemoryDir.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetMemory::GDBGetMemory
	(
	CMMemoryDir* dir
	)
	:
	CMGetMemory(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetMemory::~GDBGetMemory()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

static const JCharacter* kCommandName[] =
{
	"xb", "xh", "xw", "xg", "cb", "i"
};

void
GDBGetMemory::Starting()
{
	CMGetMemory::Starting();

	CMMemoryDir::DisplayType type;
	JSize count;
	const JString& expr = GetDirector()->GetExpression(&type, &count);

	JString cmd = "x/";
	cmd        += JString(count, JString::kBase10);
	cmd        += kCommandName[ type-1 ];
	cmd        += " ";
	cmd        += expr;

	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^\\s*0[xX][[:xdigit:]]+(.*):\\s";

void
GDBGetMemory::HandleSuccess
	(
	const JString& data
	)
{
	JString s =  data;

	JIndex i = 1;
	JArray<JIndexRange> matchList;
	while (prefixPattern.MatchFrom(s, i, &matchList))
		{
		s.RemoveSubstring(matchList.GetElement(2));
		i = matchList.GetElement(2).first;
		}

	i = 1;
	while (s.LocateNextSubstring("\t", &i))
		{
		s.SetCharacter(i, ' ');
		}

	GetDirector()->Update(s);
}
