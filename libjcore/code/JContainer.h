/******************************************************************************
 JContainer.h

	Interface for the JContainer class

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JContainer
#define _H_JContainer

#include "jx-af/jcore/JCollection.h"

class JContainer : public JCollection
{
public:

	JContainer();
	JContainer(const JContainer& source);

	const JContainer& operator=(const JContainer& source);

protected:

	void				InstallCollection(JCollection* list);
	const JCollection*	GetList() const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JCollection*	itsList;
};


/******************************************************************************
 GetList

 ******************************************************************************/

inline const JCollection*
JContainer::GetList()
	const
{
	return itsList;
}

#endif
