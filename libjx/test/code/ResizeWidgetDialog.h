/******************************************************************************
 ResizeWidgetDialog.h

	Interface for the ResizeWidgetDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_ResizeWidgetDialog
#define _H_ResizeWidgetDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXWidget;
class JXIntegerInput;

class ResizeWidgetDialog : public JXDialogDirector
{
public:

	ResizeWidgetDialog(JXWindowDirector* supervisor, const JXWidget* widget);

	virtual ~ResizeWidgetDialog();

	void	GetNewSize(JCoordinate* w, JCoordinate* h) const;

private:

// begin JXLayout

    JXIntegerInput* itsWidth;
    JXIntegerInput* itsHeight;

// end JXLayout

private:

	void	BuildWindow(const JXWidget* widget);

	// not allowed

	ResizeWidgetDialog(const ResizeWidgetDialog& source);
	const ResizeWidgetDialog& operator=(const ResizeWidgetDialog& source);
};

#endif
