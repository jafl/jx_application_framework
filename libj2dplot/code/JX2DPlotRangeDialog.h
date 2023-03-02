/******************************************************************************
 JX2DPlotRangeDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DPlotRangeDialog
#define _H_JX2DPlotRangeDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;
class JXFloatInput;
class JXStaticText;

class JX2DPlotRangeDialog : public JXModalDialogDirector
{

public:

	JX2DPlotRangeDialog(const JFloat xMax, const JFloat xMin,
						const JFloat yMax, const JFloat yMin);

	~JX2DPlotRangeDialog() override;

	bool GetRangeValues(JFloat* xMax, JFloat* xMin,
						JFloat* yMax, JFloat* yMin);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsClearRangeFlag;

// begin JXLayout

	JXFloatInput* itsXMin;
	JXFloatInput* itsXMax;
	JXFloatInput* itsYMin;
	JXFloatInput* itsYMax;
	JXTextButton* itsClearButton;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
