/******************************************************************************
 JXDocumentMenu.h

	Interface for the JXDocumentMenu class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDocumentMenu
#define _H_JXDocumentMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXDocumentManager;

class JXDocumentMenu : public JXTextMenu
{
public:

	JXDocumentMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXDocumentMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXDocumentMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXDocumentManager*	itsDocMgr;

private:

	void	JXDocumentMenuX();

	// not allowed

	JXDocumentMenu(const JXDocumentMenu& source);
	const JXDocumentMenu& operator=(const JXDocumentMenu& source);
};

#endif
