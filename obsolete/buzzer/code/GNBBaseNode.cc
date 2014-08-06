/******************************************************************************
 GNBBaseNode.cc

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GNBBaseNode.h"
#include "GNBTree.h"
#include <GNBCategoryNode.h>
#include <GNBGlobals.h>
#include <GNBNoteNode.h>
#include <GNBToDoNode.h>

#include <GCLAppointment.h>

#include <JString.h>
#include <JTreeList.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GNBBaseNode::GNBBaseNode
	(
	JNamedTreeNode* parent,
	const char* 	name,
	const JBoolean 	isBranch,
	Type 			type
	)
	:
	JNamedTreeNode(NULL, name, isBranch),
	itsAppt(NULL)
{
	itsType = type;
	if (parent != NULL)
		{
		parent->Append(this);
		}
}

GNBBaseNode::GNBBaseNode
	(
	Type 			type,
	JTree* 			tree,
	const char* 	name,
	const JBoolean 	isBranch
	)
	:
	JNamedTreeNode(tree, name, isBranch),
	itsAppt(NULL)
{
	itsType = type;
}

/******************************************************************************
 Create (public)

 ******************************************************************************/

GNBBaseNode*
GNBBaseNode::Create
	(
	const JTreeNode& 	source, 
	JTree*				tree
	)
{
	const GNBBaseNode* bnode	= dynamic_cast<const GNBBaseNode*>(&source);
	assert(bnode != NULL);
	GNBBaseNode::Type type	= bnode->GetType();
	GNBBaseNode* rnode;
	if (type == GNBBaseNode::kCategoryNode)
		{
		const GNBCategoryNode* cnode	= dynamic_cast<const GNBCategoryNode*>(bnode);
		assert(cnode != NULL);
		GNBCategoryNode* node	= new GNBCategoryNode(*cnode, tree);
		assert(node != NULL);
		rnode	= node;
		}
	else if (type == GNBBaseNode::kNoteNode)
		{
		const GNBNoteNode* nnode	= dynamic_cast<const GNBNoteNode*>(bnode);
		assert(nnode != NULL);
		GNBNoteNode* node	= new GNBNoteNode(*nnode, tree);
		assert(node != NULL);
		rnode	= node;
		}
	else if (type == GNBBaseNode::kToDoNode)
		{
		const GNBToDoNode* tdnode	= dynamic_cast<const GNBToDoNode*>(bnode);
		assert(tdnode != NULL);
		GNBToDoNode* node	= new GNBToDoNode(*tdnode, tree);
		assert(node != NULL);
		rnode	= node;
		}
	return rnode;
}


/******************************************************************************
 Destructor

******************************************************************************/

GNBBaseNode::~GNBBaseNode()
{
	delete itsAppt;
}

/******************************************************************************
 SaveNode (public)

 ******************************************************************************/

void
GNBBaseNode::SaveNode
	(
	ostream& 			os
	)
{
	if (!IsRoot())
		{
		os << GetName() << ' ';
		}
}

/******************************************************************************
 SaveBranch (public)

 ******************************************************************************/

void
GNBBaseNode::SaveBranch
	(
	ostream& 			os
	)
{
	assert(IsOpenable());
	SaveNode(os);
	const JSize count = GetChildCount();
	os << count << ' ';
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jnode = GetChild(i);
		GNBBaseNode* node = dynamic_cast<GNBBaseNode*>(jnode);
		assert(node != NULL);
		JBoolean branch = node->IsOpenable();
		os << branch << ' ';
		os << (int)node->GetType() << ' ';
		if (branch)
			{
			node->SaveBranch(os);
			}
		else
			{
			node->SaveNode(os);
			}
		}
}

/******************************************************************************
 ReadBranchSetup (public)

 ******************************************************************************/

void
GNBBaseNode::ReadBranchSetup
	(
	istream& 			is,
	const JFileVersion 	version
	)
{
	assert(IsOpenable());
	JTree* tree;
	JBoolean ok = GetTree(&tree);
	assert(ok);
	ReadNodeSetup(is, version);
	JSize count;
	is >> count;
	for (JSize i = 1; i <= count; i++)
		{
		JBoolean branch;
		is >> branch;
		if (version == 0)
			{
			if (branch)
				{
				GNBCategoryNode* node = 
					new GNBCategoryNode(tree, "");
				assert(node != NULL);
				Append(node);
				node->ReadBranchSetup(is,version);
				}
			else
				{
				GNBNoteNode* node = 
					new GNBNoteNode(tree, "", 0);
				assert(node != NULL);
				Append(node);
				node->ReadNodeSetup(is, version);
				}
			}
		else if (version > 0)
			{
			int type;
			is >> type;
			GNBBaseNode* bnode;
			if (type == kCategoryNode)
				{
				GNBCategoryNode* node = 
					new GNBCategoryNode(tree, "");
				assert(node != NULL);
				Append(node);
				bnode = node;
				}
			else if (type == kNoteNode)
				{
				GNBNoteNode* node = 
					new GNBNoteNode(tree, "", 0);
				assert(node != NULL);
				Append(node);
				bnode = node;
				}
			else if (type == kToDoNode)
				{
				GNBToDoNode* node = 
					new GNBToDoNode(tree, "");
				assert(node != NULL);
				Append(node);
				bnode = node;
				}
			if (branch)
				{
				bnode->ShouldBeOpenable(branch);
				bnode->ReadBranchSetup(is,version);
				}
			else
				{
				bnode->ReadNodeSetup(is, version);
				}
			}
		}
}

