/******************************************************************************
 GPMProcessList.h

	Copyright (C) 2000 by Glenn W. Bach.

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
	bool				GetEntryIndex(const GPMProcessEntry* entry, JIndex *index);
	bool				FindProcessEntry(const pid_t pid, GPMProcessEntry** entry) const;
	bool				ClosestMatch(const JString& prefix, GPMProcessEntry** entry) const;

	const JPtrArray<GPMProcessEntry>&	GetHiddenProcesses() const;

	bool	ListColIsSelected(const JIndex index) const;
	ListColType	GetSelectedListCol() const;
	void		ListColSelected(const JIndex index);

	bool	TreeColIsSelected(const JIndex index) const;
	TreeColType	GetSelectedTreeCol() const;
	void		TreeColSelected(const JIndex index);

	JTree*		GetProcessTree();

	bool	WillShowUserOnly() const;
	void		ShouldShowUserOnly(const bool show);

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
	bool					itsIsShowingUserOnly;
	const JIndex				itsUID;

	#ifdef _J_HAS_PROC
	JDirInfo*					itsDirInfo;
	#endif

private:

	// not allowed

	GPMProcessList(const GPMProcessList& source);
	const GPMProcessList& operator=(const GPMProcessList& source);

public:

	static const JUtf8Byte* kListChanged;
	static const JUtf8Byte* kPrepareForUpdate;

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

inline bool
GPMProcessList::ListColIsSelected
	(
	const JIndex index
	)
	const
{
	return index == (JIndex) itsListColType;
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

inline bool
GPMProcessList::TreeColIsSelected
	(
	const JIndex index
	)
	const
{
	return index == (JIndex) itsTreeColType;
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

inline bool
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
	return itsVisibleEntries->GetElement(index);
}

/******************************************************************************
 GetEntryIndex

 ******************************************************************************/

inline bool
GPMProcessList::GetEntryIndex
	(
	const GPMProcessEntry*	entry,
	JIndex*					index
	)
{
	return itsVisibleEntries->SearchSorted(const_cast<GPMProcessEntry*>(entry), JListT::kAnyMatch, index);
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
