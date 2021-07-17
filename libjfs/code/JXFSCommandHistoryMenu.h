/******************************************************************************
 JXFSCommandHistoryMenu.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSCommandHistoryMenu
#define _H_JXFSCommandHistoryMenu

#include <JXHistoryMenuBase.h>
#include "JFSBinding.h"		// need defn of CommandType

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

	virtual ~JXFSCommandHistoryMenu();

	void	AddCommand(const JString& cmd,
					   const bool isShell, const bool inWindow,
					   const bool singleFile);

	const JString&	GetCommand(const Message& message, JFSBinding::CommandType* type,
							   bool* singleFile) const;
	const JString&	GetCommand(const JIndex index, JFSBinding::CommandType* type,
							   bool* singleFile) const;

private:

	void	JFSCommandHistoryMenuX();

	// not allowed

	JXFSCommandHistoryMenu(const JXFSCommandHistoryMenu& source);
	const JXFSCommandHistoryMenu& operator=(const JXFSCommandHistoryMenu& source);
};

#endif
