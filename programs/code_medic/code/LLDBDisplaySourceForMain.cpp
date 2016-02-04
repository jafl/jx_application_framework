/******************************************************************************
 LLDBDisplaySourceForMain.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = CMDisplaySourceForMain

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
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
	CMDisplaySourceForMain(sourceDir, "")
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
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
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
	JBoolean found   = kJFalse;
	if (t.IsValid())
		{
		lldb::SBSymbolContextList list = t.FindFunctions("main");
		if (list.IsValid() && list.GetSize() == 1)
			{
			lldb::SBCompileUnit unit = list.GetContextAtIndex(0).GetCompileUnit();
			lldb::SBFileSpec file    = unit.GetFileSpec();
			if (unit.IsValid() && file.IsValid())
				{
				// sym.GetLineEntry() is not valid
				JString fullName = JCombinePathAndName(file.GetDirectory(), file.GetFilename());
				(GetSourceDir())->DisplayFile(fullName, 1, kJFalse);
				found = kJTrue;
				}
			}
		}


	if (!found)
		{
		(CMGetLink())->Log("LLDBDisplaySourceForMain failed to find 'main'");

		(GetSourceDir())->ClearDisplay();
		}
}
