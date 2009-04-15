/******************************************************************************
 THX2DPlotFunctionDialog.h

	Interface for the THX2DPlotFunctionDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THX2DPlotFunctionDialog
#define _H_THX2DPlotFunctionDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
							const THX2DPlotDirector* prevPlot = NULL);
	THX2DPlotFunctionDialog(JXDirector* supervisor, const THXVarList* varList,
							const JFunction& f, const JCharacter* curveName,
							const JFloat min, const JFloat max);

	virtual ~THX2DPlotFunctionDialog();

	void	GetSettings(JIndex* plotIndex,
						const JFunction** f, JString* curveName,
						JFloat* fMin, JFloat* fMax) const;

protected:

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

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
