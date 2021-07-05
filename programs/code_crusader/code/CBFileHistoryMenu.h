/******************************************************************************
 CBFileHistoryMenu.h

	Interface for the CBFileHistoryMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileHistoryMenu
#define _H_CBFileHistoryMenu

#include <JXFileHistoryMenu.h>
#include "CBDocumentManager.h"	// need definition of FileHistoryType

class CBFileHistoryMenu : public JXFileHistoryMenu
{
public:

	CBFileHistoryMenu(const CBDocumentManager::FileHistoryType type,
					  const JString& title, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	CBFileHistoryMenu(const CBDocumentManager::FileHistoryType type,
					  JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~CBFileHistoryMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const CBDocumentManager::FileHistoryType	itsDocType;

private:

	void	CBFileHistoryMenuX(const CBDocumentManager::FileHistoryType type);

	// not allowed

	CBFileHistoryMenu(const CBFileHistoryMenu& source);
	const CBFileHistoryMenu& operator=(const CBFileHistoryMenu& source);
};

#endif
