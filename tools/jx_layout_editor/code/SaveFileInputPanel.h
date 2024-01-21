/******************************************************************************
 SaveFileInputPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_SaveFileInputPanel
#define _H_SaveFileInputPanel

#include "WidgetPanelBase.h"

class JXTextCheckbox;

class SaveFileInputPanel : public WidgetPanelBase
{
public:

	SaveFileInputPanel(WidgetParametersDialog* dlog, const bool spaces);

	~SaveFileInputPanel();

	void	GetValues(bool* spaces);

private:

// begin Panel

	JXTextCheckbox* itsAllowSpacesCB;

// end Panel
};

#endif
