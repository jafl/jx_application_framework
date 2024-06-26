/******************************************************************************
 JFSFileTreeNode.cpp

	The parent of a JFSFileTreeNode does *not* have to be a
	JFSFileTreeNode.  It must, however, be a JFSFileTreeNodeBase, and it
	must have a JDirInfo object.  This allows one to display an arbitrary
	collection of files as well as the contents of a particular directory.

	JDirInfo options are copies from the parent.  If options are changed on
	the parent, they automatically propagate to all descendants.  Never
	manually change the options on anything but the root node.

	Derived classes should override the following function:

		CreateChild
			Create a node of the appropriate type.  The type must be a
			derived class of JFSFileTreeNode.

	BASE CLASS = JFSFileTreeNodeBase

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "JFSFileTreeNode.h"
#include "JFSFileTree.h"
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/JStdError.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFSFileTreeNode::JFSFileTreeNode
	(
	JDirEntry* entry
	)
	:
	JFSFileTreeNodeBase(CanHaveChildren(*entry)),
	itsDirEntry(entry),
	itsDirInfo(nullptr)
{
	assert( itsDirEntry != nullptr );

	const JString name = itsDirEntry->GetName();
	SetName(name);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFSFileTreeNode::~JFSFileTreeNode()
{
	jdelete itsDirEntry;
	jdelete itsDirInfo;
}

/******************************************************************************
 GoUp

 ******************************************************************************/

JError
JFSFileTreeNode::GoUp()
{
	assert( IsRoot() && itsDirInfo != nullptr );

	const JError err = itsDirInfo->GoUp();
	if (err.OK())
	{
		UpdateAfterGo();
	}
	return err;
}

/******************************************************************************
 GoTo

 ******************************************************************************/

JError
JFSFileTreeNode::GoTo
	(
	const JString& path
	)
{
	assert( IsRoot() && itsDirInfo != nullptr );

	const JError err = itsDirInfo->GoTo(path);
	if (err.OK())
	{
		UpdateAfterGo();
	}
	return err;
}

/******************************************************************************
 UpdateAfterGo (private)

 ******************************************************************************/

void
JFSFileTreeNode::UpdateAfterGo()
{
	assert( itsDirInfo != nullptr );

	const JString fullName = itsDirInfo->GetDirectory();
	jdelete itsDirEntry;
	itsDirEntry = jnew JDirEntry(fullName);

	const JString name = itsDirEntry->GetName();
	SetName(name);

	BuildChildList();
}

/******************************************************************************
 Rename

	This provides an efficient way to rename a file.

	If sort==false, it is the caller's responsibility to call
	(node->GetParent())->SortChildren().

 ******************************************************************************/

JError
JFSFileTreeNode::Rename
	(
	const JString&	newName,
	const bool	sort
	)
{
	if (newName == GetName())
	{
		return JNoError();
	}

	const JString path  = itsDirEntry->GetPath();
	JString newFullName = JCombinePathAndName(path, newName);
	if (JNameUsed(newFullName))
	{
		return JDirEntryAlreadyExists(newName);
	}

	JString oldFullName = itsDirEntry->GetFullName();
	const JError err    = JRenameVCS(oldFullName, newFullName);
	if (err.OK())
	{
		SetName(newName);

// newName may be invalid beyond this point if text is from input field

		JAppendDirSeparator(&oldFullName);	// avoid name prefix match
		JAppendDirSeparator(&newFullName);
		UpdatePath(newFullName, oldFullName, newFullName);

		// must be after UpdatePath() so all JDirEntries are correct,
		// because can invoke Update()

		if (sort)
		{
			GetParent()->SortChildren();		// this method maintains the selection
		}

		if (itsDirEntry->IsDirectory())
		{
			GetFSFileTree()->BroadcastDirectoryRenamed(oldFullName, newFullName);
		}
	}

	return err;
}

/******************************************************************************
 UpdatePath

 ******************************************************************************/

void
JFSFileTreeNode::UpdatePath
	(
	const Message& message
	)
{
	assert( message.Is(JFSFileTree::kDirectoryRenamed) );

	auto& info = dynamic_cast<const JFSFileTree::DirectoryRenamed&>(message);
	UpdatePath(info.GetOldPath(), info.GetNewPath());
}

