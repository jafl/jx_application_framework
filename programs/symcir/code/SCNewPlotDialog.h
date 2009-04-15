/******************************************************************************
 SCNewPlotDialog.h

	Interface for the SCNewPlotDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCNewPlotDialog
#define _H_SCNewPlotDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JString;
class JXInputField;
class JXFloatInput;

class SCNewPlotDialog : public JXDialogDirector
{
public:

	SCNewPlotDialog(JXWindowDirector* supervisor);

	virtual ~SCNewPlotDialog();

	void	GetSettings(JString* curveName, JFloat* fMin, JFloat* fMax) const;

private:

// begin JXLayout

    JXInputField* itsCurveName;
    JXFloatInput* itsMinValue;
    JXFloatInput* itsMaxValue;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	SCNewPlotDialog(const SCNewPlotDialog& source);
	const SCNewPlotDialog& operator=(const SCNewPlotDialog& source);
};

#endif
