/******************************************************************************
 THX3DPlotFunctionDialog.h

	Copyright © 1998-2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THX3DPlotFunctionDialog
#define _H_THX3DPlotFunctionDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JFunction;
class JXInputField;
class JXFloatInput;
class JXIntegerInput;
class JXExprEditor;
class THX3DPlotDirector;
class THXVarList;

class THX3DPlotFunctionDialog : public JXDialogDirector
{
public:

	THX3DPlotFunctionDialog(JXDirector* supervisor, const THXVarList* varList,
							const THX3DPlotDirector* prevPlot = NULL);
	THX3DPlotFunctionDialog(JXDirector* supervisor, const THXVarList* varList,
							const JFunction& f, const JCharacter* surfaceName,
							const JFloat xMin, const JFloat xMax, const JSize xCount,
							const JFloat yMin, const JFloat yMax, const JSize yCount);

	virtual ~THX3DPlotFunctionDialog();

	void	GetSettings(const JFunction** f, JString* surfaceName,
						JFloat* xMin, JFloat* xMax, JSize* xCount,
						JFloat* yMin, JFloat* yMax, JSize* yCount) const;

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JXExprEditor*	itsExprWidget;

// begin JXLayout

    JXInputField*   itsSurfaceName;
    JXFloatInput*   itsXMinInput;
    JXFloatInput*   itsXMaxInput;
    JXIntegerInput* itsXCountInput;
    JXFloatInput*   itsYMinInput;
    JXFloatInput*   itsYMaxInput;
    JXIntegerInput* itsYCountInput;

// end JXLayout

private:

	void	BuildWindow(const THXVarList* varList);

	// not allowed

	THX3DPlotFunctionDialog(const THX3DPlotFunctionDialog& source);
	const THX3DPlotFunctionDialog& operator=(const THX3DPlotFunctionDialog& source);
};

#endif
