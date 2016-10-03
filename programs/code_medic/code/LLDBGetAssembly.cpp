/******************************************************************************
 LLDBGetAssembly.cpp

	Get the assembly code for the current function.

	BASE CLASS = CMGetAssembly

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBGetAssembly.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "CMSourceDirector.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetAssembly::LLDBGetAssembly
	(
	CMSourceDirector* dir
	)
	:
	CMGetAssembly(dir, "")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetAssembly::~LLDBGetAssembly()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex offsetPattern = "<\\+[[:digit:]]+>$";

void
LLDBGetAssembly::HandleSuccess
	(
	const JString& cmdData
	)
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
		{
		return;
		}

	const CMLocation& loc = GetDirector()->GetDisassemblyLocation();

	const JString cmd = "disassemble -n " + JPrepArgForExec(loc.GetFunctionName());
	lldb::SBCommandReturnObject result;
	interp.HandleCommand(cmd, result);

	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	JString instText;

	if (result.IsValid() && result.Succeeded() && result.HasResult())
		{
		std::istringstream input(result.GetOutput());
		JString line, s;
		JSize maxOffsetLength = 0;
		while (!input.eof() && !input.fail())
			{
			line = JReadLine(input);

			JIndex i;
			if (line.LocateSubstring(":", &i) && i < line.GetLength())
				{
				s = line.GetSubstring(1, i-1);
				if (s.BeginsWith("->") && s.GetLength() > 2)
					{
					s = s.GetSubstring(3, s.GetLength());
					}
				s.TrimWhitespace();
				addrList.Append(s);

				JIndexRange r;
				if (offsetPattern.Match(s, &r))
					{
					maxOffsetLength = JMax(maxOffsetLength, r.GetLength());
					}

				if (!instText.IsEmpty())
					{
					instText.AppendCharacter('\n');
					}
				s = line.GetSubstring(i+1, line.GetLength());
				s.TrimWhitespace();
				instText.Append(s);
				}
			}

		const JSize count = addrList.GetElementCount();
		for (JIndex i=1; i<count; i++)
			{
			JString* s = addrList.NthElement(i);
			JIndexRange r;
			if (offsetPattern.Match(*s, &r))
				{
				const JSize pad = maxOffsetLength - r.GetLength();
				for (JIndex j=0; j<pad; j++)
					{
					s->InsertCharacter('0', r.first+2);
					}
				}
			}
		}

	GetDirector()->DisplayDisassembly(&addrList, instText);
}
