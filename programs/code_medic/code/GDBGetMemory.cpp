/******************************************************************************
 GDBGetMemory.cpp

	BASE CLASS = CMGetMemory

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GDBGetMemory.h"
#include "CMMemoryDir.h"
#include <JStringIterator.h>
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

static const JUtf8Byte* kCommandName[] =
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

	JString cmd("x/");
	cmd += JString((JUInt64) count);
	cmd += kCommandName[ type-1 ];
	cmd += " ";
	cmd += expr;

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
	JString s = data;

	JStringIterator iter(&s);
	while (iter.Next(prefixPattern))
		{
		const JStringMatch& m = iter.GetLastMatch();

		const JSize count = m.GetCharacterRange(1).GetCount();
		iter.SkipPrev(count + 2);
		iter.RemoveNext(count);
		}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next("\t"))
		{
		iter.SetPrev(JUtf8Character(' '));
		}

	GetDirector()->Update(s);
}
