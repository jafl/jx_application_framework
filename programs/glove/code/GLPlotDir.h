/******************************************************************************
 GLPlotDir.h

	Interface for the GLPlotDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLPlotDir
#define _H_GLPlotDir

#include <JXDocument.h>
#include <JPtrArray.h>
#include "GloveCurveStats.h"
#include <JXPM.h>

class GLPlotter;
class JString;
class JFunction;
class GLVarList;
class JXTextMenu;
class GLPlotFunctionDialog;
class GLFitBase;
class GLFitParmsDir;
class GLRaggedFloatTableData;
class JXFileDocument;
class JPlotDataBase;
class GLHistoryDir;
class JXPSPrinter;
class GLFitModuleDialog;
class GLPlotModuleFit;
class GLPlotFitFunction;
class GLPlotFitProxy;
class JX2DPlotEPSPrinter;

class GLPlotDir : public JXDocument
{
public:

	GLPlotDir(JXDirector* supervisor, JXFileDocument* notifySupervisor,
			const JString& filename, const JBoolean hideOnClose = kJFalse);

	virtual ~GLPlotDir();

	GLPlotter*			GetPlot();
	void 				NewFileName(const JString& filename);

	void 				WriteSetup(std::ostream& os);
	void 				ReadSetup(std::istream& is, const JFloat gloveVersion);

	void 				WriteData(std::ostream& os, GLRaggedFloatTableData* data);
	void 				ReadData(std::istream& is, GLRaggedFloatTableData* data, const JFloat gloveVersion);
	virtual JBoolean	NeedsSave() const override;
	GLHistoryDir*		GetSessionDir();
	JBoolean			AddFitModule(GLPlotModuleFit* fit, JPlotDataBase* fitData);
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason) override;
	virtual JBoolean	GetMenuIcon(const JXImage** icon) const override;

	void				AddFitProxy(GLPlotFitProxy* fit, const JIndex index, const JString& name);

	virtual JBoolean	Close() override;

	JBoolean			CurveIsFit(const JIndex index) const;

protected:

	virtual JBoolean	OKToClose() override;
	virtual JBoolean	OKToRevert() override;
	virtual JBoolean	CanRevert() override;
	virtual void		DiscardChanges() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	GLPlotter* 					itsPlot;
	JString 					itsFileName;
	GLVarList*					itsVarList;
	JXTextMenu*					itsPlotMenu;
	JXTextMenu*					itsAnalysisMenu;
	JXTextMenu*					itsFitParmsMenu;
	JXTextMenu*					itsDiffMenu;
	JXTextMenu*					itsHelpMenu;
	JIndex						itsXVarIndex;
	GLPlotFunctionDialog*		itsFunctionDialog;
	JPtrArray<GLFitBase>*		itsFits;
	GLFitParmsDir*				itsFitParmsDir;
	JArray<GloveCurveStats>*	itsCurveStats;
	GCurveType					itsCurrentCurveType;
	JXFileDocument*				itsSupervisor;
	JPtrArray<GLPlotDir>*		itsDiffDirs;
	JBoolean					itsHideOnClose;
	GLHistoryDir*				itsSessionDir;
	JXPSPrinter*				itsPrinter;
	JX2DPlotEPSPrinter*			itsEPSPrinter;
	JBoolean					itsIsPrintAll;
	GLFitModuleDialog*			itsFitModuleDialog;
	JIndex						itsCurveForFit;

	JBoolean					itsPlotIsClosing;

private:

	void	WriteCurves(std::ostream& os, GLRaggedFloatTableData* data);
	void 	ReadCurves(std::istream& is, GLRaggedFloatTableData* data);

	void	HandlePlotMenu(const JIndex index);

	void	HandleAnalysisMenu(const JIndex index);
	void	CreateFunction();
	void 	PlotFunction(JFunction* f);
	void	SelectFitModule();
	void	UpdateFitParmsMenu();

	void	UpdateDiffMenu();

	void	HandleHelpMenu(const JIndex index);

	void	HandleCurveRemoved(const JIndex index);
	void	HandleCurveAdded();
	void	RemoveFit(const JIndex index);
	void	NewFit(const JIndex plotindex, const GCurveFitType type);
	void	AddDiffCurve(JPlotDataBase* ddata);
	void	AddFit(GLPlotFitFunction* fit, const JIndex plotIndex,
					const GCurveFitType type);

	// not allowed

	GLPlotDir(const GLPlotDir& source);
	const GLPlotDir& operator=(const GLPlotDir& source);
};

/******************************************************************************
 GetPlot

 ******************************************************************************/

inline GLPlotter*
GLPlotDir::GetPlot()
{
	return itsPlot;
}

/******************************************************************************
 GetSessionDir

 ******************************************************************************/

inline GLHistoryDir*
GLPlotDir::GetSessionDir()
{
	return itsSessionDir;
}

#endif
