/******************************************************************************
 GInboxMgr.cc

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GInboxMgr.h>
#include <GPrefsMgr.h>
#include <GMApp.h>

#include <JXTimerTask.h>
#include <JXDisplay.h>

#include <JDirEntry.h>
#include <JMinMax.h>

#include <GMGlobals.h>

#include <jDirUtil.h>
#include <jFileUtil.h>

#include <jAssert.h>

const Time kTimerDelay = 5000;

/******************************************************************************
 Constructor

 *****************************************************************************/

GInboxMgr::GInboxMgr()
{
	itsEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert(itsEntries != NULL);

	itsTimes = new JArray<time_t>;
	assert(itsTimes != NULL);

	UpdateEntries();

	itsTimer = new JXTimerTask(kTimerDelay);
	assert(itsTimer != NULL);

	itsTimer->Start();
	ListenTo(itsTimer);

	ListenTo(GGetPrefsMgr());
	ListenTo(GMGetApplication());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GInboxMgr::~GInboxMgr()
{
	itsEntries->DeleteAll();
	delete itsEntries;
	delete itsTimes;
	delete itsTimer;
}

/******************************************************************************
 Receive

 *****************************************************************************/

void
GInboxMgr::Receive
	(
	JBroadcaster*					sender,
	const JBroadcaster::Message&	message
	)
{
	if (sender == itsTimer && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (!itsPostponeUpdate)
			{
			CheckEntries();
			}		
		}
	else if (sender == GGetPrefsMgr() && message.Is(GPrefsMgr::kInboxAdded))
		{
		const GPrefsMgr::InboxAdded* info =
			dynamic_cast<const GPrefsMgr::InboxAdded*>(&message);
		assert(info != NULL);
		JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
		GGetPrefsMgr()->GetInboxes(inboxes);
		JIndex index = info->GetIndex();
		assert(inboxes.IndexValid(index));
		JDirEntry* entry = new JDirEntry(*(inboxes.NthElement(index)));
		assert(entry != NULL);
		itsEntries->InsertAtIndex(index, entry);
		itsTimes->InsertElementAtIndex(index, entry->GetModTime());
		inboxes.DeleteAll();
		}
	else if (sender == GGetPrefsMgr() && message.Is(GPrefsMgr::kInboxRemoved))
		{
		const GPrefsMgr::InboxRemoved* info =
			dynamic_cast<const GPrefsMgr::InboxRemoved*>(&message);
		assert(info != NULL);
		JIndex index = info->GetIndex();
		assert(itsEntries->IndexValid(index));
		itsEntries->DeleteElement(index);
		assert(itsTimes->IndexValid(index));
		itsTimes->RemoveElement(index);
		}
	else if (sender == GMGetApplication() && message.Is(GMApp::kMailboxClosed))
		{
		const GMApp::MailboxClosed* info =
			dynamic_cast<const GMApp::MailboxClosed*>(&message);
		assert(info != NULL);
		MailboxClosed(info->GetMailbox());
		}
}

/******************************************************************************
 UpdateEntries

 *****************************************************************************/

void
GInboxMgr::UpdateEntries()
{
	itsEntries->DeleteAll();
	itsTimes->RemoveAll();

	JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
	GGetPrefsMgr()->GetInboxes(inboxes);

	for (JSize i = 1; i <= inboxes.GetElementCount(); i++)
		{
		JDirEntry* entry = new JDirEntry(*(inboxes.NthElement(i)));
		assert(entry != NULL);
		itsEntries->Append(entry);
		itsTimes->AppendElement(entry->GetModTime());
		}
	inboxes.DeleteAll();
}

/******************************************************************************
 CheckEntries

 *****************************************************************************/

void
GInboxMgr::CheckEntries()
{
	for (JSize i = 1; i <= itsEntries->GetElementCount(); i++)
		{
		JDirEntry* entry = itsEntries->NthElement(i);
		if (entry->Update())
			{
			entry->ForceUpdate();
			}
		time_t newTime = entry->GetModTime();
		time_t oldTime = itsTimes->GetElement(i);
		if (newTime != oldTime)
			{
			if ((newTime != oldTime) && (JFileExists(entry->GetFullName())))
				{
				GMGetApplication()->OpenIconifiedMailbox(entry->GetFullName());
				}
			itsTimes->SetElement(i, newTime);
			}
		}
}

/******************************************************************************
 SaveState

 *****************************************************************************/

void
GInboxMgr::SaveState
	(
	ostream& os
	)
{
	JSize count = itsTimes->GetElementCount();
	os << count << " ";
	for (JSize i = 1; i <= count; i++)
		{
		os << itsTimes->GetElement(i) << " ";
		}
}

/******************************************************************************
 ReadState

 *****************************************************************************/

void
GInboxMgr::ReadState
	(
	istream& is
	)
{
	JSize pcount;
	is >> pcount;
	JSize tcount = itsTimes->GetElementCount();
	JSize count = JMin(pcount, tcount);
	assert(count <= tcount);
	for (JSize i = 1; i <= count; i++)
		{
		time_t currentTime = itsTimes->GetElement(i);
		time_t time;
		is >> time;
		if (currentTime > time)
			{
			JDirEntry* entry = itsEntries->NthElement(i);
			if (JFileExists(entry->GetFullName()))
				{
				GMGetApplication()->OpenIconifiedMailbox(entry->GetFullName(), kJFalse);
				}
			}
		}
}

/******************************************************************************
 MailboxClosed

 *****************************************************************************/

void
GInboxMgr::MailboxClosed
	(
	const JString& mailbox
	)
{
	JString trueMBox;
	JGetTrueName(mailbox, &trueMBox);
	for (JSize i = 1; i <= itsEntries->GetElementCount(); i++)
		{
		JDirEntry* entry = itsEntries->NthElement(i);
		JString trueFN;
		JGetTrueName(entry->GetFullName(), &trueFN);
		if (trueFN == trueMBox)
			{
			entry->Update();
			time_t newTime = entry->GetModTime();
			time_t oldTime = itsTimes->GetElement(i);
			if (newTime != oldTime)
				{
				itsTimes->SetElement(i, newTime);
				}
			}
		}

}

/******************************************************************************
 ShouldBePostponingUpdate

 *****************************************************************************/

void
GInboxMgr::ShouldBePostponingUpdate
	(
	const JBoolean postpone
	)
{
	itsPostponeUpdate	= postpone;
}
