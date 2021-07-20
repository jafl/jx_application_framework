/******************************************************************************
 LLDBDisplaySourceForMain.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = CMDisplaySourceForMain

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBDisplaySourceForMain.h"
#include "CMSourceDirector.h"
#include "LLDBLink.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBSymbolContextList.h"
#include "lldb/API/SBSymbolContext.h"
#include "lldb/API/SBCompileUnit.h"
#include "lldb/API/SBFileSpec.h"
#include "cmGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBDisplaySourceForMain::LLDBDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
	:
	CMDisplaySourceForMain(sourceDir, JString::empty)
{
	ListenTo(CMGetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBDisplaySourceForMain::~LLDBDisplaySourceForMain()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
LLDBDisplaySourceForMain::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CMGetLink() && message.Is(CMLink::kSymbolsLoaded))
		{
		const auto* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CMCommand::Send();
			}
		}
	else
		{
		CMDisplaySourceForMain::Receive(sender, message);
		}
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
	lldb::SBTarget t = dynamic_cast<LLDBLink*>(CMGetLink())->GetDebugger()->GetSelectedTarget();
	bool found   = false;
	if (t.IsValid())
		{
		lldb::SBSymbolContextList list = t.FindFunctions("main");
		if (list.IsValid() && list.GetSize() == 1)
			{
			lldb::SBSymbolContext ctx = list.GetContextAtIndex(0);
			lldb::SBCompileUnit unit  = ctx.GetCompileUnit();
			lldb::SBFileSpec file     = unit.GetFileSpec();
			if (unit.IsValid() && file.IsValid())
				{
				// sym.GetLineEntry() is not valid -- we need to search

				JIndex line = 1;

				const lldb::addr_t target = ctx.GetSymbol().GetStartAddress().GetFileAddress();
				const JSize count         = unit.GetNumLineEntries();
				for (JUnsignedOffset i=0; i<count; i++)
					{
					lldb::SBLineEntry e = unit.GetLineEntryAtIndex(i);
					if (e.GetStartAddress().GetFileAddress() == target)
						{
						line = e.GetLine();
						break;
						}
					}

				JString fullName = JCombinePathAndName(
					JString(file.GetDirectory(), JString::kNoCopy),
					JString(file.GetFilename(), JString::kNoCopy));

				GetSourceDir()->DisplayFile(fullName, line, false);
				found = true;
				}
			}
		}


	if (!found)
		{
		CMGetLink()->NotifyUser(JGetString("CannotFindMain::LLDBDisplaySourceForMain"), true);
		CMGetLink()->Log("LLDBDisplaySourceForMain failed to find 'main'");

		GetSourceDir()->ClearDisplay();
		}
}
