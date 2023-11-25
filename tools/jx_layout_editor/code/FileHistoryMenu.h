/******************************************************************************
 FileHistoryMenu.h

	Interface for the FileHistoryMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileHistoryMenu
#define _H_FileHistoryMenu

#include <jx-af/jx/JXFileHistoryMenu.h>

class FileHistoryMenu : public JXFileHistoryMenu
{
public:

	FileHistoryMenu(const JString& title, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	FileHistoryMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~FileHistoryMenu() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	void	FileHistoryMenuX();
};

#endif
