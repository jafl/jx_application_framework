/******************************************************************************
 ColorChooserPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ColorChooserPanel
#define _H_ColorChooserPanel

#include "WidgetPanelBase.h"
#include <jx-af/jcore/jColor.h>

class JXTextButton;

class ColorChooserPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	ColorChooserPanel(WidgetParametersDialog* dlog, const JColorID color);

	~ColorChooserPanel();

	void	GetValues(JColorID* color);

private:

// begin Panel

	JXTextButton* itsColorSample;

// end Panel
};

#endif
