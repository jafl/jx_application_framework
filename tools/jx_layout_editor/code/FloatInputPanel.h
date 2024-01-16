/******************************************************************************
 FloatInputPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_FloatInputPanel
#define _H_FloatInputPanel

#include "WidgetPanelBase.h"

class JXFloatInput;
class JXTextCheckbox;

class FloatInputPanel : public WidgetPanelBase
{
public:

	FloatInputPanel(WidgetParametersDialog* dlog, const bool required,
					const bool hasMin, const JFloat min,
					const bool hasMax, const JFloat max);

	~FloatInputPanel();

	bool	Validate() const override;

	void	GetValues(bool* required,
					  bool* hasMin, JFloat* min,
					  bool* hasMax, JFloat* max);

private:

// begin Panel

	JXTextCheckbox* itsFloatRequiredCB;
	JXFloatInput*   itsFloatMinValueInput;
	JXFloatInput*   itsFloatMaxValueInput;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const bool required,
						const bool hasMin, const JFloat min,
						const bool hasMax, const JFloat max);
};

#endif
