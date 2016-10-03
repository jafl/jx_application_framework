/******************************************************************************
 GMAccountMenu.h

	Interface for the GMAccountMenu class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMAccountMenu
#define _H_GMAccountMenu

#include <JXTextMenu.h>

class GMAccountMenu : public JXTextMenu
{
public:

	GMAccountMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);
	virtual ~GMAccountMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

private:

	void	AdjustItems();

	// not allowed

	GMAccountMenu(const GMAccountMenu& source);
	const GMAccountMenu& operator=(const GMAccountMenu& source);

public:

	static const JCharacter* kItemsChanged;

	class ItemsChanged : public JBroadcaster::Message
		{
		public:

			ItemsChanged()
				:
				JBroadcaster::Message(kItemsChanged)
				{ };
		};
};

#endif
