/******************************************************************************
 GNBToDoNode.cc

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GNBToDoNode.h"
#include "GNBGlobals.h"
#include "GNBNoteDir.h"
#include "GNBTreeDir.h"
#include "GNBTree.h"

#include <GNBApp.h>

#include <JXWindow.h>

#include <JColormap.h>
#include <JFontStyle.h>
#include <JString.h>
#include <JTree.h>

#include <jStreamUtil.h>
#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

GNBToDoNode::GNBToDoNode
	(
	JNamedTreeNode* parent,
	const char* 	name
	)
	:
	GNBBaseNode(parent, name, kJFalse, GNBBaseNode::kToDoNode)
{
	itsIsDone 	= kJFalse;
	itsHasNote	= kJFalse;
	itsIsUrgent	= kJFalse;
}

GNBToDoNode::GNBToDoNode
	(
	JTree* 			tree,
	const char* 	name
	)
	:
	GNBBaseNode(GNBBaseNode::kToDoNode, tree, name, kJFalse)
{
	itsIsDone 	= kJFalse;
	itsHasNote	= kJFalse;
	itsIsUrgent	= kJFalse;
}

GNBToDoNode::GNBToDoNode
	(
	const GNBToDoNode&	source,
	JTree* 				tree
	)
	:
	GNBBaseNode(GNBBaseNode::kToDoNode, tree, source.GetName(), kJFalse)
{
	itsIsDone 	= source.itsIsDone;
	itsHasNote	= source.itsHasNote;
	itsIsUrgent	= source.itsIsUrgent;

	if (itsHasNote)
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
}


/******************************************************************************
 Destructor

******************************************************************************/

GNBToDoNode::~GNBToDoNode()
{
}

/******************************************************************************
 SaveNode (public)

 ******************************************************************************/

void
GNBToDoNode::SaveNode
	(
	ostream& 			os
	)
{
	os << itsIsDone << ' ';
	os << itsIsUrgent << ' ';
	os << itsHasNote << ' ';
	if (itsHasNote)
		{
		os << itsFileID << ' ';
		}
	GNBBaseNode::SaveNode(os);
}

/******************************************************************************
 ReadNodeSetup (public)

 ******************************************************************************/

void
GNBToDoNode::ReadNodeSetup
	(
	istream& 			is,
	const JFileVersion 	version
	)
{
	is >> itsIsDone;
	if (version > 1)
		{
		is >> itsIsUrgent;
		}
	is >> itsHasNote;
	if (itsHasNote)
		{
		is >> itsFileID;
		}

	GNBBaseNode::ReadNodeSetup(is, version);
}

/******************************************************************************
 GetFontStyle (public)

 ******************************************************************************/

JFontStyle
GNBToDoNode::GetFontStyle()
	const
{
	if (itsIsDone)
		{
		return JFontStyle(kJFalse, kJFalse, 0, kJFalse, JGetCurrColormap()->GetGrayColor(50));
		}
	else if (itsIsUrgent)
		{
		return JFontStyle(kJFalse, kJFalse, 0, kJFalse, JGetCurrColormap()->GetRedColor());
		}
	return JFontStyle();
}

/******************************************************************************
 ToggleDone (public)

 ******************************************************************************/

void
GNBToDoNode::ToggleDone()
{
	itsIsDone = JNegate(itsIsDone);
}

/******************************************************************************
 AddNote (public)

 ******************************************************************************/

void
GNBToDoNode::AddNote()
{
	assert(!itsHasNote);
	
	itsFileID	= GetGNBTree()->GetDir()->NewNote();
	itsHasNote	= kJTrue;
}

/******************************************************************************
 RemoveNote (public)

 ******************************************************************************/

void
GNBToDoNode::RemoveNote()
{
	assert(itsHasNote);
	
	GetGNBTree()->GetDir()->DeleteFileElement(itsFileID);
	itsHasNote	= kJFalse;
}

/******************************************************************************
 DiscardNode (public)

 ******************************************************************************/

void
GNBToDoNode::DiscardNode()
{
	if (itsHasNote)
		{
		GetGNBTree()->GetDir()->DeleteFileElement(itsFileID);
		}
}

/******************************************************************************
 GetID (public)

 ******************************************************************************/

JBoolean
GNBToDoNode::GetID
	(
	JFAID_t* id
	)
{
	if (itsHasNote)
		{
		*id = itsFileID;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 ToggleUrgency (public)

 ******************************************************************************/

void
GNBToDoNode::ToggleUrgency()
{
	itsIsUrgent = JNegate(itsIsUrgent);
}

/******************************************************************************
 PrintNode (virtual public)

 ******************************************************************************/

void
GNBToDoNode::PrintNode
	(
	JString* 		text, 
	const JBoolean	notes
	)
{
	const JSize depth	= GetDepth();
	for (JIndex i = 1; i < depth; i++)
		{
		text->AppendCharacter(' ');
		text->AppendCharacter(' ');
		}
	if (IsDone())
		{
		text->AppendCharacter('x');
		}
	else
		{
		text->AppendCharacter('_');
		}
	text->AppendCharacter(' ');
	text->Append(GetName());
	text->AppendCharacter('\n');

	if (notes && itsHasNote)
		{
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
			text->Append("\n*** Note: ");
			text->Append(GetName());
			text->Append(" ***\n");
			text->Append(note);
			text->AppendCharacter('\n');
			}
		}
}
