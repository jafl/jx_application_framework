/******************************************************************************
 JX2DPlotScaleDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DPlotScaleDialog
#define _H_JX2DPlotScaleDialog

#include <JXDialogDirector.h>

class JXFloatInput;
class JXRadioGroup;
class JXStaticText;

class JX2DPlotScaleDialog : public JXDialogDirector
{
public:

	enum
	{
		kLinear = 1,
		kLog
	};

public:

	JX2DPlotScaleDialog(JXWindowDirector* supervisor,
						const JFloat xMin, const JFloat xMax, const JFloat xInc,
						const bool xLinear,
						const JFloat yMin, const JFloat yMax, const JFloat yInc,
						const bool yLinear);

	virtual ~JX2DPlotScaleDialog();

	void GetScaleValues(JFloat* xMin, JFloat* xMax, JFloat* xInc,
						bool* xLinear,
						JFloat* yMin, JFloat* yMax, JFloat* yInc,
						bool* yLinear);

	void EditXAxis(const bool xAxis);

protected:

	virtual bool	OKToDeactivate() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXFloatInput* itsXMin;
	JXFloatInput* itsXMax;
	JXStaticText* itsXIncLabel;
	JXFloatInput* itsXInc;
	JXFloatInput* itsYMin;
	JXFloatInput* itsYMax;
	JXStaticText* itsYIncLabel;
	JXFloatInput* itsYInc;
	JXRadioGroup* itsXAxisTypeRG;
	JXRadioGroup* itsYAxisTypeRG;

// end JXLayout

private:

	void	BuildWindow();
	void	AdjustXScaleActivation();
	void	AdjustYScaleActivation();

	// not allowed

	JX2DPlotScaleDialog(const JX2DPlotScaleDialog& source);
	const JX2DPlotScaleDialog& operator=(const JX2DPlotScaleDialog& source);
};

#endif
