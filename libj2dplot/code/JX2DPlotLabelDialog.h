/******************************************************************************
 JX2DPlotLabelDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DPlotLabelDialog
#define _H_JX2DPlotLabelDialog

#include <jx-af/jx/JXDialogDirector.h>
#include "jx-af/j2dplot/J2DPlotWidget.h"

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

	~JX2DPlotLabelDialog();

	void	GetLabels(JString* title, JString* xLabel, JString* yLabel) const;
	JString	GetFontName() const;
	JSize	GetFontSize() const;

	void	Activate() override;

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
};

#endif
