/******************************************************************************
 JX2DPlotScaleDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DPlotScaleDialog
#define _H_JX2DPlotScaleDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXFloatInput;
class JXRadioGroup;
class JXStaticText;

class JX2DPlotScaleDialog : public JXModalDialogDirector
{
public:

	enum
	{
		kLinear = 1,
		kLog
	};

public:

	JX2DPlotScaleDialog(const JFloat xMin, const JFloat xMax, const JFloat xInc,
						const bool xLinear,
						const JFloat yMin, const JFloat yMax, const JFloat yInc,
						const bool yLinear,
						const bool editXAxis);

	~JX2DPlotScaleDialog() override;

	void	Activate() override;

	void GetScaleValues(JFloat* xMin, JFloat* xMax, JFloat* xInc,
						bool* xLinear,
						JFloat* yMin, JFloat* yMax, JFloat* yInc,
						bool* yLinear);

protected:

	bool	OKToDeactivate() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const bool itsEditXAxisFlag;

// begin JXLayout

	JXRadioGroup* itsXAxisTypeRG;
	JXStaticText* itsXIncLabel;
	JXRadioGroup* itsYAxisTypeRG;
	JXStaticText* itsYIncLabel;
	JXFloatInput* itsXMin;
	JXFloatInput* itsXMax;
	JXFloatInput* itsXInc;
	JXFloatInput* itsYMin;
	JXFloatInput* itsYMax;
	JXFloatInput* itsYInc;

// end JXLayout

private:

	void	BuildWindow();
	void	AdjustXScaleActivation();
	void	AdjustYScaleActivation();
};

#endif
