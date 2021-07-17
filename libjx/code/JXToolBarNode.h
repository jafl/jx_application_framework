/******************************************************************************
 JXToolBarNode.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXToolBarNode
#define _H_JXToolBarNode

#include "JNamedTreeNode.h"

class JXTextMenu;

class JXToolBarNode : public JNamedTreeNode
{
public:

	JXToolBarNode(JXTextMenu* menu, const JIndex index,
				  const bool hasSeparator, const bool checked,
				  JTree* tree, JNamedTreeNode* parent, const JString& name);

	virtual ~JXToolBarNode();

	JXTextMenu*	GetMenu() const;
	JIndex		GetIndex() const;
	bool	HasSeparator() const;
	bool	IsChecked() const;
	void		ToggleChecked();

private:

	JXTextMenu*	itsMenu;
	JIndex		itsIndex;
	bool	itsHasSeparator;
	bool	itsIsChecked;

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

inline bool
JXToolBarNode::HasSeparator()
	const
{
	return itsHasSeparator;
}

/******************************************************************************
 IsChecked (public)

 ******************************************************************************/

inline bool
JXToolBarNode::IsChecked()
	const
{
	return itsIsChecked;
}

#endif
