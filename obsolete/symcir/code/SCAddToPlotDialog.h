/******************************************************************************
 SCAddToPlotDialog.h

	Interface for the SCAddToPlotDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCAddToPlotDialog
#define _H_SCAddToPlotDialog

#include <JXDialogDirector.h>

class JString;
class JXTextMenu;
class JXInputField;
class JXFloatInput;
class SCCircuitDocument;

class SCAddToPlotDialog : public JXDialogDirector
{
public:

	SCAddToPlotDialog(SCCircuitDocument* supervisor);

	virtual ~SCAddToPlotDialog();

	void	GetSettings(JIndex* plotIndex, JString* curveName,
						JFloat* fMin, JFloat* fMax) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsPlotIndex;

// begin JXLayout

	JXInputField* itsCurveName;
	JXFloatInput* itsMinValue;
	JXFloatInput* itsMaxValue;
	JXTextMenu*   itsPlotMenu;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);

	// not allowed

	SCAddToPlotDialog(const SCAddToPlotDialog& source);
	const SCAddToPlotDialog& operator=(const SCAddToPlotDialog& source);
};

#endif
