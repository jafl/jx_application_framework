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
#include <JPtrArray-JString.h>
#include <JStringIterator.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

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
	assert( itsDirEntry != nullptr );

	const JString name = itsDirEntry->GetName();
	SetName(name);

	BuildChildList();
}

/******************************************************************************
 Rename

	This provides an efficient way to rename a file.

	If sort==kJFalse, it is the caller's responsibility to call
	(node->GetParent())->SortChildren().

 ******************************************************************************/

JError
JFSFileTreeNode::Rename
	(
	const JString&	newName,
	const JBoolean	sort
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

	const JFSFileTree::DirectoryRenamed* info =
		dynamic_cast<const JFSFileTree::DirectoryRenamed*>(&message);
	assert( info != nullptr );

	UpdatePath(info->GetOldPath(), info->GetNewPath());
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
	if (fullName.BeginsWith(oldPath))
		{
		const JBoolean setName = JI2B(fullName == oldPath);

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
	else if (oldPath.BeginsWith(fullName))
		{
		const JSize childCount = GetChildCount();
		for (JIndex i=1; i<=childCount; i++)
			{
			(GetFSChild(i))->UpdatePath(oldPath, newPath);
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
	assert( itsDirEntry != nullptr );

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

JBoolean
JFSFileTreeNode::CanHaveChildren()
	const
{
	return CanHaveChildren(*itsDirEntry);
}

// static

JBoolean
JFSFileTreeNode::CanHaveChildren
	(
	const JDirEntry& entry
	)
{
	return CanHaveChildren(entry.GetFullName());
}

JBoolean
JFSFileTreeNode::CanHaveChildren
	(
	const JString& path
	)
{
	return JI2B(JDirectoryReadable(path) &&
				JCanEnterDirectory(path));
}

/******************************************************************************
 OKToOpen (virtual protected)

 ******************************************************************************/

JBoolean
JFSFileTreeNode::OKToOpen()
	const
{
	if (!JFSFileTreeNodeBase::OKToOpen())
		{
		return kJFalse;
		}

	JFSFileTreeNode* me = const_cast<JFSFileTreeNode*>(this);
	if (itsDirInfo == nullptr)
		{
		me->BuildChildList();
		}
	else
		{
		me->Update();
		}

	return JI2B(itsDirInfo != nullptr);
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

		for (JDirEntry* e : *itsDirInfo)
			{
			JDirEntry* entry = jnew JDirEntry(*e);
			assert( entry != nullptr );
			InsertSorted(CreateChild(entry));
			}
		}
}

/******************************************************************************
 CreateDirInfo (private)

 ******************************************************************************/

JBoolean
JFSFileTreeNode::CreateDirInfo()
{
	if (itsDirInfo != nullptr)
		{
		return kJTrue;
		}

	assert( !HasChildren() );

	const JString fullName = itsDirEntry->GetFullName();

	JBoolean ok = kJFalse;
	JTreeNode* parent;
	if (GetParent(&parent))
		{
		JFSFileTreeNodeBase* fsParent = dynamic_cast<JFSFileTreeNodeBase*>(parent);
		assert( fsParent != nullptr );

		JDirInfo* parentInfo;
		ok = fsParent->GetDirInfo(&parentInfo);
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
		Update(kJTrue);		// force call to UpdateChildren()
		}
	else
		{
		JFSFileTreeNodeBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDirInfoSettings (private)

	Caller must call Update(kJTrue);

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
	JFSFileTreeNode* node = jnew JFSFileTreeNode(entry);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Update (virtual)

	Returns kJTrue if anything changed.

	If !force && *updateNode != nullptr, sets *updateNode to next node
	in depth-first search.

 ******************************************************************************/

JBoolean
JFSFileTreeNode::Update
	(
	const JBoolean			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	JBoolean changed = itsDirEntry->Update(force);
	if (changed)
		{
		GetTree()->BroadcastChange(this);
		}

	const JBoolean canHaveChildren = CanHaveChildren();
	ShouldBeOpenable(canHaveChildren);

	if (canHaveChildren && itsDirInfo != nullptr)
		{
		if (itsDirInfo->Update(force))
			{
			UpdateChildren();
			changed = kJTrue;
			}

		if (force || updateNode == nullptr || *updateNode == nullptr)
			{
			// check if subdirectories need updating

			if (JFSFileTreeNodeBase::Update(force, updateNode))
				{
				changed = kJTrue;
				}
			}
		}
	else if (!canHaveChildren)
		{
		if (itsDirInfo != nullptr)
			{
			changed = kJTrue;
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
	newChildren.SetCompareFunction(CompareTypeAndName);

	for (JDirEntry* e : *itsDirInfo)
		{
		JDirEntry* entry = jnew JDirEntry(*e);
		assert( entry != nullptr );
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
			newChildren.DeleteElement(j);
			}
		else
			{
			jdelete node;
			}
		}

	// save remaining new entries

	for (JTreeNode* n : newChildren)
		{
		InsertSorted(n);
		}

	// re-sort since mod time, size, etc. of *pre-existing* children may have changed

	SortChildren();
}

/******************************************************************************
 CompareTypeAndName (static private)

 ******************************************************************************/

JListT::CompareResult
JFSFileTreeNode::CompareTypeAndName
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	JFSFileTreeNode* n1	= dynamic_cast<JFSFileTreeNode*>(e1);
	JFSFileTreeNode* n2	= dynamic_cast<JFSFileTreeNode*>(e2);

	const long t = n1->itsDirEntry->GetType() - n2->itsDirEntry->GetType();
	if (t < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else if (t > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JNamedTreeNode::DynamicCastCompareNames(e1, e2);
		}
}

/******************************************************************************
 CompareUserName (static protected)

 ******************************************************************************/

JListT::CompareResult
JFSFileTreeNode::CompareUserName
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	JFSFileTreeNode* n1	= dynamic_cast<JFSFileTreeNode*>(e1);
	JFSFileTreeNode* n2	= dynamic_cast<JFSFileTreeNode*>(e2);

	JString u1 = n1->itsDirEntry->GetUserName();
	JString u2 = n2->itsDirEntry->GetUserName();
	JListT::CompareResult result =
		JCompareStringsCaseInsensitive(&(u1), &(u2));

	if (result == JListT::kFirstEqualSecond)
		{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
		}
	return result;
}

/******************************************************************************
 CompareGroup (static protected)

 ******************************************************************************/

JListT::CompareResult
JFSFileTreeNode::CompareGroupName
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	JFSFileTreeNode* n1	= dynamic_cast<JFSFileTreeNode*>(e1);
	JFSFileTreeNode* n2	= dynamic_cast<JFSFileTreeNode*>(e2);

	JString u1 = n1->itsDirEntry->GetGroupName();
	JString u2 = n2->itsDirEntry->GetGroupName();
	JListT::CompareResult result =
		JCompareStringsCaseInsensitive(&(u1), &(u2));

	if (result == JListT::kFirstEqualSecond)
		{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
		}
	return result;
}

/******************************************************************************
 CompareSize (static protected)

 ******************************************************************************/

JListT::CompareResult
JFSFileTreeNode::CompareSize
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	JFSFileTreeNode* n1	= dynamic_cast<JFSFileTreeNode*>(e1);
	JFSFileTreeNode* n2	= dynamic_cast<JFSFileTreeNode*>(e2);

	JListT::CompareResult result;

	if (n1->IsOpenable() && n2->IsOpenable())
		{
		result = JListT::kFirstEqualSecond;
		}
	else if (n1->IsOpenable())
		{
		result = JListT::kFirstLessSecond;
		}
	else if (n2->IsOpenable())
		{
		result = JListT::kFirstGreaterSecond;
		}
	else
		{
		result = JDirEntry::CompareSizes(n1->itsDirEntry, n2->itsDirEntry);
		}

	if (result == JListT::kFirstEqualSecond)
		{
		result = JNamedTreeNode::DynamicCastCompareNames(e1, e2);
		}
	return result;
}

/******************************************************************************
 CompareDate (static protected)

 ******************************************************************************/

JListT::CompareResult
JFSFileTreeNode::CompareDate
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	JFSFileTreeNode* n1	= dynamic_cast<JFSFileTreeNode*>(e1);
	JFSFileTreeNode* n2	= dynamic_cast<JFSFileTreeNode*>(e2);

	JListT::CompareResult result =
		JDirEntry::CompareModTimes(n1->itsDirEntry, n2->itsDirEntry);

	if (result == JListT::kFirstEqualSecond)
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
	JFSFileTreeNode* node = dynamic_cast<JFSFileTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const JFSFileTreeNode*
JFSFileTreeNode::GetFSChild
	(
	const JIndex index
	)
	const
{
	const JFSFileTreeNode* node = dynamic_cast<const JFSFileTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 GetDirInfo (virtual)

 ******************************************************************************/

JBoolean
JFSFileTreeNode::GetDirInfo
	(
	JDirInfo** info
	)
{
	*info = itsDirInfo;
	return JI2B( itsDirInfo != nullptr );
}

JBoolean
JFSFileTreeNode::GetDirInfo
	(
	const JDirInfo** info
	)
	const
{
	*info = itsDirInfo;
	return JI2B( itsDirInfo != nullptr );
}