void
JFSFileTreeNode::UpdatePath
	(
	const JString& oldPath,
	const JString& newPath
	)
{
	assert( oldPath.GetLastCharacter() == '/' &&
			newPath.GetLastCharacter() == '/' );

	JString fullName = itsDirEntry->GetFullName();
	JAppendDirSeparator(&fullName);		// need exact match for directories
	if (fullName.StartsWith(oldPath))
	{
		const bool setName = fullName == oldPath;

		JStringIterator iter(&fullName);
		iter.BeginMatch();
		iter.SkipNext(oldPath.GetCharacterCount());
		iter.FinishMatch();
		iter.ReplaceLastMatch(newPath);
		iter.Invalidate();

		UpdatePath(fullName, oldPath, newPath);

		if (setName)
		{
			const JString name = itsDirEntry->GetName();
			SetName(name);
		}
	}
	else if (oldPath.StartsWith(fullName))
	{
		const JSize childCount = GetChildCount();
		for (JIndex i=1; i<=childCount; i++)
		{
			GetFSChild(i)->UpdatePath(oldPath, newPath);
		}
	}
}

// private

void
JFSFileTreeNode::UpdatePath
	(
	const JString&	fullName,
	const JString&	oldPath,
	const JString&	newPath
	)
{
	jdelete itsDirEntry;
	itsDirEntry = jnew JDirEntry(fullName);

	if (itsDirInfo != nullptr)
	{
		const JError err = itsDirInfo->GoTo(fullName);
		assert_ok( err );

		const JSize childCount = GetChildCount();
		for (JIndex i=1; i<=childCount; i++)
		{
			(GetFSChild(i))->UpdatePath(oldPath, newPath);
		}
	}
}

/******************************************************************************
 CanHaveChildren

 ******************************************************************************/

bool
JFSFileTreeNode::CanHaveChildren()
	const
{
	return CanHaveChildren(*itsDirEntry);
}

// static

bool
JFSFileTreeNode::CanHaveChildren
	(
	const JDirEntry& entry
	)
{
	return CanHaveChildren(entry.GetFullName());
}

bool
JFSFileTreeNode::CanHaveChildren
	(
	const JString& path
	)
{
	return JDirectoryReadable(path) &&
				JCanEnterDirectory(path);
}

/******************************************************************************
 OKToOpen (virtual protected)

 ******************************************************************************/

bool
JFSFileTreeNode::OKToOpen()
	const
{
	if (!JFSFileTreeNodeBase::OKToOpen())
	{
		return false;
	}

	auto* me = const_cast<JFSFileTreeNode*>(this);
	if (itsDirInfo == nullptr)
	{
		me->BuildChildList();
	}
	else
	{
		me->Update();
	}

	return itsDirInfo != nullptr;
}

/******************************************************************************
 BuildChildList (private)

 ******************************************************************************/

void
JFSFileTreeNode::BuildChildList()
{
	if (CreateDirInfo())
	{
		DeleteAllChildren();

		for (const auto* e : *itsDirInfo)
		{
			auto* entry = jnew JDirEntry(*e);
			InsertSorted(CreateChild(entry));
		}
	}
}

/******************************************************************************
 CreateDirInfo (private)

 ******************************************************************************/

