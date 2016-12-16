/******************************************************************************
 GNBNoteNode.cc

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GNBNoteNode.h"
#include "GNBGlobals.h"
#include "GNBNoteDir.h"
#include "GNBTreeDir.h"
#include "GNBTree.h"

#include <GNBApp.h>

#include <JXWindow.h>

#include <JString.h>

#include <jStreamUtil.h>
#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GNBNoteNode::GNBNoteNode
	(
	JNamedTreeNode* parent,
	const char* 	name,
	const JFAID_t 	id
	)
	:
	GNBBaseNode(parent, name, kJFalse, GNBBaseNode::kNoteNode),
	itsFileID(id),
	itsHasName(kJFalse)
{
}

GNBNoteNode::GNBNoteNode
	(
	JTree* 			tree,
	const char* 	name,
	const JFAID_t 	id
	)
	:
	GNBBaseNode(GNBBaseNode::kNoteNode, tree, name, kJFalse),
	itsFileID(id),
	itsHasName(kJFalse)
{
}

GNBNoteNode::GNBNoteNode
	(
	const GNBNoteNode&	source,
	JTree* 				tree
	)
	:
	GNBBaseNode(GNBBaseNode::kNoteNode, tree, source.GetName(), kJFalse),
	itsHasName(source.itsHasName)
{
	std::string data;
	JFAID jid(source.itsFileID);
	source.GetGNBTree()->GetDir()->GetFileArray()->GetElement(jid, &data);
	JFileArray* fa	= GetGNBTree()->GetDir()->GetFileArray();
	fa->AppendElement(data.c_str());
	JFAID id; 
	fa->IndexToID(fa->GetElementCount(), &id);
	itsFileID	= id.GetID();
}


/******************************************************************************
 Destructor

******************************************************************************/

GNBNoteNode::~GNBNoteNode()
{
}

/******************************************************************************
 SaveNode (public)

 ******************************************************************************/

void
GNBNoteNode::SaveNode
	(
	std::ostream& 			os
	)
{
	os << itsFileID << ' ';
	os << itsHasName << ' ';

	GNBBaseNode::SaveNode(os);
}

/******************************************************************************
SetHasName (public)

 ******************************************************************************/

void
GNBNoteNode::SetHasName
	(
	const JBoolean hasname
	)
{
	itsHasName = hasname;
}

/******************************************************************************
 ReadNodeSetup (public)

 ******************************************************************************/

void
GNBNoteNode::ReadNodeSetup
	(
	std::istream& 			is,
	const JFileVersion 	version
	)
{
	is >> itsFileID;
	is >> itsHasName;
	GNBBaseNode::ReadNodeSetup(is, version);
}

/******************************************************************************
 DiscardNode (public)

 ******************************************************************************/

void
GNBNoteNode::DiscardNode()
{
	GetGNBTree()->GetDir()->DeleteFileElement(itsFileID);
}

/******************************************************************************
 AdjustName (public)

 ******************************************************************************/

void
GNBNoteNode::AdjustName
	(
	const JString& newName
	)
{
	if (!itsHasName && (newName != GetName()))
		{
		SetName(newName);
		}
}

/******************************************************************************
 PrintNode (virtual public)

 ******************************************************************************/

void
GNBNoteNode::PrintNode
	(
	JString* 		text, 
	const JBoolean	notes
	)
{
	if (notes)
		{
		GNBBaseNode::PrintNode(text, notes);
		GNBNoteDir* dir	= GNBGetCurrentPrintDir();
		std::string data;
		JFAID jid(itsFileID);
		GetGNBTree()->GetDir()->GetFileArray()->GetElement(jid, &data);
		if (!data.length() == 0)
			{
			std::istringstream is(data);
			dir->GetWindow()->ReadGeometry(is);
			JString note;
			JReadAll(is, &note);
			text->Append("*** Note: ");
			JString name	= GetName();
			name.TrimWhitespace();
			text->Append(name);
			text->Append(" ***\n\n");
			text->Append(note);
			text->AppendCharacter('\n');
			}
		}
}
