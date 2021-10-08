/******************************************************************************
 JXFSCommandHistoryMenu.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSCommandHistoryMenu
#define _H_JXFSCommandHistoryMenu

#include <jx-af/jx/JXHistoryMenuBase.h>
#include "jx-af/jfs/JFSBinding.h"		// need defn of CommandType

class JXFSCommandHistoryMenu : public JXHistoryMenuBase
{
public:

	JXFSCommandHistoryMenu(const JSize historyLength,
						  const JString& title, JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	JXFSCommandHistoryMenu(const JSize historyLength,
						  JXMenu* owner, const JIndex itemIndex,
						  JXContainer* enclosure);

	~JXFSCommandHistoryMenu();

	void	AddCommand(const JString& cmd,
					   const bool isShell, const bool inWindow,
					   const bool singleFile);

	const JString&	GetCommand(const Message& message, JFSBinding::CommandType* type,
							   bool* singleFile) const;
	const JString&	GetCommand(const JIndex index, JFSBinding::CommandType* type,
							   bool* singleFile) const;

private:

	void	JFSCommandHistoryMenuX();
};

#endif
