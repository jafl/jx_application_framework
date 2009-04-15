/******************************************************************************
 UndoWidgetDir.h

	Interface for the UndoWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_UndoWidgetDir
#define _H_UndoWidgetDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXTextMenu;
class UndoWidget;

class UndoWidgetDir : public JXWindowDirector
{
public:

	UndoWidgetDir(JXDirector* supervisor);

	virtual ~UndoWidgetDir();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu* itsActionsMenu;
	UndoWidget*	itsWidget;

private:

	void UpdateActionsMenu();
	void HandleActionsMenu(const JIndex index);
	
	void BuildWindow();

	// not allowed

	UndoWidgetDir(const UndoWidgetDir& source);
	const UndoWidgetDir& operator=(const UndoWidgetDir& source);
};

#endif
