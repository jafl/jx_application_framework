/******************************************************************************
 JXFSCommandHistoryMenu.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFSCommandHistoryMenu
#define _H_JXFSCommandHistoryMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXHistoryMenuBase.h>
#include "JFSBinding.h"		// need defn of CommandType

class JXFSCommandHistoryMenu : public JXHistoryMenuBase
{
public:

	JXFSCommandHistoryMenu(const JSize historyLength,
						  const JCharacter* title, JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	JXFSCommandHistoryMenu(const JSize historyLength,
						  JXMenu* owner, const JIndex itemIndex,
						  JXContainer* enclosure);

	virtual ~JXFSCommandHistoryMenu();

	void	AddCommand(const JCharacter* cmd,
					   const JBoolean isShell, const JBoolean inWindow,
					   const JBoolean singleFile);

	const JString&	GetCommand(const Message& message, JFSBinding::CommandType* type,
							   JBoolean* singleFile) const;
	const JString&	GetCommand(const JIndex index, JFSBinding::CommandType* type,
							   JBoolean* singleFile) const;

private:

	void	JFSCommandHistoryMenuX();

	// not allowed

	JXFSCommandHistoryMenu(const JXFSCommandHistoryMenu& source);
	const JXFSCommandHistoryMenu& operator=(const JXFSCommandHistoryMenu& source);
};

#endif
