/******************************************************************************
 GDBGetAssembly.cpp

	Get the assembly code for the current function.

	BASE CLASS = CMGetAssembly

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBGetAssembly.h"
#include "CMSourceDirector.h"
#include "GDBLink.h"
#include "cmGlobals.h"
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
	CMGetAssembly(dir, "")
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

	const CMLocation& loc = (GetDirector())->GetDisassemblyLocation();

	JString cmd = "-data-disassemble";
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
		cmd += JString(loc.GetLineNumber(), JString::kBase10);
		}

	cmd += " -- 0";
	SetCommand(cmd);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex bpPattern = "asm_insns=\\[";

void
GDBGetAssembly::HandleSuccess
	(
	const JString& cmdData
	)
{
	JPtrArray<JString> addrList(JPtrArrayT::kDeleteAll);
	JString instText;

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetCString());

	JIndexRange r;
	JPtrArray< JStringPtrMap<JString> > list(JPtrArrayT::kDeleteAll);
	if (bpPattern.Match(data, &r))
		{
		stream.seekg(r.last);
		if (!GDBLink::ParseMapArray(stream, &list))
			{
			(CMGetLink())->Log("invalid data map");
			}
		else
			{
			const JSize count = list.GetElementCount();
			JString *addr, *inst;
			for (JIndex i=1; i<=count; i++)
				{
				JStringPtrMap<JString>* map = list.NthElement(i);

				if (!map->GetElement("address", &addr))
					{
					(CMGetLink())->Log("invalid assembly instruction: missing address");
					continue;
					}

				if (!map->GetElement("inst", &inst))
					{
					(CMGetLink())->Log("invalid assembly instruction: missing inst");
					continue;
					}

				addrList.Append(*addr);

				if (!instText.IsEmpty())
					{
					instText.AppendCharacter('\n');
					}
				instText.Append(*inst);
				}
			}
		}

	(GetDirector())->DisplayDisassembly(&addrList, instText);
}
