/******************************************************************************
 GMMailboxData.cc

	<Description>

	BASE CLASS = public JBroadcaster

	Copyright © 2003 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GMMailboxData.h>
#include "GMessageHeaderList.h"
#include "GMessageHeader.h"
#include "GMessageFrom.h"
#include "GMessageDataUpdateTask.h"
#include "GPrefsMgr.h"
#include "GMApp.h"
#include "GMGlobals.h"
#include "gMailUtils.h"

#include <JError.h>
#include <JLatentPG.h>
#include <JProgressDisplay.h>
#include <JString.h>
#include <JDirInfo.h>

#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JCharacter* GMMailboxData::kStartingUpdate		= "kStartingUpdate::GMMailboxData";
const JCharacter* GMMailboxData::kUpdateFinished		= "kUpdateFinished::GMMailboxData";
const JCharacter* GMMailboxData::kNewMail				= "kNewMail::GMMailboxData";
const JCharacter* GMMailboxData::kMBoxState				= "kMBoxState::GMMailboxData";
const JCharacter* GMMailboxData::kHeaderRemoved			= "kHeaderRemoved::GMMailboxData";
const JCharacter* GMMailboxData::kHeaderAdded			= "kHeaderAdded::GMMailboxData";
const JCharacter* GMMailboxData::kDataReverted			= "kDataReverted::GMMailboxData";
const JCharacter* GMMailboxData::kDataModified			= "kDataModified::GMMailboxData";
const JCharacter* GMMailboxData::kHeaderMarkedDeleted	= "kHeaderMarkedDeleted::GMMailboxData";
const JCharacter* GMMailboxData::kSortChanged			= "kSortChanged::GMMailboxData";
const JCharacter* GMMailboxData::kHeaderStatusChanged	= "kHeaderStatusChanged::GMMailboxData";


/******************************************************************************
 Constructor

 *****************************************************************************/

GMMailboxData::GMMailboxData
	(
	const JCharacter*	mailfile,
	GMessageHeaderList*	list,
	JProgressDisplay* 	pg
	)
	:
	itsMailFile(mailfile),
	itsList(list),
	itsPG(pg),
	itsParent(NULL)
{
	if (!itsMailFile.BeginsWith("/"))
		{
		const JString cwd = JGetCurrentDirectory();
		itsMailFile = JCombinePathAndName(cwd, itsMailFile);
		}
		
	itsEntry = new JDirEntry(mailfile);
	assert(itsEntry != NULL);

	itsFromList = new JPtrArray<GMessageFrom>(JPtrArrayT::kForgetAll);
	assert(itsFromList != NULL);
	itsFromList->SetCompareFunction(GMessageFrom::CompareFromLines);

	itsIdleTask = new GMessageDataUpdateTask(this);
	assert(itsIdleTask != NULL);
	JXGetApplication()->InstallIdleTask(itsIdleTask);
	GMessageDataUpdateTask::StopUpdate(kJFalse);

	AdjustList(list);
	GenerateFromList();

	GMMailboxDataX();
}

GMMailboxData::GMMailboxData
	(
	GMMailboxData* 		parent,
	GMessageHeaderList*	list
	)
	:
	itsList(list),
	itsParent(parent)
{		
	itsEntry 			= NULL;
	itsFromList 		= NULL;
	itsIdleTask			= NULL;

	GMMailboxDataX();
	
	ListenTo(itsParent);
}

/******************************************************************************
 GMMailboxDataX

 *****************************************************************************/

