/******************************************************************************
 ImageChooserPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageChooserPanel
#define _H_ImageChooserPanel

#include "WidgetPanelBase.h"
#include <jx-af/jcore/JPtrArray-JString.h>

class JString;
class JXImageMenu;
class LayoutDocument;

class ImageChooserPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	ImageChooserPanel(WidgetParametersDialog* dlog, LayoutDocument* doc,
					  const JString& fullName);

	~ImageChooserPanel();

	void	GetValues(JString* fullName);

	static JString	ReadImageName(std::istream& input, LayoutDocument* doc);
	static void		WriteImageName(const JString& fullName, std::ostream& output);

private:

	JPtrArray<JString>*	itsImagePathList;
	JIndex				itsImageIndex;

// begin Panel

	JXImageMenu* itsImageMenu;

// end Panel
};

#endif