/******************************************************************************
 ReadNodeSetup (public)

 ******************************************************************************/

void
GNBBaseNode::ReadNodeSetup
	(
	istream& 			is,
	const JFileVersion 	version
	)
{
	if (IsRoot())
		{
		return;
		}
	JString name;
	is >> name;
	SetName(name);
}

/******************************************************************************
 DiscardNode (public)

 ******************************************************************************/

void
GNBBaseNode::DiscardNode()
{

}

/******************************************************************************
 DiscardBranch (public)

 ******************************************************************************/

void
GNBBaseNode::DiscardBranch()
{
	assert(IsOpenable());
	const JSize count = GetChildCount();
	for (JSize i = count; i >= 1; i--)
		{
		JTreeNode* jnode = GetChild(i);
		GNBBaseNode* node = dynamic_cast<GNBBaseNode*>(jnode);
		assert(node != NULL);
		JBoolean branch = node->IsOpenable();
		if (branch)
			{
			node->DiscardBranch();
			}
		else
			{
			node->DiscardNode();
			}
		delete node;
		}

}

/******************************************************************************
 FindChildNode (public)

 ******************************************************************************/

JBoolean
GNBBaseNode::FindChildNode
	(
	const JFAID_t 	id, 
	JTreeNode** 	node
	)
{
	assert(IsOpenable());
	const JSize count = GetChildCount();
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jnode = GetChild(i);
		JBoolean branch = jnode->IsOpenable();
		if (branch)
			{
			GNBBaseNode* bnode = dynamic_cast<GNBBaseNode*>(jnode);
			assert(bnode != NULL);
			JBoolean found = bnode->FindChildNode(id, node);
			if (found)
				{
				return found;
				}
			}
		else
			{
			GNBNoteNode* nnode = dynamic_cast<GNBNoteNode*>(jnode);
			if (nnode != NULL && nnode->GetID() == id)
				{
				*node = nnode;
				return kJTrue;
				}

			GNBToDoNode* tdnode = dynamic_cast<GNBToDoNode*>(jnode);
			JFAID_t id1;
			if (tdnode != NULL && tdnode->GetID(&id1) && id1 == id)
				{
				*node = tdnode;
				return kJTrue;
				}
			}
		}
	return kJFalse;
}

/******************************************************************************
 PrintNode (virtual public)

 ******************************************************************************/

void
GNBBaseNode::PrintNode
	(
	JString* 		text, 
	const JBoolean	notes
	)
{
	if (!IsRoot())
		{
		text->AppendCharacter('\n');
		const JSize depth	= GetDepth();
		for (JIndex i = 1; i < depth; i++)
			{
			text->AppendCharacter(' ');
			text->AppendCharacter(' ');
			}
		text->Append(GetName());
		text->AppendCharacter('\n');
		}
}

/******************************************************************************
 PrintNode (public)

 ******************************************************************************/

void
GNBBaseNode::PrintBranch
	(
	JString* 		text, 
	const JBoolean	notes,
	const JBoolean 	closed
	)
{
	PrintNode(text, notes);
	const JSize count = GetChildCount();
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jnode = GetChild(i);
		GNBBaseNode* node = dynamic_cast<GNBBaseNode*>(jnode);
		assert(node != NULL);
		JBoolean branch = node->IsOpenable();
		if (branch)
			{
			if ((closed || GNBGetCurrentPrintList()->IsOpen(node)) && 
				(notes || node->HasToDos()))
				{
				node->PrintBranch(text, notes, closed);
				}
			}
		else
			{
			node->PrintNode(text, notes);
			}
		}
}

/******************************************************************************
 HasToDos (private)

 ******************************************************************************/

JBoolean
GNBBaseNode::HasToDos()
{
	const JSize count = GetChildCount();
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jnode = GetChild(i);
		GNBBaseNode* node = dynamic_cast<GNBBaseNode*>(jnode);
		assert(node != NULL);
		if (node->GetType() == kToDoNode)
			{
			return kJTrue;
			}
		else if (node->GetType() == kCategoryNode)
			{
			if (node->HasToDos())
				{
				return kJTrue;
				}
			}
		}
	return kJFalse;
}

/******************************************************************************
 GetGNBTree (public)

 ******************************************************************************/

GNBTree*
GNBBaseNode::GetGNBTree()
{
	JTree* jtree	= GetTree();
	GNBTree* tree	= dynamic_cast<GNBTree*>(jtree);
	assert(tree != NULL);
	return tree;
}

/******************************************************************************
 GetGNBTree (public)

 ******************************************************************************/

const GNBTree*
GNBBaseNode::GetGNBTree()
	const
{
	const JTree* jtree	= GetTree();
	const GNBTree* tree	= dynamic_cast<const GNBTree*>(jtree);
	assert(tree != NULL);
	return tree;
}

/******************************************************************************
 GetAppointment (public)

 ******************************************************************************/

JBoolean
GNBBaseNode::GetAppointment
	(
	GCLAppointment** appt
	)
{
	if (itsAppt	== NULL)
		{
		return kJFalse;
		}
	*appt	= itsAppt;
	return kJTrue;
}

/******************************************************************************
 SetAppointment (public)

 We take ownership of the appointment passed in.

 ******************************************************************************/

void
GNBBaseNode::SetAppointment
	(
	GCLAppointment* appt
	)
{
	itsAppt	= appt;
}

/******************************************************************************
 ClearAppointment (public)

 ******************************************************************************/

void
GNBBaseNode::ClearAppointment()
{
	if (itsAppt != NULL)
		{
		delete itsAppt;
		}	
}