void	
GMMailboxData::GMMailboxDataX()
{
	itsNeedsUpdate  	= kJFalse;
	itsNeedsSave		= kJFalse;
	itsHasNewMail		= kJFalse;
	itsPostponeUpdate	= kJFalse;
	itsSortType			= kNone;
	itsSortIsAscending	= kJTrue;

	itsListByDate		= new GMessageHeaderList();
	assert(itsListByDate != NULL);
	itsListBySize		= new GMessageHeaderList();
	assert(itsListBySize != NULL);
	itsListBySubject	= new GMessageHeaderList();
	assert(itsListBySubject != NULL);
	itsListByFrom		= new GMessageHeaderList();
	assert(itsListByFrom != NULL);

	itsListByDate->SetCompareFunction(GMessageHeader::CompareDates);
	itsListBySize->SetCompareFunction(GMessageHeader::CompareSize);
	itsListBySubject->SetCompareFunction(GMessageHeader::CompareSubject);
	itsListByFrom->SetCompareFunction(GMessageHeader::CompareFrom);

	SyncLists();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMMailboxData::~GMMailboxData()
{
	delete itsEntry;
	JXGetApplication()->RemoveIdleTask(itsIdleTask);
	delete itsIdleTask;
	if (itsFromList != NULL)
		{
		itsFromList->DeleteAll();
		}	
	delete itsFromList;
	if (itsParent == NULL &&
		itsList != NULL)
		{
		itsList->DeleteAll();
		}
	delete itsList;
	delete itsListByDate;
	delete itsListBySize;
	delete itsListBySubject;
	delete itsListByFrom;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
GMMailboxData::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsParent)
		{
		if (message.Is(GMMailboxData::kHeaderRemoved))
			{
			const GMMailboxData::HeaderRemoved* info = 
				dynamic_cast(const GMMailboxData::HeaderRemoved*, &message);
			assert(info != NULL);
			RemoveHeader(info->GetHeader());
			}
		else if (message.Is(GMMailboxData::kHeaderMarkedDeleted))
			{
			const GMMailboxData::HeaderMarkedDeleted* info = 
				dynamic_cast(const GMMailboxData::HeaderMarkedDeleted*, &message);
			assert(info != NULL);
			Broadcast(HeaderMarkedDeleted(info->GetHeader()));
			}
		else if (message.Is(GMMailboxData::kHeaderStatusChanged))
			{
			const GMMailboxData::HeaderStatusChanged* info = 
				dynamic_cast(const GMMailboxData::HeaderStatusChanged*, &message);
			assert(info != NULL);
			Broadcast(HeaderStatusChanged(info->GetHeader(), info->GetStatus()));
			}
		}
}

/******************************************************************************
 Update

 ******************************************************************************/

