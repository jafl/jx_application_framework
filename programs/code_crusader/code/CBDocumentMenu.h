/******************************************************************************
 CBDocumentMenu.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDocumentMenu
#define _H_CBDocumentMenu

#include <JXDocumentMenu.h>

class CBDocumentMenu : public JXDocumentMenu
{
public:

	CBDocumentMenu(const JCharacter* title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	CBDocumentMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~CBDocumentMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	CBDocumentMenu(const CBDocumentMenu& source);
	const CBDocumentMenu& operator=(const CBDocumentMenu& source);
};

#endif
