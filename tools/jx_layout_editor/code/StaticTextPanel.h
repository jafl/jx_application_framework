/******************************************************************************
 StaticTextPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_StaticTextPanel
#define _H_StaticTextPanel

#include <jx-af/jcore/JBroadcaster.h>
#include "WidgetPanelBase.h"

class JString;
class JXInputField;
class JXIntegerInput;
class JXRegexInput;
class JXTextCheckbox;
class WidgetParametersDialog;

class StaticTextPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	StaticTextPanel(WidgetParametersDialog* dlog, const JString& text,
					const bool label, const bool center,
					const bool wordWrap, const bool selectable, const bool styleable);

	~StaticTextPanel();

	void	GetValues(JString* text, bool* label, bool* center,
					  bool* wordWrap, bool* selectable, bool* styleable);

private:

// begin Panel

	JXTextCheckbox* itsLabelCB;
	JXTextCheckbox* itsCenterHorizCB;
	JXTextCheckbox* itsWordWrapCB;
	JXTextCheckbox* itsSelectableCB;
	JXTextCheckbox* itsStyleableCB;
	JXInputField*   itsTextInput;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, const JString& text,
						const bool label, const bool center,
						const bool wordWrap, const bool selectable, const bool styleable);

	void	UpdateDisplay() const;
};

#endif
