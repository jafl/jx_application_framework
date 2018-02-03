/******************************************************************************
 GMailFileTreeNode.h

	Interface for GMailFileTreeNode class.

	Copyright <A9> 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMailFileTreeNode
#define _H_GMailFileTreeNode

#include <JFSFileTreeNode.h>

class JDirEntry;
class GMFileTree;

class GMailFileTreeNode : public JFSFileTreeNode
{
public:

	GMailFileTreeNode(JDirEntry* entry);

	virtual ~GMailFileTreeNode();

	JBoolean			HasNewMail() const;
	virtual JBoolean	Update(const JBoolean force);

protected:

	virtual JFSFileTreeNode*	CreateChild(JDirEntry* entry);

private:

	JBoolean	itsHasNewMail;

private:

	void GMailFileTreeNodeX();

	// not allowed

	GMailFileTreeNode(const GMailFileTreeNode& source);
	const GMailFileTreeNode& operator=(const GMailFileTreeNode& source);

};

/******************************************************************************
 HasChanged (public)

 ******************************************************************************/

inline JBoolean
GMailFileTreeNode::HasNewMail()
	const
{
	return itsHasNewMail;
}

#endif