bool
JFSFileTreeNode::CreateDirInfo()
{
	if (itsDirInfo != nullptr)
	{
		return true;
	}

	assert( !HasChildren() );

	const JString fullName = itsDirEntry->GetFullName();

	bool ok = false;
	JTreeNode* parent;
	if (GetParent(&parent))
	{
		auto& fsParent = dynamic_cast<JFSFileTreeNodeBase&>(*parent);

		JDirInfo* parentInfo;
		ok = fsParent.GetDirInfo(&parentInfo);
		assert( ok );
		ok = JDirInfo::Create(*parentInfo, fullName, &itsDirInfo);
	}
	else
	{
		ok = JDirInfo::Create(fullName, &itsDirInfo);
	}

	if (ok && IsRoot())
	{
		ListenTo(itsDirInfo);
	}

	return ok;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JFSFileTreeNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDirInfo && message.Is(JDirInfo::kSettingsChanged))
	{
		UpdateDirInfoSettings(*itsDirInfo);
		Update(true);		// force call to UpdateChildren()
	}
	else
	{
		JFSFileTreeNodeBase::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateDirInfoSettings (private)

	Caller must call Update(true);

 ******************************************************************************/

void
JFSFileTreeNode::UpdateDirInfoSettings
	(
	const JDirInfo& info
	)
{
	// don't change itsDirInfo, because that causes infinite loop in Receive()

	const JSize childCount = GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
	{
		JFSFileTreeNode* child = GetFSChild(i);

		if (child->itsDirInfo != nullptr)
		{
			(child->itsDirInfo)->CopySettings(info);
			child->UpdateDirInfoSettings(info);
		}
	}
}

/******************************************************************************
 CreateChild (virtual protected)

	Derived classes can override this to create a node of the appropriate
	type.  The type must be a derived class of JFSFileTreeNode.

 ******************************************************************************/

JFSFileTreeNode*
JFSFileTreeNode::CreateChild
	(
	JDirEntry* entry
	)
{
	auto* node = jnew JFSFileTreeNode(entry);
	return node;
}

/******************************************************************************
 Update (virtual)

	Returns true if anything changed.

	If !force && *updateNode != nullptr, sets *updateNode to next node
	in depth-first search.

 ******************************************************************************/

bool
JFSFileTreeNode::Update
	(
	const bool			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	bool changed = itsDirEntry->Update(force);
	if (changed)
	{
		GetTree()->BroadcastChange(this);
	}

	const bool canHaveChildren = CanHaveChildren();
	ShouldBeOpenable(canHaveChildren);

	if (canHaveChildren && itsDirInfo != nullptr)
	{
		if (itsDirInfo->Update(force))
		{
			UpdateChildren();
			changed = true;
		}

		// check if subdirectories need updating

		if ((force || updateNode == nullptr || *updateNode == nullptr) &&
			JFSFileTreeNodeBase::Update(force, updateNode))
		{
			changed = true;
		}
	}
	else if (!canHaveChildren)
	{
		if (itsDirInfo != nullptr)
		{
			changed = true;
		}

		jdelete itsDirInfo;
		itsDirInfo = nullptr;
	}

	// find next updateNode

	if (force || updateNode == nullptr || *updateNode == nullptr)
	{
		return changed;
	}

	JTreeNode* parent = nullptr;
	if (HasChildren())
	{
		*updateNode = GetFSChild(1);
	}
	else if (GetParent(&parent))
	{
		JTreeNode* child = this;
		do
		{
			const JIndex parentIndex = child->GetIndexInParent();
			if (parentIndex < parent->GetChildCount())
			{
				*updateNode =
					dynamic_cast<JFSFileTreeNodeBase*>(parent->GetChild(parentIndex+1));
				break;
			}
			else
			{
				child = parent;
				parent->GetParent(&parent);
			}
		}
			while (parent != nullptr);

		if (parent == nullptr)
		{
			*updateNode =
				dynamic_cast<JFSFileTreeNodeBase*>(GetTree()->GetRoot());
		}
	}
	else
	{
		*updateNode = this;
	}

	return changed;
}

/******************************************************************************
 UpdateChildren (private)

	Maintains selection, open state, etc. of existing children.

 ******************************************************************************/

void
JFSFileTreeNode::UpdateChildren()
{
	assert( itsDirInfo != nullptr );

	// build updated list of entries

	JPtrArray<JTreeNode> newChildren(JPtrArrayT::kForgetAll);
	newChildren.SetCompareFunction(CompareTypesAndNames);

	for (const auto* e : *itsDirInfo)
	{
		auto* entry = jnew JDirEntry(*e);
		newChildren.InsertSorted(CreateChild(entry));
	}

	// toss new entries that already exist
	// remove old entries that no longer exist

	for (JIndex i=GetChildCount(); i>=1; i--)
	{
		JTreeNode* node = GetChild(i);
		JIndex j;
		if (newChildren.SearchSorted(node, JListT::kAnyMatch, &j))
		{
			newChildren.DeleteItem(j);
		}
		else
		{
			jdelete node;
		}
	}

	// save remaining new entries

	for (auto* n : newChildren)
	{
		InsertSorted(n);
	}

	// re-sort since mod time, size, etc. of *pre-existing* children may have changed

	SortChildren();
}

/******************************************************************************
 CompareTypesAndNames (static private)

 ******************************************************************************/

std::weak_ordering
JFSFileTreeNode::CompareTypesAndNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	auto* n1 = dynamic_cast<JFSFileTreeNode*>(e1);
	auto* n2 = dynamic_cast<JFSFileTreeNode*>(e2);

	std::weak_ordering r = n1->itsDirEntry->GetType() <=> n2->itsDirEntry->GetType();
	if (r == std::weak_ordering::equivalent)
	{
		r = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
	}
	return r;
}

