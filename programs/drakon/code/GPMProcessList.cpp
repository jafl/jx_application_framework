/******************************************************************************
 GPMProcessList.cpp

	BASE CLASS = JContainer

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GPMProcessList.h>
#include <GPMProcessEntry.h>

#include <JTree.h>

#ifdef _J_HAS_PROC
#include <JDirInfo.h>
#endif

#include <jStreamUtil.h>
#include <jTime.h>
#include <jSysUtil.h>

#include <jFStreamUtil.h>
#include <sys/time.h>
#include <unistd.h>

#include <jAssert.h>

const JUtf8Byte* GPMProcessList::kListChanged      = "ListChanged::GPMProcessList";
const JUtf8Byte* GPMProcessList::kPrepareForUpdate = "PrepareForUpdate::GPMProcessList";

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMProcessList::GPMProcessList()
	:
	itsElapsedTime(0),
	itsLastTime(0),
	itsIsShowingUserOnly(kJTrue),
	itsUID(getuid())
#ifdef _J_HAS_PROC
	,itsDirInfo(nullptr)
#endif
{
	itsVisibleEntries = jnew JPtrArray<GPMProcessEntry>(JPtrArrayT::kForgetAll);
	assert(itsVisibleEntries != nullptr);
	itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPID);
	InstallList(itsVisibleEntries);
	itsListColType = kListPID;
	itsTreeColType = kTreeCommand;

	itsAlphaEntries = jnew JPtrArray<GPMProcessEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != nullptr );
	itsAlphaEntries->SetCompareFunction(GPMProcessEntry::CompareListCommandForIncrSearch);

	itsHiddenEntries = jnew JPtrArray<GPMProcessEntry>(JPtrArrayT::kDeleteAll);
	assert(itsHiddenEntries != nullptr);
	itsHiddenEntries->SetCompareFunction(GPMProcessEntry::CompareListPID);

	itsRootNode = jnew JTreeNode(nullptr);
	assert( itsRootNode != nullptr );
	itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeCommand,
										 JListT::kSortAscending, kJTrue);

	itsTree = jnew JTree(itsRootNode);
	assert( itsTree != nullptr );

	Update();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMProcessList::~GPMProcessList()
{
	jdelete itsHiddenEntries;
	jdelete itsVisibleEntries;
	jdelete itsAlphaEntries;
	jdelete itsTree;

	#ifdef _J_HAS_PROC
	jdelete itsDirInfo;
	#endif
}

/******************************************************************************
 ShouldShowUserOnly

 ******************************************************************************/

void
GPMProcessList::ShouldShowUserOnly
	(
	const JBoolean show
	)
{
	if (itsIsShowingUserOnly != show)
		{
		itsIsShowingUserOnly = show;
		Update();
		}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
GPMProcessList::Update()
{
	Broadcast(PrepareForUpdate());
	
	struct timeval currentTime;
	struct timezone tZone;
	gettimeofday(&currentTime, &tZone);

	const JFloat newTime = currentTime.tv_sec + (JFloat)(currentTime.tv_usec / 1000000.0);
	itsElapsedTime       = itsLastTime == 0 ? 0 : newTime - itsLastTime;
	itsLastTime          = newTime;

	JPtrArray<GPMProcessEntry> newEntries(JPtrArrayT::kForgetAll);
	newEntries.SetCompareFunction(GPMProcessEntry::CompareListPID);

#ifdef _J_HAS_PROC
	{
	if (itsDirInfo == nullptr)
		{
		JBoolean ok	= JDirInfo::Create("/proc", &itsDirInfo);
		assert(ok);
		}
	else
		{
		itsDirInfo->ForceUpdate();
		}

	const JSize count = itsDirInfo->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = itsDirInfo->GetEntry(i);
		if (entry.GetName().IsInteger())
			{
			GPMProcessEntry* pentry = jnew GPMProcessEntry(itsTree, entry);
			assert(pentry != nullptr);
			newEntries.InsertSorted(pentry);
			}
		}
	}
#elif defined _J_HAS_SYSCTL
	{
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

	size_t len;
	int result = sysctl(mib, 4, nullptr, &len, nullptr, 0);
	if (result != 0)
		{
		itsRootNode->DeleteAllChildren();
		itsVisibleEntries->RemoveAll();
		itsAlphaEntries->RemoveAll();
		}
	else
		{
		void* buf = malloc(len);
		assert( buf != nullptr );

		result = sysctl(mib, 4, buf, &len, nullptr, 0);
		if (result != 0)
			{
			itsRootNode->DeleteAllChildren();
			itsVisibleEntries->RemoveAll();
			itsAlphaEntries->RemoveAll();
			}
		else
			{
			kinfo_proc* list = (kinfo_proc*) buf;

			const JSize count = len / sizeof(kinfo_proc);
			for (JUnsignedOffset i=0; i<count; i++)
				{
				GPMProcessEntry* pentry = jnew GPMProcessEntry(itsTree, list[i]);
				assert( pentry != nullptr );
				newEntries.InsertSorted(pentry);
				}
			}

		free(buf);
		}
	}
#endif

	if (!itsIsShowingUserOnly)
		{
		itsHiddenEntries->CleanOut();
		}

	// remove dead processes from the hidden list

	JSize count = itsHiddenEntries->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		GPMProcessEntry* pentry	= itsHiddenEntries->GetElement(i);
		JIndex findex;
		if (!newEntries.SearchSorted(pentry, JListT::kAnyMatch, &findex))
			{
			itsHiddenEntries->DeleteElement(i);
			}
		}

	// hide processes from other users, if necessary

	if (itsIsShowingUserOnly)
		{
		count = newEntries.GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			GPMProcessEntry* pentry	= newEntries.GetElement(i);
			if (pentry->GetUID() != itsUID)
				{
				newEntries.RemoveElement(i);

				JIndex findex;
				if (itsHiddenEntries->SearchSorted(pentry, JListT::kAnyMatch, &findex))
					{
					jdelete pentry;
					}
				else
					{
					itsHiddenEntries->InsertSorted(pentry);
					}
				}
			}

		count = itsHiddenEntries->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsHiddenEntries->GetElement(i))->Update(itsElapsedTime);
			}
		}

	// remove dead processes from the visible list

	count = itsVisibleEntries->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		GPMProcessEntry* pentry	= itsVisibleEntries->GetElement(i);
		JIndex findex;
		if (newEntries.SearchSorted(pentry, JListT::kAnyMatch, &findex))
			{
			newEntries.DeleteElement(findex);
			}
		else
			{
			while (pentry->HasChildren())
				{
				itsRootNode->InsertSorted(pentry->GetChild(1));
				}

			itsAlphaEntries->Remove(pentry);
			itsVisibleEntries->DeleteElement(i);
			}
		}

	// update information on all pre-existing processes

	count = itsVisibleEntries->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsVisibleEntries->GetElement(i))->Update(itsElapsedTime);
		}

	itsVisibleEntries->Sort();
	itsRootNode->SortChildren(kJTrue);

	// add new processes to the list

	JListT::CompareResult (*treeCompareFn)(JTreeNode * const &,
												 JTreeNode * const &);
	JListT::SortOrder	treeSortOrder;
	itsRootNode->GetChildCompareFunction(&treeCompareFn, &treeSortOrder);

	count = newEntries.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		GPMProcessEntry* pentry = newEntries.GetElement(i);
		pentry->Update(itsElapsedTime);
		itsVisibleEntries->InsertSorted(pentry);
		itsAlphaEntries->InsertSorted(pentry);

		pentry->SetChildCompareFunction(treeCompareFn, treeSortOrder, kJTrue);
		}

	// reparent all nodes

	count = itsVisibleEntries->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		GPMProcessEntry* pentry = itsVisibleEntries->GetElement(i);
		GPMProcessEntry* parent;
		if (FindProcessEntry(pentry->GetPPID(), &parent) &&
			parent != pentry)
			{
			parent->InsertSorted(pentry);
			}
		else
			{
			itsRootNode->InsertSorted(pentry);
			}
		}

	Broadcast(ListChanged());
}

