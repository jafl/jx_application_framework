/******************************************************************************
 ToolBarPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ToolBarPanel
#define _H_ToolBarPanel

#include "WidgetPanelBase.h"

class JString;
class JXInputField;
class JXTextCheckbox;

class ToolBarPanel : public WidgetPanelBase
{
public:

	ToolBarPanel(WidgetParametersDialog* dlog, const JString& prefsMgr,
				 const JString& prefID, const JString& menuBar);

	~ToolBarPanel();

	void	GetValues(JString* prefsMgr, JString* prefID, JString* menuBar);

private:

// begin Panel

	JXInputField* itsPrefsMgrInput;
	JXInputField* itsPrefIDInput;
	JXInputField* itsMenuBarInput;

// end Panel
};

#endif
