/******************************************************************************
 JXToolBarNode.h

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXToolBarNode
#define _H_JXToolBarNode

#include "JNamedTreeNode.h"

class JXTextMenu;

class JXToolBarNode : public JNamedTreeNode
{
public:

	JXToolBarNode(JXTextMenu* menu, const JIndex index,
				  const JBoolean hasSeparator, const JBoolean checked,
				  JTree* tree, JNamedTreeNode* parent, const JString& name);

	virtual ~JXToolBarNode();

	JXTextMenu*	GetMenu() const;
	JIndex		GetIndex() const;
	JBoolean	HasSeparator() const;
	JBoolean	IsChecked() const;
	void		ToggleChecked();

private:

	JXTextMenu*	itsMenu;
	JIndex		itsIndex;
	JBoolean	itsHasSeparator;
	JBoolean	itsIsChecked;

private:

	// not allowed

	JXToolBarNode(const JXToolBarNode& source);
	const JXToolBarNode& operator=(const JXToolBarNode& source);
};


/******************************************************************************
 GetMenu (public)

 ******************************************************************************/

inline JXTextMenu*
JXToolBarNode::GetMenu()
	const
{
	return itsMenu;
}

/******************************************************************************
 GetIndex (public)

 ******************************************************************************/

inline JIndex
JXToolBarNode::GetIndex()
	const
{
	return itsIndex;
}

/******************************************************************************
 HasSeparator (public)

 ******************************************************************************/

inline JBoolean
JXToolBarNode::HasSeparator()
	const
{
	return itsHasSeparator;
}

/******************************************************************************
 IsChecked (public)

 ******************************************************************************/

inline JBoolean
JXToolBarNode::IsChecked()
	const
{
	return itsIsChecked;
}

#endif
