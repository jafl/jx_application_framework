/******************************************************************************
 GDBGetAssembly.cpp

	Get the assembly code for the current function.

	BASE CLASS = CMGetAssembly

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GDBGetAssembly.h"
#include "CMSourceDirector.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetAssembly::GDBGetAssembly
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

GDBGetAssembly::~GDBGetAssembly()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
GDBGetAssembly::Starting()
{
	CMGetAssembly::Starting();

	const CMLocation& loc = GetDirector()->GetDisassemblyLocation();

	JString cmd("-data-disassemble");
	if (loc.GetFileName().IsEmpty())
		{
		cmd += " -s ";
		cmd += loc.GetFunctionName();
		}
	else
		{
		JString path, name;
		JSplitPathAndName(loc.GetFileName(), &path, &name);

		cmd += " -f ";
		cmd += name;
		cmd += " -l ";
		cmd += JString((JUInt64) loc.GetLineNumber());
		}

	cmd += " -- 0";
	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex bpPattern     = "asm_insns=\\[";
static const JRegex offsetPattern = "<\\+[[:digit:]]+>$";

void
GDBGetAssembly::HandleSuccess
	(
	const JString& cmdData
	)
{
	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	JString instText;

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JPtrArray< JStringPtrMap<JString> > list(JPtrArrayT::kDeleteAll);
	JSize maxOffsetLength = 0;

	const JStringMatch m = bpPattern.Match(data, JRegex::kIgnoreSubmatches);
	if (!m.IsEmpty())
		{
		stream.seekg(m.GetUtf8ByteRange().last);
		if (!GDBLink::ParseMapArray(stream, &list))
			{
			CMGetLink()->Log("invalid data map");
			}
		else
			{
			const JSize count = list.GetElementCount();
			JString *addr, *offset, *inst;
			for (JIndex i=1; i<=count; i++)
				{
				JStringPtrMap<JString>* map = list.GetElement(i);

				if (!map->GetElement("address", &addr))
					{
					CMGetLink()->Log("invalid assembly instruction: missing address");
					continue;
					}

				if (!map->GetElement("offset", &offset))
					{
					CMGetLink()->Log("invalid assembly instruction: missing address");
					continue;
					}

				if (!map->GetElement("inst", &inst))
					{
					CMGetLink()->Log("invalid assembly instruction: missing inst");
					continue;
					}

				*addr += " <+";
				*addr += *offset;
				*addr += ">";
				addrList.Append(*addr);

				maxOffsetLength = JMax(maxOffsetLength, offset->GetCharacterCount() + 3);

				if (!instText.IsEmpty())
					{
					instText.Append("\n");
					}
				instText.Append(*inst);
				}
			}
		}

	const JSize count = addrList.GetElementCount();
	for (JIndex i=1; i<count; i++)
		{
		JString* s = addrList.GetElement(i);

		const JStringMatch m2 = offsetPattern.Match(*s, JRegex::kIgnoreSubmatches);
		if (!m2.IsEmpty())
			{
			const JSize pad = maxOffsetLength - m2.GetCharacterRange().GetCount();

			JStringIterator iter(s, kJIteratorStartBefore, m2.GetCharacterRange().first + 2);
			for (JUnsignedOffset j=0; j<pad; j++)
				{
				iter.Insert("0");
				}
			}
		}

	GetDirector()->DisplayDisassembly(&addrList, instText);
}
