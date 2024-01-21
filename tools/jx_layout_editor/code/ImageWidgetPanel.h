/******************************************************************************
 ImageWidgetPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageWidgetPanel
#define _H_ImageWidgetPanel

#include "WidgetPanelBase.h"
#include <jx-af/jcore/JPtrArray-JString.h>

class JString;
class JXImageMenu;
class LayoutDocument;

class ImageWidgetPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	ImageWidgetPanel(WidgetParametersDialog* dlog, LayoutDocument* doc,
					 const JString& fullName);

	~ImageWidgetPanel();

	void	GetValues(JString* fullName);

private:

	JIndex				itsImageIndex;
	JPtrArray<JString>*	itsImagePathList;

// begin Panel

	JXImageMenu* itsImageMenu;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog, LayoutDocument* doc,
						const JString& fullName);
};

#endif
