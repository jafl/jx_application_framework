/******************************************************************************
 HistoryMenuPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_HistoryMenuPanel
#define _H_HistoryMenuPanel

#include "WidgetPanelBase.h"

class JString;
class JXInputField;

class HistoryMenuPanel : public WidgetPanelBase
{
public:

	HistoryMenuPanel(WidgetParametersDialog* dlog, const JString& length);

	~HistoryMenuPanel();

	void	GetValues(JString* length);

private:

// begin Panel

	JXInputField* itsHistoryLengthInput;

// end Panel
};

#endif
