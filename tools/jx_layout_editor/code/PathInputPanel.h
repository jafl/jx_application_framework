/******************************************************************************
 PathInputPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_PathInputPanel
#define _H_PathInputPanel

#include "WidgetPanelBase.h"

class JXTextCheckbox;

class PathInputPanel : public WidgetPanelBase
{
public:

	PathInputPanel(WidgetParametersDialog* dlog, const bool required,
					const bool invalid, const bool write);

	~PathInputPanel();

	void	GetValues(bool* required, bool* invalid, bool* write);

private:

// begin Panel

	JXTextCheckbox* itsPathRequiredCB;
	JXTextCheckbox* itsAllowInvalidPathCB;
	JXTextCheckbox* itsRequirePathWritableCB;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const bool required,
						const bool invalid, const bool write);
};

#endif
