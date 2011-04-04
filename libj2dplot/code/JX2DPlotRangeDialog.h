/******************************************************************************
 JX2DPlotRangeDialog.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DPlotRangeDialog
#define _H_JX2DPlotRangeDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextButton;
class JXFloatInput;
class JXStaticText;

class JX2DPlotRangeDialog : public JXDialogDirector
{

public:

	JX2DPlotRangeDialog(JXWindowDirector* supervisor, const JFloat xMax,
						const JFloat xMin, const JFloat yMax,
						const JFloat yMin);

	virtual ~JX2DPlotRangeDialog();

	JBoolean GetRangeValues(JFloat* xMax, JFloat* xMin,
							JFloat* yMax, JFloat* yMin);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsClearRangeFlag;

// begin JXLayout

	JXFloatInput* itsXMin;
	JXFloatInput* itsXMax;
	JXFloatInput* itsYMin;
	JXFloatInput* itsYMax;
	JXTextButton* itsClearButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	JX2DPlotRangeDialog(const JX2DPlotRangeDialog& source);
	const JX2DPlotRangeDialog& operator=(const JX2DPlotRangeDialog& source);
};

#endif
