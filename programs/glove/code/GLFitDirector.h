/******************************************************************************
 GLFitDirector.h

	Interface for the GLFitDirector class

	Copyright © 2000 by Glenn W. Bach. All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GLFitDirector
#define _H_GLFitDirector

//includes

#include <JXWindowDirector.h>

class GLParmColHeaderWidget;
class JXExprWidget;
class JXHorizPartition;
class JXVertPartition;
class JXImage;
class JXPSPrinter;
class JXStaticText;
class JXTextMenu;
class JXToolBar;

class JPlotFitFunction;
class J2DPlotJFunction;
class J2DPlotWidget;
class JX2DPlotWidget;

class GLCurveNameList;
class GLFitDescriptionList;
class GLFitParameterTable;
class GLNonLinearFitDialog;
class GLPolyFitDialog;
class PlotDir;

class GloveHistoryDir;

class GVarList;

class GLFitDirector : public JXWindowDirector
{
public:

public:

	GLFitDirector(PlotDir* supervisor, J2DPlotWidget* plot, const JCharacter* file);
	virtual ~GLFitDirector();

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*		itsFitMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;
	JXToolBar*		itsToolBar;
	J2DPlotWidget*	itsPlot;
	
	GLCurveNameList*		itsCurveList;
	GLFitDescriptionList*	itsFitList;
	GLFitParameterTable*	itsParameterTable;
	GLParmColHeaderWidget*	itsParameterColHeader;
	JX2DPlotWidget*			itsFitPlot;
	JX2DPlotWidget*			itsDiffPlot;
	JXStaticText*			itsChiSq;
	JPlotFitFunction*		itsCurrentFit;
	J2DPlotJFunction*		itsTestFunction;

	JXHorizPartition*		itsMainPartition;
	JXVertPartition*		itsListPartition;
	JXVertPartition*		itsPlotPartition;

	GloveHistoryDir*		itsHistory;

	JXExprWidget*			itsExprWidget;
	GVarList*				itsExprVarList;

	GLNonLinearFitDialog*	itsNLFitDialog;
	GLPolyFitDialog*		itsPolyFitDialog;

	PlotDir*				itsDir;
	JXPSPrinter*			itsPrinter;

private:

	void Fit();
	void TestFit();
	void Refit();
	void Plot();
	void Print();

	void AddHistoryText(const JBoolean refit = kJFalse);

	void RemoveCurves();
	void RemoveFit();

	void UpdateFitMenu();
	void HandleFitMenu(const JIndex index);

	void HandlePrefsMenu(const JIndex index);

	void HandleHelpMenu(const JIndex index);

	void BuildWindow();

	// not allowed

	GLFitDirector(const GLFitDirector& source);
	const GLFitDirector& operator=(const GLFitDirector& source);

};


#endif
