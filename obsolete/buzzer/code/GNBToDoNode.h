/******************************************************************************
 GNBToDoNode.h

	Interface for GNBToDoNode class.

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBToDoNode
#define _H_GNBToDoNode

#include "GNBBaseNode.h"
#include <JFileArray.h>
#include <JFontStyle.h>
#include <iostream.h>

class JString;
class JTree;

class GNBToDoNode : public GNBBaseNode
{
public:

	GNBToDoNode(JNamedTreeNode* parent, const char* name);
	GNBToDoNode(JTree* tree, const char* name);
	GNBToDoNode(const GNBToDoNode& source, JTree* tree);

	virtual ~GNBToDoNode();

	virtual void		SaveNode(ostream& os);
	virtual void		ReadNodeSetup(istream& is, const JFileVersion version);
	virtual void		DiscardNode();
	virtual void		PrintNode(JString* text, const JBoolean notes);

	JBoolean			IsDone() const;
	void				ToggleDone();

	JFontStyle			GetFontStyle() const;

	JBoolean			GetID(JFAID_t* id);

	JBoolean			HasNote() const;
	void				AddNote();
	void				RemoveNote();

	JBoolean			IsUrgent() const;
	void				ToggleUrgency();

private:

	JBoolean 			itsIsDone;
	JBoolean			itsHasNote;
	JFAID_t				itsFileID;
	JBoolean			itsIsUrgent;
	
private:

	// not allowed

	GNBToDoNode(const GNBToDoNode& source);
	const GNBToDoNode& operator=(const GNBToDoNode& source);

};

/******************************************************************************
 IsDone (public)

 ******************************************************************************/

inline JBoolean
GNBToDoNode::IsDone()
	const
{
	return itsIsDone;
}

/******************************************************************************
 HasNote (public)

 ******************************************************************************/

inline JBoolean
GNBToDoNode::HasNote()
	const
{
	return itsHasNote;
}

/******************************************************************************
 IsUrgent (public)

 ******************************************************************************/

inline JBoolean
GNBToDoNode::IsUrgent()
	const
{
	return itsIsUrgent;
}

#endif
