/******************************************************************************
 UndoWidgetDir.h

	Interface for the UndoWidgetDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_UndoWidgetDir
#define _H_UndoWidgetDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class UndoWidget;

class UndoWidgetDir : public JXWindowDirector
{
public:

	UndoWidgetDir(JXDirector* supervisor);

private:

	JXTextMenu* itsActionsMenu;
	UndoWidget*	itsWidget;

private:

	void UpdateActionsMenu();
	void HandleActionsMenu(const JIndex index);
	
	void BuildWindow();
};

#endif
