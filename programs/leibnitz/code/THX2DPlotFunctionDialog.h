/******************************************************************************
 THX2DPlotFunctionDialog.h

	Interface for the THX2DPlotFunctionDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THX2DPlotFunctionDialog
#define _H_THX2DPlotFunctionDialog

#include <JXDialogDirector.h>

class JFunction;
class JXTextMenu;
class JXInputField;
class JXFloatInput;
class JXExprEditor;
class THX2DPlotDirector;
class THXVarList;

class THX2DPlotFunctionDialog : public JXDialogDirector
{
public:

	THX2DPlotFunctionDialog(JXDirector* supervisor, const THXVarList* varList,
							const THX2DPlotDirector* prevPlot = nullptr);
	THX2DPlotFunctionDialog(JXDirector* supervisor, const THXVarList* varList,
							const JFunction& f, const JString& curveName,
							const JFloat min, const JFloat max);

	virtual ~THX2DPlotFunctionDialog();

	void	GetSettings(JIndex* plotIndex,
						const JFunction** f, JString* curveName,
						JFloat* fMin, JFloat* fMax) const;

protected:

	virtual JBoolean	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex			itsPlotIndex;
	JXExprEditor*	itsExprWidget;

// begin JXLayout

	JXInputField* itsCurveName;
	JXFloatInput* itsMinInput;
	JXFloatInput* itsMaxInput;
	JXTextMenu*   itsPlotMenu;

// end JXLayout

private:

	void	BuildWindow(const THXVarList* varList, const THX2DPlotDirector* prevPlot);

	// not allowed

	THX2DPlotFunctionDialog(const THX2DPlotFunctionDialog& source);
	const THX2DPlotFunctionDialog& operator=(const THX2DPlotFunctionDialog& source);
};

#endif
