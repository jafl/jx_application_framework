/******************************************************************************
 MenuPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_MenuPanel
#define _H_MenuPanel

#include "WidgetPanelBase.h"
#include "Menu.h"				// need defn of Type
#include <jx-af/jx/JXMenu.h>	// need defn of ArrowPosition, ArrowDirection

class JString;
class JXInputField;
class JXIntegerInput;

class MenuPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	MenuPanel(WidgetParametersDialog* dlog, const Menu::Type type,
			  const JString& title, const JXMenu::ArrowPosition pos,
			  const JXMenu::ArrowDirection dir, const JSize colCount);

	~MenuPanel();

	void	GetValues(JString* title, JXMenu::ArrowPosition* pos,
					  JXMenu::ArrowDirection* dir, JSize* colCount);

private:

	JIndex	itsArrowPositionIndex;
	JIndex	itsArrowDirectionIndex;

// begin Panel

	JXTextMenu*     itsArrowPositionMenu;
	JXTextMenu*     itsArrowDirectionMenu;
	JXInputField*   itsTitleInput;
	JXIntegerInput* itsColumnCountInput;

// end Panel
};

#endif
