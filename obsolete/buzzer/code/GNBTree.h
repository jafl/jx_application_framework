/******************************************************************************
 GNBTree.h

	Interface for the GNBTree class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GNBTree
#define _H_GNBTree

#include <JTree.h>

class GNBTreeDir;

class GNBTree : public JTree
{
public:

	GNBTree(JTreeNode* root, GNBTreeDir* dir);
	virtual ~GNBTree();

	GNBTreeDir*			GetDir();
	const GNBTreeDir*	GetDir() const;

protected:

private:

	GNBTreeDir*		itsDir;

private:

	// not allowed

	GNBTree(const GNBTree& source);
	const GNBTree& operator=(const GNBTree& source);

};

/******************************************************************************
 GetDir (public)

 ******************************************************************************/

inline GNBTreeDir*
GNBTree::GetDir()
{
	return itsDir;
}

/******************************************************************************
 GetDir (public)

 ******************************************************************************/

inline const GNBTreeDir*
GNBTree::GetDir()
	const
{
	return itsDir;
}


#endif
