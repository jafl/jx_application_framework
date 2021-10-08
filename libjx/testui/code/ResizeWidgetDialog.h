/******************************************************************************
 ResizeWidgetDialog.h

	Interface for the ResizeWidgetDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_ResizeWidgetDialog
#define _H_ResizeWidgetDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXWidget;
class JXIntegerInput;

class ResizeWidgetDialog : public JXDialogDirector
{
public:

	ResizeWidgetDialog(JXWindowDirector* supervisor, const JXWidget* widget);

	~ResizeWidgetDialog();

	void	GetNewSize(JCoordinate* w, JCoordinate* h) const;

private:

// begin JXLayout

	JXIntegerInput* itsWidth;
	JXIntegerInput* itsHeight;

// end JXLayout

private:

	void	BuildWindow(const JXWidget* widget);
};

#endif
