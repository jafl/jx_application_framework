/******************************************************************************
 GPMProcessList.cc

	BASE CLASS = JContainer

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

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

const JCharacter* GPMProcessList::kListChanged      = "ListChanged::GPMProcessList";
const JCharacter* GPMProcessList::kPrepareForUpdate = "PrepareForUpdate::GPMProcessList";

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
	,itsDirInfo(NULL)
#endif
{
	itsVisibleEntries = new JPtrArray<GPMProcessEntry>(JPtrArrayT::kForgetAll);
	assert(itsVisibleEntries != NULL);
	itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPID);
	InstallOrderedSet(itsVisibleEntries);
	itsListColType = kListPID;
	itsTreeColType = kTreeCommand;

	itsAlphaEntries = new JPtrArray<GPMProcessEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != NULL );
	itsAlphaEntries->SetCompareFunction(GPMProcessEntry::CompareListCommandForIncrSearch);

	itsHiddenEntries = new JPtrArray<GPMProcessEntry>(JPtrArrayT::kDeleteAll);
	assert(itsHiddenEntries != NULL);
	itsHiddenEntries->SetCompareFunction(GPMProcessEntry::CompareListPID);

	itsRootNode = new JTreeNode(NULL);
	assert( itsRootNode != NULL );
	itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeCommand,
										 JOrderedSetT::kSortAscending, kJTrue);

	itsTree = new JTree(itsRootNode);
	assert( itsTree != NULL );

	Update();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMProcessList::~GPMProcessList()
{
	delete itsHiddenEntries;
	delete itsVisibleEntries;
	delete itsAlphaEntries;
	delete itsTree;

	#ifdef _J_HAS_PROC
	delete itsDirInfo;
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
	if (itsDirInfo == NULL)
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
			GPMProcessEntry* pentry = new GPMProcessEntry(itsTree, entry);
			assert(pentry != NULL);
			newEntries.InsertSorted(pentry);
			}
		}
	}
#elif defined _J_HAS_SYSCTL
	{
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

	size_t len;
	int result = sysctl(mib, 4, NULL, &len, NULL, 0);
	if (result != 0)
		{
		itsRootNode->DeleteAllChildren();
		itsVisibleEntries->RemoveAll();
		itsAlphaEntries->RemoveAll();
		}
	else
		{
		void* buf = malloc(len);
		assert( buf != NULL );

		result = sysctl(mib, 4, buf, &len, NULL, 0);
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
			for (JIndex i=0; i<count; i++)
				{
				GPMProcessEntry* pentry = new GPMProcessEntry(itsTree, list[i]);
				assert( pentry != NULL );
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
		GPMProcessEntry* pentry	= itsHiddenEntries->NthElement(i);
		JIndex findex;
		if (!newEntries.SearchSorted(pentry, JOrderedSetT::kAnyMatch, &findex))
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
			GPMProcessEntry* pentry	= newEntries.NthElement(i);
			if (pentry->GetUID() != itsUID)
				{
				newEntries.RemoveElement(i);

				JIndex findex;
				if (itsHiddenEntries->SearchSorted(pentry, JOrderedSetT::kAnyMatch, &findex))
					{
					delete pentry;
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
			(itsHiddenEntries->NthElement(i))->Update(itsElapsedTime);
			}
		}

	// remove dead processes from the visible list

	count = itsVisibleEntries->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		GPMProcessEntry* pentry	= itsVisibleEntries->NthElement(i);
		JIndex findex;
		if (newEntries.SearchSorted(pentry, JOrderedSetT::kAnyMatch, &findex))
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
		(itsVisibleEntries->NthElement(i))->Update(itsElapsedTime);
		}

	itsVisibleEntries->Sort();
	itsRootNode->SortChildren(kJTrue);

	// add new processes to the list

	JOrderedSetT::CompareResult (*treeCompareFn)(JTreeNode * const &,
												 JTreeNode * const &);
	JOrderedSetT::SortOrder	treeSortOrder;
	itsRootNode->GetChildCompareFunction(&treeCompareFn, &treeSortOrder);

	count = newEntries.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		GPMProcessEntry* pentry = newEntries.NthElement(i);
		pentry->Update(itsElapsedTime);
		itsVisibleEntries->InsertSorted(pentry);
		itsAlphaEntries->InsertSorted(pentry);

		pentry->SetChildCompareFunction(treeCompareFn, treeSortOrder, kJTrue);
		}

	// reparent all nodes

	count = itsVisibleEntries->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		GPMProcessEntry* pentry = itsVisibleEntries->NthElement(i);
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
		GPMProcessEntry* e = itsVisibleEntries->NthElement(i);
		if (e->GetPID() == pid)
			{
			*entry = e;
			return kJTrue;
			}
		}

	*entry = NULL;
	return kJFalse;
}

/******************************************************************************
 ClosestMatch

 ******************************************************************************/

JBoolean
GPMProcessList::ClosestMatch
	(
	const JCharacter*	prefix,
	GPMProcessEntry**	entry
	)
	const
{
	GPMProcessEntry target(itsTree, prefix);
	JBoolean found;
	JIndex i = itsAlphaEntries->SearchSorted1(&target, JOrderedSetT::kFirstMatch, &found);
	if (i > itsAlphaEntries->GetElementCount())		// insert beyond end of list
		{
		i = itsAlphaEntries->GetElementCount();
		}

	if (i > 0)
		{
		*entry = itsAlphaEntries->NthElement(i);
		return kJTrue;
		}
	else
		{
		*entry = NULL;
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
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListUser)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListUser);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListNice)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListNice);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortAscending);
		changed = kJTrue;
		}
	else if (index == kListSize)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListSize);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListCPU)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPercentCPU);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListMemory)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListPercentMemory);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListTime)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListTime);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortDescending);
		changed = kJTrue;
		}
	else if (index == kListCommand)
		{
		itsVisibleEntries->SetCompareFunction(GPMProcessEntry::CompareListCommand);
		itsVisibleEntries->SetSortOrder(JOrderedSetT::kSortAscending);
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
											 JOrderedSetT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeUser)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeUser,
											 JOrderedSetT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeNice)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeNice,
											 JOrderedSetT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeSize)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeSize,
											 JOrderedSetT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeCPU)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreePercentCPU,
											 JOrderedSetT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeMemory)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreePercentMemory,
											 JOrderedSetT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeTime)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeTime,
											 JOrderedSetT::kSortDescending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
	else if (index == kTreeCommand)
		{
		Broadcast(PrepareForUpdate());
		itsRootNode->SetChildCompareFunction(GPMProcessEntry::CompareTreeCommand,
											 JOrderedSetT::kSortAscending, kJTrue);
		itsTreeColType = (TreeColType) index;
		Broadcast(ListChanged());
		}
}