JBoolean
GMMailboxData::Update
	(
	const JBoolean unlock,
	const JBoolean alreadyLocked
	)
{
	time_t oldTime		= itsEntry->GetModTime();
	JBoolean updated	= itsEntry->Update();

	JBoolean exists		= JI2B(itsEntry->GetType() != JDirEntry::kDoesNotExist);
	Broadcast(MBoxState(exists));
	
	JBoolean modified	= JI2B(oldTime	!= itsEntry->GetModTime());

	if (modified || itsNeedsUpdate)
		{
		if (!itsNeedsUpdate)
			{
			Broadcast(NewMail(modified));
			itsHasNewMail = kJTrue;
			}
		if (!itsPostponeUpdate)
			{
			Broadcast(StartingUpdate());
			
			GMessageHeaderList* list;
			if (!alreadyLocked)
				{
				GBlockUntilFileUnlocked(itsMailFile);
				while (!GLockFile(itsMailFile))
					{
					// wait until we get a lock.
					}
				}
			JBoolean ok = GMessageHeaderList::Create(itsMailFile, &list, itsPG);

			if (ok)
				{
				AdjustList(list);
				JPtrArray<GMessageFrom> fromlist(JPtrArrayT::kDeleteAll);
				JSize count = list->GetElementCount();
				JSize index;
				for (index = 1; index <= count; index++)
					{
					GMessageHeader* header = list->NthElement(index);
					GMessageFrom* from = new GMessageFrom(header);
					assert(from != NULL);
					fromlist.Append(from);
					}
				count = fromlist.GetElementCount();
				for (index = count; index >= 1; index--)
					{
					GMessageFrom* from = fromlist.NthElement(index);
					JIndex findindex;
					if (itsFromList->SearchSorted(from, JOrderedSetT::kFirstMatch, &findindex))
						{
						GMessageFrom* currentfrom = itsFromList->NthElement(findindex);
						GMessageHeader* header = currentfrom->GetHeader();
						GMessageHeader* header2 = from->GetHeader();
						header->SetHeaderStart(header2->GetHeaderStart());
						header->SetHeaderEnd(header2->GetHeaderEnd());
						header->SetMessageEnd(header2->GetMessageEnd());
						list->Remove(header2);
						fromlist.Remove(from);
						itsFromList->Remove(currentfrom);
						delete header2;
						delete from;
						delete currentfrom;
						}
					}
				count = itsFromList->GetElementCount();
				for (index = count; index >= 1; index--)
					{
					GMessageFrom* from = itsFromList->NthElement(index);
					GMessageHeader* header = from->GetHeader();
					RemoveHeader(header);
					itsFromList->Remove(from);
					delete header;
					delete from;
					}
				count = fromlist.GetElementCount();
				for (index = 1; index <= count; index++)
					{
					GMessageFrom* from = fromlist.NthElement(index);
					GMessageHeader* header = from->GetHeader();
					itsList->Append(header);
					HeaderAppended(header);
					}
				delete list;
				}

			GenerateFromList();

			if (unlock)
				{
				GUnlockFile(itsMailFile);
				}

			itsHasNewMail 	= kJFalse;
			itsNeedsUpdate  = kJFalse;

			Broadcast(UpdateFinished());
			return kJTrue;
			}
		else
			{
			itsNeedsUpdate = kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 Save

 ******************************************************************************/

void
GMMailboxData::Save()
{
	if (!itsNeedsSave || !JFileExists(itsMailFile))
		{
		return;
		}
//	GBlockUntilFileUnlocked(itsMailFile);
//	if (!GLockFile(itsMailFile))
//		{
//		return;
//		}

	JString path, name;
	JSplitPathAndName(itsMailFile, &path, &name);
	JString tempname;
	JError err = JCreateTempFile(path, NULL, &tempname);
	if (!err.OK())
		{
		err = JCreateTempFile(&tempname);
		if (!err.OK())
			{
			JString report = "I was unable to create a temporary file, so the changes can not be saved.\n\n";
			report        += err.GetMessage();
			JGetUserNotification()->ReportError(report);
			return;
			}
		}

	mode_t perms;
	err = JGetPermissions(itsMailFile, &perms);
	if (!err.OK())
		{
		perms = 0600;
		}
	Update(kJFalse);
	if (!GFileLocked(itsMailFile))
		{
		GLockFile(itsMailFile);
		}
	ifstream is(itsMailFile);
	assert(is.good());
	ofstream os(tempname);
	if (!itsInternalMessage.IsEmpty())
		{
		itsInternalMessage.Print(os);
		}
	JSize count = itsList->GetElementCount();
	JSize index;
	for (index = 1; index <= count; index++)
		{
		GMessageHeader* header = itsList->NthElement(index);
		GSaveMessage(is, os, header, kJTrue);
		}

	if (os.fail())
		{
		JString report = "There was a problem saving your mail.";
		JGetUserNotification()->ReportError(report);
		GUnlockFile(itsMailFile);
		is.close();
		os.close();
		JRemoveFile(tempname);
		return;
		}

	is.close();
	os.close();

	GMReplaceFileWithTemp(tempname, itsMailFile);

	JSetPermissions(itsMailFile, perms);
	itsEntry->ForceUpdate();
	BroadcastDataReverted();
	GUnlockFile(itsMailFile);
}

/******************************************************************************
 Flush

 *****************************************************************************/

void			
GMMailboxData::Flush()
{
	if (itsParent != NULL)
		{
		itsParent->Flush();
		return;		
		}
		
	JSize count 		= itsList->GetElementCount();
	for (JSize index = count; index >= 1; index--)
		{
		GMessageHeader* header = itsList->NthElement(index);
		if (header->GetMessageStatus() == GMessageHeader::kDelete)
			{
			BroadcastDataModified();
			RemoveHeader(header);
			delete(header);
			}
		}
	GenerateFromList();
	itsEntry->ForceUpdate();
	Save();
}

/******************************************************************************
 SaveSelectedMessages (public)

 ******************************************************************************/

void
GMMailboxData::SaveSelectedMessages
	(
	ostream&			os,
	const JBoolean		headers,
	GMessageHeaderList* list
	)
{
	if (itsParent != NULL)
		{
		itsParent->SaveSelectedMessages(os, headers, list);
		return;
		}
		
	if (!GLockFile(itsMailFile))
		{
		return;
		}
	Update(kJFalse);
	ifstream is(itsMailFile);
	assert(is.good());

	const JSize count	= list->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMessageHeader* header = list->NthElement(i);
		if (itsList->Includes(header))
			{
			SaveHeader(is, os, header, headers);
			}
		}

	is.close();
	GUnlockFile(itsMailFile);
}

/******************************************************************************
 SaveHeader (private)

 ******************************************************************************/

void
GMMailboxData::SaveHeader
	(
	istream&		is,
	ostream&		os,
	GMessageHeader* header,
	const JBoolean	headers
	)
{
	JIndex start;
	JIndex end = header->GetMessageEnd();
	if (headers)
		{
		start = header->GetHeaderStart();
		}
	else
		{
		start = header->GetHeaderEnd();
		}
	JSeekg(is, start);
	JString body = JRead(is, end - start);
	if (!body.EndsWith("\n\n"))
		{
		body.Append("\n\n");
		}
	body.Print(os);
}

/******************************************************************************
 AdjustList (private)

 ******************************************************************************/

void
GMMailboxData::AdjustList
	(
	GMessageHeaderList* list
	)
{
	if (list->GetElementCount() == 0)
		{
		return;
		}
	GMessageHeader* header	= list->NthElement(1);
	if (header->GetFrom().Contains("Mail System Internal Data"))
		{
		ifstream is(itsMailFile);
		assert(is.good());
		JSeekg(is, header->GetHeaderStart());
		itsInternalMessage.Read(is, header->GetMessageEnd() - header->GetHeaderStart());
		is.close();
		list->DeleteElement(1);
		}
}

/******************************************************************************
 HandleMessageTransfer (private)

 ******************************************************************************/

void
GMMailboxData::HandleMessageTransfer
	(
	const JCharacter*	dest,
	const JBoolean		move,
	GMessageHeaderList* list
	)
{
	if (itsParent != NULL)
		{
		itsParent->HandleMessageTransfer(dest, move, list);
		return;
		}
		
	Update(kJFalse);

	const JSize count	= list->GetElementCount();
	for (JIndex i = count; i >= 1; i--)
		{
		GMessageHeader* header = list->NthElement(i);
		if (!itsList->Includes(header))
			{
			list->Remove(header);
			}
		}

	if (GAppendMessage(itsMailFile, dest, list) && move)
		{
		const JSize count = list->GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			GMessageHeader* header = list->NthElement(i);
			header->SetMessageStatus(GMessageHeader::kDelete);
			Broadcast(HeaderMarkedDeleted(header));
			BroadcastDataModified();
			}
		}
	GMGetApplication()->BroadcastMailboxClosed(dest);
	
	GUnlockFile(itsMailFile);
}

/******************************************************************************
 GenerateFromList

 ******************************************************************************/

void
GMMailboxData::GenerateFromList()
{
	itsFromList->DeleteAll();
	JSize count = itsList->GetElementCount();
	for (JSize index = 1; index <= count; index++)
		{
		GMessageHeader* header = itsList->NthElement(index);
		GMessageFrom* from = new GMessageFrom(header);
		assert(from != NULL);
		itsFromList->InsertSorted(from, kJTrue);
		}
}

/******************************************************************************
 BroadcastDataModified

 *****************************************************************************/

void	
GMMailboxData::BroadcastDataModified()
{
	if (itsParent != NULL)
		{
		itsParent->BroadcastDataModified();
		return;
		}
		
	if (!itsNeedsSave)
		{
		itsNeedsSave	= kJTrue;
		itsHasNewMail	= kJFalse;
		Broadcast(DataModified());
		}
}

/******************************************************************************
 BroadcastDataReverted

 *****************************************************************************/

void	
GMMailboxData::BroadcastDataReverted()
{
	if (itsParent != NULL)
		{
		itsParent->BroadcastDataReverted();
		return;
		}

	if (itsNeedsSave)
		{
		itsNeedsSave = kJFalse;
		Broadcast(DataReverted());
		}
}

/******************************************************************************
 GetMailFile

 ******************************************************************************/

const JString&
GMMailboxData::GetMailFile()
{
	if (itsParent != NULL)
		{
		return itsParent->GetMailFile();
		}
		
	return itsMailFile;
}

/******************************************************************************
 GetHeaderCount

 *****************************************************************************/

JSize		
GMMailboxData::GetHeaderCount()
	const
{
	return itsList->GetElementCount();
}

/******************************************************************************
 GetHeader

 *****************************************************************************/

GMessageHeader*
GMMailboxData::GetHeader
	(
	const JIndex index
	)
{
	assert(itsList->IndexValid(index));
	if (itsSortType == kNone)
		{
		return itsList->NthElement(index);
		}

	JIndex realIndex	= index;
	if (!itsSortIsAscending)
		{
		realIndex		= itsList->GetElementCount() - index + 1;
		}

	if (itsSortType == kDate)
		{
		return itsListByDate->NthElement(realIndex);
		}
	else if (itsSortType == kSize)
		{
		return itsListBySize->NthElement(realIndex);
		}
	else if (itsSortType == kSubject)
		{
		return itsListBySubject->NthElement(realIndex);
		}
	else if (itsSortType == kFrom)
		{
		return itsListByFrom->NthElement(realIndex);
		}
	else
		{
		return itsList->NthElement(index);
		}
	
}

/******************************************************************************
 IsPostponingUpdate

 ******************************************************************************/

JBoolean
GMMailboxData::IsPostponingUpdate()
	const
{
	return itsPostponeUpdate;
}

/******************************************************************************
 ShouldBePostponingUpdate

 ******************************************************************************/

void
GMMailboxData::ShouldBePostponingUpdate
	(
	const JBoolean postpone
	)
{
	itsPostponeUpdate	= postpone;
}

/******************************************************************************
 Includes

 *****************************************************************************/

JBoolean	
GMMailboxData::Includes
	(
	const GMessageHeader*	header,
	JIndex* 				index
	)
{
	JIndex findex;
	JBoolean ok	= itsList->Find(header, &findex);
	if (ok && index != NULL)
		{
		*index	= findex;
		}
	return ok;
}

/******************************************************************************
 DropHeader

	This function places the specified header at the specified location.
	It updates *index to point to the item after the new location of the
	specified header.

 *****************************************************************************/

void		
GMMailboxData::DropHeader
	(
	GMessageHeader*	header,
	JIndex*			index
	)
{
	JIndex i;
	const JBoolean found = itsList->Find(header, &i);
	assert( found );

	if (i == *index)
		{
		(*index)++;
		return;
		}

	RemoveHeaderAtIndex(i);

	if (i < *index)
		{
		(*index)--;
		}

	itsList->InsertAtIndex(*index, header);
	(*index)++;

	HeaderAppended(header);
	BroadcastDataModified();
}

/******************************************************************************
 DropHeaders

  This function inserts a list of headers from srcMBox to a position after
  afterIndex. If afterIndex == 0, then the header is prepended to the list.

 *****************************************************************************/

void		
GMMailboxData::DropHeaders
	(
	const JCharacter* srcMBox,
	const JIndex afterIndex,
	GMessageHeaderList* headers
	)
{
	assert(afterIndex == 0 || itsList->IndexValid(afterIndex));
	
	// already locked in table's WillAcceptDrop.
	Update(kJFalse, kJTrue); 

	JString path, name;
	JSplitPathAndName(itsMailFile, &path, &name);
	JString tempname;
	JError err = JCreateTempFile(path, NULL, &tempname);
	if (!err.OK())
		{
		err = JCreateTempFile(&tempname);
		if (!err.OK())
			{
			err.ReportIfError();
			}
		}

	if (err.OK())
		{
		ifstream is(itsMailFile);
		ifstream mboxis(srcMBox);

		ofstream os(tempname);

		JSize count  = itsList->GetElementCount();
		JSize lcount = headers->GetElementCount();
		JIndex index;

		// 3 cases, prepend, append, insert after afterIndex

		// prepend
		if (afterIndex == 0)
			{
			for (index = 1; index <= lcount; index++)
				{
				GMessageHeader* header = headers->NthElement(index);
				GSaveMessage(mboxis, os, header, kJTrue, kJFalse);
				}
			for (index = 1; index <= count; index++)
				{
				GMessageHeader* header = itsList->NthElement(index);
				GSaveMessage(is, os, header, kJTrue, kJFalse);
				}
			for (index = lcount; index >= 1; index--)
				{
				GMessageHeader* header = headers->NthElement(index);
				itsList->Prepend(header);
				HeaderAppended(header);
				}
			}
		// append
		else if (afterIndex == itsList->GetElementCount())
			{
			for (index = 1; index <= count; index++)
				{
				GMessageHeader* header = itsList->NthElement(index);
				JSize pos = JTellp(os);
				GSaveMessage(is, os, header, kJTrue, kJFalse);
				}
			for (index = 1; index <= lcount; index++)
				{
				GMessageHeader* header = headers->NthElement(index);
				JSize pos = JTellp(os);
				GSaveMessage(mboxis, os, header, kJTrue, kJFalse);
				itsList->Append(header);
				HeaderAppended(header);
				}
			}
		// insert after afterIndex
		else
			{
			GMessageHeader* after	= itsList->NthElement(afterIndex);

			for (index = 1; index <= afterIndex; index++)
				{
				GMessageHeader* header = itsList->NthElement(index);
				JSize pos = JTellp(os);
				GSaveMessage(is, os, header, kJTrue, kJFalse);
				}

			for (index = 1; index <= lcount; index++)
				{
				GMessageHeader* header = headers->NthElement(index);
				JSize pos = JTellp(os);
				GSaveMessage(mboxis, os, header, kJTrue, kJFalse);
				}
			for (index = afterIndex+1; index <= count; index++)
				{
				GMessageHeader* header = itsList->NthElement(index);
				JSize pos = JTellp(os);
				GSaveMessage(is, os, header, kJTrue, kJFalse);
				}
			for (index = lcount; index >= 1; index--)
				{
				GMessageHeader* src = headers->NthElement(index);
				itsList->InsertAfter(after, src);
				HeaderAppended(src);
				}
			}

		is.close();
		os.close();
		mboxis.close();

		GMReplaceFileWithTemp(tempname, itsMailFile);
		}

	GUnlockFile(itsMailFile);
	GUnlockFile(srcMBox);

	itsEntry->ForceUpdate();
	GenerateFromList();

	// need to tell director that things changed, so its message count must
	// change
	Broadcast(UpdateFinished());
}

/******************************************************************************
 ChangeMessageStatus

 *****************************************************************************/

void		
GMMailboxData::ChangeMessageStatus
	(
	GMessageHeader* 					header,
	const GMessageHeader::MessageStatus	status
	)
{
	if (header->GetMessageStatus() == status)
		{
		return;
		}

	if (status == GMessageHeader::kRead &&
		header->GetMessageStatus() != GMessageHeader::kNew &&
		header->GetMessageStatus() != GMessageHeader::kDelete)
		{
		return;
		}

	header->SetMessageStatus(status);
	Broadcast(HeaderStatusChanged(header, status));
	if (itsParent != NULL)
		{
		itsParent->Broadcast(HeaderStatusChanged(header, status));
		return;
		}

	if (status == GMessageHeader::kDelete)
		{
		Broadcast(HeaderMarkedDeleted(header));
		if (itsParent != NULL)
			{
			itsParent->Broadcast(HeaderMarkedDeleted(header));
			return;
			}

		}

	BroadcastDataModified();	
}

/******************************************************************************
 NeedsSave

 *****************************************************************************/

JBoolean	
GMMailboxData::NeedsSave()
	const
{
	return itsNeedsSave;
}

/******************************************************************************
 HasNewMail

 *****************************************************************************/

JBoolean	
GMMailboxData::HasNewMail()
	const
{
	return itsHasNewMail;
}

/******************************************************************************
 ShouldHaveNewMail

 *****************************************************************************/

void		
GMMailboxData::ShouldHaveNewMail
	(
	const JBoolean newMail
	)
{
	itsHasNewMail	= newMail;
}

/******************************************************************************
 GetEntry

 *****************************************************************************/

const JDirEntry&
GMMailboxData::GetEntry()
	const
{
	return *itsEntry;
}

/******************************************************************************
 RemoveHeader

 *****************************************************************************/

void	
GMMailboxData::RemoveHeader
	(
	const GMessageHeader* header
	)
{
	Broadcast(HeaderRemoved(header));
	itsList->Remove(header);

	itsListByDate->Remove(header);
	itsListBySize->Remove(header);
	itsListBySubject->Remove(header);
	itsListByFrom->Remove(header);
}

/******************************************************************************
 RemoveHeaderAtIndex

 *****************************************************************************/

void	
GMMailboxData::RemoveHeaderAtIndex
	(
	const JIndex index
	)
{
	assert(itsList->IndexValid(index));
	GMessageHeader* header	= itsList->NthElement(index);
	
	Broadcast(HeaderRemoved(header));
	itsList->RemoveElement(index);
	
	itsListByDate->Remove(header);
	itsListBySize->Remove(header);
	itsListBySubject->Remove(header);
	itsListByFrom->Remove(header);
}

/******************************************************************************
 HeaderAppended

 *****************************************************************************/

void	
GMMailboxData::HeaderAppended
	(
	GMessageHeader* header
	)
{
	Broadcast(HeaderAdded(header));
	itsListByDate->InsertSorted(header);
	itsListBySize->InsertSorted(header);
	itsListBySubject->InsertSorted(header);
	itsListByFrom->InsertSorted(header);
}

/******************************************************************************
 GetSortType

 *****************************************************************************/

GMMailboxData::kSortType	
GMMailboxData::GetSortType()
	const
{
	return itsSortType;
}

/******************************************************************************
 SortIsAscending

 *****************************************************************************/

JBoolean	
GMMailboxData::SortIsAscending()
	const
{
	return itsSortIsAscending;
}

/******************************************************************************
 ToggleSortType

 *****************************************************************************/

void		
GMMailboxData::ToggleSortType
	(
	const kSortType type
	)
{
	if (type == itsSortType)
		{
		if (itsSortIsAscending)
			{
			itsSortIsAscending	= kJFalse;
			}
		else
			{
			itsSortType			= kNone;
			itsSortIsAscending	= kJTrue;
			}
		}
	else
		{
		itsSortIsAscending	= kJTrue;
		itsSortType			= type;
		}
		
	Broadcast(SortChanged());
}

/******************************************************************************
 SyncLists

 *****************************************************************************/

void	
GMMailboxData::SyncLists()
{
	const JSize count	= itsList->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMessageHeader* header	= itsList->NthElement(i);
		itsListByDate->InsertSorted(header);
		itsListBySize->InsertSorted(header);
		itsListBySubject->InsertSorted(header);
		itsListByFrom->InsertSorted(header);
//		cout << header->GetSubject() << ": " << header->GetBaseSubject() << endl;
		}
}

/******************************************************************************
 CanReorganize

 *****************************************************************************/

JBoolean	
GMMailboxData::CanReorganize()
	const
{
	return JI2B(itsSortType == kNone);
}
