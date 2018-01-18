/******************************************************************************
 GMPOPRetrieverMenu.h

	Interface for the GMPOPRetrieverMenu class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMPOPRetrieverMenu
#define _H_GMPOPRetrieverMenu

#include <JXTextMenu.h>

class GMPOPRetrieverMenu : public JXTextMenu
{
public:

	GMPOPRetrieverMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);
	GMPOPRetrieverMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);
	virtual ~GMPOPRetrieverMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

private:

	void	GMPOPRetrieverMenuX();
	void	AdjustItems();

	// not allowed

	GMPOPRetrieverMenu(const GMPOPRetrieverMenu& source);
	const GMPOPRetrieverMenu& operator=(const GMPOPRetrieverMenu& source);

public:
};

#endif