/******************************************************************************
 CompareUserNames (static protected)

 ******************************************************************************/

std::weak_ordering
JFSFileTreeNode::CompareUserNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	auto* n1 = dynamic_cast<JFSFileTreeNode*>(e1);
	auto* n2 = dynamic_cast<JFSFileTreeNode*>(e2);

	JString u1 = n1->itsDirEntry->GetUserName();
	JString u2 = n2->itsDirEntry->GetUserName();
	std::weak_ordering result = JCompareStringsCaseInsensitive(&u1, &u2);

	if (result == std::weak_ordering::equivalent)
	{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
	}
	return result;
}

/******************************************************************************
 CompareGroups (static protected)

 ******************************************************************************/

std::weak_ordering
JFSFileTreeNode::CompareGroupNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	auto* n1 = dynamic_cast<JFSFileTreeNode*>(e1);
	auto* n2 = dynamic_cast<JFSFileTreeNode*>(e2);

	JString u1 = n1->itsDirEntry->GetGroupName();
	JString u2 = n2->itsDirEntry->GetGroupName();

	auto result = JCompareStringsCaseInsensitive(&u1, &u2);
	if (result == std::weak_ordering::equivalent)
	{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
	}
	return result;
}

/******************************************************************************
 CompareSizes (static protected)

 ******************************************************************************/

std::weak_ordering
JFSFileTreeNode::CompareSizes
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	auto* n1 = dynamic_cast<JFSFileTreeNode*>(e1);
	auto* n2 = dynamic_cast<JFSFileTreeNode*>(e2);

	if (n1->IsOpenable())
	{
		return std::weak_ordering::less;
	}
	else if (n2->IsOpenable())
	{
		return std::weak_ordering::greater;
	}

	auto result = JDirEntry::CompareSizes(n1->itsDirEntry, n2->itsDirEntry);
	if (result == std::weak_ordering::equivalent)
	{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
	}
	return result;
}

/******************************************************************************
 CompareDates (static protected)

 ******************************************************************************/

std::weak_ordering
JFSFileTreeNode::CompareDates
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	auto* n1 = dynamic_cast<JFSFileTreeNode*>(e1);
	auto* n2 = dynamic_cast<JFSFileTreeNode*>(e2);

	auto result = JDirEntry::CompareModTimes(n1->itsDirEntry, n2->itsDirEntry);
	if (result == std::weak_ordering::equivalent)
	{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
	}
	return result;
}

/******************************************************************************
 GetFSChild

 ******************************************************************************/

JFSFileTreeNode*
JFSFileTreeNode::GetFSChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<JFSFileTreeNode*>(GetChild(index));
	assert( node != nullptr );
	return node;
}

const JFSFileTreeNode*
JFSFileTreeNode::GetFSChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const JFSFileTreeNode*>(GetChild(index));
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 GetDirInfo (virtual)

 ******************************************************************************/

bool
JFSFileTreeNode::GetDirInfo
	(
	JDirInfo** info
	)
{
	*info = itsDirInfo;
	return itsDirInfo != nullptr;
}

bool
JFSFileTreeNode::GetDirInfo
	(
	const JDirInfo** info
	)
	const
{
	*info = itsDirInfo;
	return itsDirInfo != nullptr;
}
