/******************************************************************************
 CMGetSourceFileList.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetSourceFileList.h"
#include "CMFileListDir.h"
#include "cmGlobals.h"
#include <JXFileListTable.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetSourceFileList::CMGetSourceFileList
	(
	const JString&	cmd,
	CMFileListDir*	fileList
	)
	:
	CMCommand(cmd, false, false),
	itsFileList(fileList),
	itsNeedRedoOnFirstStop(true)
{
	ListenTo(CMGetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetSourceFileList::~CMGetSourceFileList()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMGetSourceFileList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CMLink* link = CMGetLink();

	if (sender == link && message.Is(CMLink::kPrepareToLoadSymbols))
		{
		(itsFileList->GetTable())->RemoveAllFiles();
		}
	else if (sender == link && message.Is(CMLink::kSymbolsLoaded))
		{
		const auto* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CMCommand::Send();
			itsNeedRedoOnFirstStop = true;
			}
		}
	else if (sender == link && message.Is(CMLink::kSymbolsReloaded))
		{
		CMCommand::Send();
		}
	else if (sender == link && message.Is(CMLink::kProgramFirstStop))
		{
		if (itsNeedRedoOnFirstStop)
			{
			CMCommand::Send();
			itsNeedRedoOnFirstStop = false;
			}
		}
	else if (sender == link &&
			 (message.Is(CMLink::kCoreLoaded) ||
			  message.Is(CMLink::kAttachedToProcess)))
		{
		CMCommand::Send();
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