/******************************************************************************
 FindProcessEntry

 ******************************************************************************/

JBoolean
GPMProcessList::FindProcessEntry
	(
	const pid_t			pid,
	GPMProcessEntry**	entry
	)
	const
{
	const JSize count = itsVisibleEntries->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		GPMProcessEntry* e = itsVisibleEntries->GetElement(i);
		if (e->GetPID() == pid)
			{
			*entry = e;
			return kJTrue;
			}
		}

	*entry = nullptr;
	return kJFalse;
}

/******************************************************************************
 ClosestMatch

 ******************************************************************************/

JBoolean
GPMProcessList::ClosestMatch
	(
	const JString&		prefix,
	GPMProcessEntry**	entry
	)
	const
{
	GPMProcessEntry target(itsTree, prefix);
	JBoolean found;
	JIndex i = itsAlphaEntries->SearchSorted1(&target, JListT::kFirstMatch, &found);
	if (i > itsAlphaEntries->GetElementCount())		// insert beyond end of list
		{
		i = itsAlphaEntries->GetElementCount();
		}

	if (i > 0)
		{
		*entry = itsAlphaEntries->GetElement(i);
		return kJTrue;
		}
	else
		{
		*entry = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 ListColSelected

 ******************************************************************************/

void
GPMProcessList::ListColSelected
	(
	const JIndex index
	)
{
	JBoolean changed = kJFalse;

	if (index == kListPID)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPID);
		itsVisibleEntries->SetSortOrder(JListT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListUser)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListUser);
		itsVisibleEntries->SetSortOrder(JListT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListNice)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListNice);
		itsVisibleEntries->SetSortOrder(JListT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListSize)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListSize);
		itsVisibleEntries->SetSortOrder(JListT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListCPU)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPercentCPU);
		itsVisibleEntries->SetSortOrder(JListT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListMemory)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPercentMemory);
		itsVisibleEntries->SetSortOrder(JListT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListTime)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListTime);
		itsVisibleEntries->SetSortOrder(JListT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListCommand)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListCommand);
		itsVisibleEntries->SetSortOrder(JListT::kSortAscending);
		changed = kJTrue;
		}

	if (changed)
		{
		Broadcast(PrepareForUpdate());
		itsVisibleEntries->Sort();
		itsListColType = (ListColType) index;
		Broadcast(ListChanged());
		}
}

/******************************************************************************
 TreeColSelected

 ******************************************************************************/

void
GPMProcessList::TreeColSelected
	(
	const JIndex index
	)
{
	if (index == kTreePID)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreePID,
											 JListT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeUser)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeUser,
											 JListT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeNice)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeNice,
											 JListT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeSize)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeSize,
											 JListT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeCPU)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreePercentCPU,
											 JListT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeMemory)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreePercentMemory,
											 JListT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeTime)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeTime,
											 JListT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeCommand)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeCommand,
											 JListT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
}
