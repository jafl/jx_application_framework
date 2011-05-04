/******************************************************************************
 GPMProcessList.h

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GPMProcessList
#define _H_GPMProcessList

#include <JContainer.h>
#include <JPtrArray-JString.h>
#include "GPMProcessEntry.h"

class JTree;
class JTreeNode;
class JDirInfo;

class GPMProcessList : public JContainer
{
public:

	// don't forget to update GPMListHeaderWidget

	enum ListColType
	{
		kListState = 1,
		kListPID,
		kListUser,
//		kListPPID,
//		kListPriority,
		kListNice,
		kListSize,
//		kListResident,
//		kListShare,
		kListCPU,
		kListMemory,
		kListTime,
		kListCommand,

		kListCount = kListCommand
	};

	// don't forget to update GPMTreeHeaderWidget

	enum TreeColType
	{
		kTreeOpenClose = 1,
		kTreeCommand,
		kTreeState,
		kTreePID,
		kTreeUser,
//		kTreePPID,
//		kTreePriority,
		kTreeNice,
		kTreeSize,
//		kTreeResident,
//		kTreeShare,
		kTreeCPU,
		kTreeMemory,
		kTreeTime,

		kTreeCount = kTreeTime
	};

public:

	GPMProcessList();
	virtual ~GPMProcessList();

	void	Update();

	const GPMProcessEntry*	GetProcessEntry(const JIndex index) const;
	JBoolean				GetEntryIndex(const GPMProcessEntry* entry, JIndex *index);
	JBoolean				FindProcessEntry(const pid_t pid, GPMProcessEntry** entry) const;
	JBoolean				ClosestMatch(const JCharacter* prefix, GPMProcessEntry** entry) const;

	const JPtrArray<GPMProcessEntry>&	GetHiddenProcesses() const;

	JBoolean	ListColIsSelected(const JIndex index) const;
	ListColType	GetSelectedListCol() const;
	void		ListColSelected(const JIndex index);

	JBoolean	TreeColIsSelected(const JIndex index) const;
	TreeColType	GetSelectedTreeCol() const;
	void		TreeColSelected(const JIndex index);

	JTree*		GetProcessTree();

	JBoolean	WillShowUserOnly() const;
	void		ShouldShowUserOnly(const JBoolean show);

private:

	JPtrArray<GPMProcessEntry>*	itsVisibleEntries;
	JPtrArray<GPMProcessEntry>*	itsAlphaEntries;
	JPtrArray<GPMProcessEntry>*	itsHiddenEntries;
	JTree*						itsTree;
	JTreeNode*					itsRootNode;
	JFloat						itsElapsedTime;
	JFloat						itsLastTime;
	ListColType					itsListColType;
	TreeColType					itsTreeColType;
	JBoolean					itsIsShowingUserOnly;
	const JIndex				itsUID;

	#ifdef _J_HAS_PROC
	JDirInfo*					itsDirInfo;
	#endif

private:

	// not allowed

	GPMProcessList(const GPMProcessList& source);
	const GPMProcessList& operator=(const GPMProcessList& source);

public:

	static const JCharacter* kListChanged;
	static const JCharacter* kPrepareForUpdate;

	class ListChanged : public JBroadcaster::Message
		{
		public:

			ListChanged()
				:
				JBroadcaster::Message(kListChanged)
				{ };
		};

	class PrepareForUpdate : public JBroadcaster::Message
		{
		public:

			PrepareForUpdate()
				:
				JBroadcaster::Message(kPrepareForUpdate)
				{ };
		};
};


/******************************************************************************
 ListColIsSelected

 ******************************************************************************/

inline JBoolean
GPMProcessList::ListColIsSelected
	(
	const JIndex index
	)
	const
{
	return JI2B(index == (JIndex) itsListColType);
}

/******************************************************************************
 GetSelectedListCol

 ******************************************************************************/

inline GPMProcessList::ListColType
GPMProcessList::GetSelectedListCol()
	const
{
	return itsListColType;
}

/******************************************************************************
 TreeColIsSelected

 ******************************************************************************/

inline JBoolean
GPMProcessList::TreeColIsSelected
	(
	const JIndex index
	)
	const
{
	return JI2B(index == (JIndex) itsTreeColType);
}

/******************************************************************************
 GetSelectedTreeCol

 ******************************************************************************/

inline GPMProcessList::TreeColType
GPMProcessList::GetSelectedTreeCol()
	const
{
	return itsTreeColType;
}

/******************************************************************************
 GetProcessTree

 ******************************************************************************/

inline JTree*
GPMProcessList::GetProcessTree()
{
	return itsTree;
}

/******************************************************************************
 WillShowUserOnly

 ******************************************************************************/

inline JBoolean
GPMProcessList::WillShowUserOnly()
	const
{
	return itsIsShowingUserOnly;
}

/******************************************************************************
 GetProcessEntry

 ******************************************************************************/

inline const GPMProcessEntry*
GPMProcessList::GetProcessEntry
	(
	const JIndex index
	)
	const
{
	return itsVisibleEntries->NthElement(index);
}

/******************************************************************************
 GetEntryIndex

 ******************************************************************************/

inline JBoolean
GPMProcessList::GetEntryIndex
	(
	const GPMProcessEntry*	entry,
	JIndex*					index
	)
{
	return itsVisibleEntries->SearchSorted(const_cast<GPMProcessEntry*>(entry), JOrderedSetT::kAnyMatch, index);
}

/******************************************************************************
 GetHiddenProcesses

 ******************************************************************************/

inline const JPtrArray<GPMProcessEntry>&
GPMProcessList::GetHiddenProcesses()
	const
{
	return *itsHiddenEntries;
}

#endif
