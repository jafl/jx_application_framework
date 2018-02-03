/******************************************************************************
 GNBNoteNode.h

	Interface for GNBNoteNode class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GNBNoteNode
#define _H_GNBNoteNode

#include "GNBBaseNode.h"
#include <JFileArray.h>
#include <iostream.h>

class JString;
//class std::ostream;

class GNBNoteNode : public GNBBaseNode
{
public:

	GNBNoteNode(JNamedTreeNode* parent, const char* name, const JFAID_t id);
	GNBNoteNode(JTree* tree, const JCharacter* name, const JFAID_t id);
	GNBNoteNode(const GNBNoteNode& source, JTree* tree);

	virtual ~GNBNoteNode();

	virtual void	SaveNode(std::ostream& os);
	virtual void	ReadNodeSetup(std::istream& is, const JFileVersion version);
	virtual void	DiscardNode();
	virtual void	PrintNode(JString* text, const JBoolean notes);
	JFAID_t			GetID() const;
	void			SetHasName(const JBoolean hasname);
	void			AdjustName(const JString& newname);

private:

	JFAID_t		itsFileID;
	JBoolean	itsHasName;
	
private:

	// not allowed

	GNBNoteNode(const GNBNoteNode& source);
	const GNBNoteNode& operator=(const GNBNoteNode& source);

};

/******************************************************************************
 GetID (public)

 ******************************************************************************/

inline JFAID_t
GNBNoteNode::GetID()
	const
{
	return itsFileID;
}

#endif
