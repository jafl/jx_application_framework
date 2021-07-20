/******************************************************************************
 LLDBGetAssembly.cpp

	Get the assembly code for the current function.

	BASE CLASS = CMGetAssembly

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetAssembly.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBCommandReturnObject.h"
#include "CMSourceDirector.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
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
	CMGetAssembly(dir, JString::empty)
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
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
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
	interp.HandleCommand(cmd.GetBytes(), result);

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

			JStringIterator iter(line);
			iter.BeginMatch();
			if (!line.EndsWith(":") && iter.Next(":"))
				{
				s = iter.FinishMatch().GetString();
				if (s.BeginsWith("->") && s.GetCharacterCount() > 2)
					{
					JStringIterator i2(&s);
					i2.RemoveNext(2);
					}
				s.TrimWhitespace();
				addrList.Append(s);

				const JStringMatch m = offsetPattern.Match(s, JRegex::kIgnoreSubmatches);
				if (!m.IsEmpty())
					{
					maxOffsetLength = JMax(maxOffsetLength, m.GetCharacterRange().GetCount());
					}

				if (!instText.IsEmpty())
					{
					instText += "\n";
					}

				iter.BeginMatch();
				iter.MoveTo(kJIteratorStartAtEnd, 0);
				s = iter.FinishMatch().GetString();
				s.TrimWhitespace();
				instText.Append(s);
				}
			}

		const JSize count = addrList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JString* s = addrList.GetElement(i);

			JStringIterator iter(s);
			const JStringMatch m = offsetPattern.Match(*s, JRegex::kIgnoreSubmatches);
			if (iter.Next(offsetPattern))
				{
				const JSize count = iter.GetLastMatch().GetCharacterRange().GetCount();
				iter.SkipPrev(count-2);

				const JSize pad = maxOffsetLength - count;
				for (JUnsignedOffset j=0; j<pad; j++)
					{
					iter.Insert("0");
					}
				}
			}
		}

	GetDirector()->DisplayDisassembly(&addrList, instText);
}
