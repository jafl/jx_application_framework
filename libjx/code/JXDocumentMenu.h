/******************************************************************************
 JXDocumentMenu.h

	Interface for the JXDocumentMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocumentMenu
#define _H_JXDocumentMenu

#include "JXTextMenu.h"

class JXDocumentManager;

class JXDocumentMenu : public JXTextMenu
{
public:

	JXDocumentMenu(const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXDocumentMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXDocumentMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDocumentManager*	itsDocMgr;

private:

	void	JXDocumentMenuX();

	// not allowed

	JXDocumentMenu(const JXDocumentMenu& source);
	const JXDocumentMenu& operator=(const JXDocumentMenu& source);
};

#endif
