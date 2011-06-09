/******************************************************************************
 GNBBaseNode.h

	Interface for GNBBaseNode class.

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBBaseNode
#define _H_GNBBaseNode

#include "JNamedTreeNode.h"

#include <JFileArray.h>
#include <iostream.h>

class GNBTree;
class JString;
class GCLAppointment;

class GNBBaseNode : public JNamedTreeNode
{
public:

	enum Type
	{
		kBaseNode = 1,
		kRootNode,
		kCategoryNode,
		kNoteNode,
		kToDoNode
	};

public:

	GNBBaseNode(JNamedTreeNode* parent,
						const char* name,
						const JBoolean isBranch,
						Type type);

	GNBBaseNode(Type type, JTree* tree, 
						const JCharacter* name,
				   		const JBoolean isBranch = kJTrue);

	static GNBBaseNode*	Create(const JTreeNode& source, JTree* tree);

	virtual ~GNBBaseNode();

	virtual void	SaveNode(ostream& os);
	void			SaveBranch(ostream& os);
	void			ReadBranchSetup(istream& is, const JFileVersion version);
	virtual void	ReadNodeSetup(istream& is, const JFileVersion version);
	virtual void	DiscardNode();
	void			DiscardBranch();
	virtual void	PrintNode(JString* text, const JBoolean notes);
	void			PrintBranch(JString* text, const JBoolean notes, const JBoolean closed);

	JBoolean		FindChildNode(const JFAID_t id, JTreeNode** node);
	Type			GetType() const;

	GNBTree*		GetGNBTree();
	const GNBTree*	GetGNBTree() const;

	JBoolean		HasAlarm() const;
	JBoolean		GetAppointment(GCLAppointment** appt);
	void			SetAppointment(GCLAppointment* appt); // we own appt now
	void			ClearAppointment();

protected:

private:

	Type				itsType; 
	GCLAppointment*		itsAppt;

private:

	JBoolean		HasToDos();

	// not allowed

	GNBBaseNode(const GNBBaseNode& source);
	const GNBBaseNode& operator=(const GNBBaseNode& source);

};

/******************************************************************************
 GetType (public)

 ******************************************************************************/

inline GNBBaseNode::Type
GNBBaseNode::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 HasAlarm (public)

 ******************************************************************************/

inline JBoolean
GNBBaseNode::HasAlarm()
	const
{
	return JI2B(itsAppt != NULL);
}


#endif
