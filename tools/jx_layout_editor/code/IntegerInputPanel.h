/******************************************************************************
 IntegerInputPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_IntegerInputPanel
#define _H_IntegerInputPanel

#include "WidgetPanelBase.h"

class JXIntegerInput;
class JXTextCheckbox;

class IntegerInputPanel : public WidgetPanelBase
{
public:

	IntegerInputPanel(WidgetParametersDialog* dlog, const bool required,
					  const bool hasMin, const JInteger min,
					  const bool hasMax, const JInteger max);

	~IntegerInputPanel();

	bool	Validate() const override;

	void	GetValues(bool* required,
					  bool* hasMin, JInteger* min,
					  bool* hasMax, JInteger* max);

private:

// begin Panel

	JXTextCheckbox* itsIntRequiredCB;
	JXIntegerInput* itsIntMinValueInput;
	JXIntegerInput* itsIntMaxValueInput;

// end Panel
};

#endif
