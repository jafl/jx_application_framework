/******************************************************************************
 JContainer.h

	Interface for the JContainer class

	Copyright © 1994 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JContainer
#define _H_JContainer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCollection.h>

class JContainer : public JCollection
{
public:

	JContainer();
	JContainer(const JContainer& source);

	const JContainer& operator=(const JContainer& source);

protected:

	void				InstallOrderedSet(JCollection* theOrderedSet);
	const JCollection*	GetOrderedSet();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JCollection*	itsOrderedSet;
};


/******************************************************************************
 GetOrderedSet

 ******************************************************************************/

inline const JCollection*
JContainer::GetOrderedSet()
{
	return itsOrderedSet;
}

#endif
