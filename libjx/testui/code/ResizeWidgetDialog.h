/******************************************************************************
 ResizeWidgetDialog.h

	Interface for the ResizeWidgetDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_ResizeWidgetDialog
#define _H_ResizeWidgetDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXWidget;
class JXIntegerInput;

class ResizeWidgetDialog : public JXModalDialogDirector
{
public:

	ResizeWidgetDialog(const JXWidget* widget);

	~ResizeWidgetDialog() override;

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
