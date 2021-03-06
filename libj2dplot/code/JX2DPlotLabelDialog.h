/******************************************************************************
 JX2DPlotLabelDialog.h

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DPlotLabelDialog
#define _H_JX2DPlotLabelDialog

#include <JXDialogDirector.h>
#include <J2DPlotWidget.h>

class JString;
class JXInputField;
class JXFontNameMenu;
class JXFontSizeMenu;

class JX2DPlotLabelDialog : public JXDialogDirector
{
public:

	JX2DPlotLabelDialog(JXWindowDirector* supervisor, const JString& title,
						const JString& xLabel, const JString& yLabel,
						const JString& font, const JSize size,
						const J2DPlotWidget::LabelSelection selection);

	virtual ~JX2DPlotLabelDialog();

	void	GetLabels(JString* title, JString* xLabel, JString* yLabel) const;
	JString	GetFontName() const;
	JSize	GetFontSize() const;

	virtual void	Activate();

private:

	J2DPlotWidget::LabelSelection itsSelection;

// begin JXLayout

	JXInputField*   itsPlotTitle;
	JXInputField*   itsXAxisLabel;
	JXInputField*   itsYAxisLabel;
	JXFontNameMenu* itsFontMenu;
	JXFontSizeMenu* itsSizeMenu;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	JX2DPlotLabelDialog(const JX2DPlotLabelDialog& source);
	const JX2DPlotLabelDialog& operator=(const JX2DPlotLabelDialog& source);
};

#endif
