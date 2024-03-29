/******************************************************************************
 JXToolBarNode.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXToolBarNode
#define _H_JXToolBarNode

#include "jx-af/jcore/JNamedTreeNode.h"

class JXTextMenu;

class JXToolBarNode : public JNamedTreeNode
{
public:

	JXToolBarNode(JXTextMenu* menu, const JIndex index,
				  const bool hasSeparator, const bool checked,
				  JTree* tree, JNamedTreeNode* parent, const JString& name);

	~JXToolBarNode() override;

	JXTextMenu*	GetMenu() const;
	JIndex		GetIndex() const;
	bool		HasSeparator() const;
	bool		IsChecked() const;
	void		ToggleChecked();

	void	SetSeparator();

private:

	JXTextMenu*	itsMenu;
	JIndex		itsIndex;
	bool		itsHasSeparator;
	bool		itsIsChecked;
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
 SetSeparator (public)

 ******************************************************************************/

inline void
JXToolBarNode::SetSeparator()
{
	itsHasSeparator = true;
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
