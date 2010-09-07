/******************************************************************************
 PlotDir.h

	Interface for the PlotDir class

	Copyright © 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_PlotDir
#define _H_PlotDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDocument.h>
#include <JPtrArray.h>
#include "CurveStats.h"
#include <JXPM.h>

class GlovePlotter;
class JString;
class JFunction;
class GVarList;
class JXTextMenu;
class GXPlotFunctionDialog;
class JFitBase;
class FitParmsDir;
class GRaggedFloatTableData;
class JXFileDocument;
class JPlotDataBase;
class GloveHistoryDir;
class JXPSPrinter;
class FitModuleDialog;
class JPlotModuleFit;
class JPlotFitFunction;
class JPlotFitProxy;
class JX2DPlotEPSPrinter;

class PlotDir : public JXDocument
{
public:

	PlotDir(JXDirector* supervisor, JXFileDocument* notifySupervisor,
			const JCharacter* filename, const JBoolean hideOnClose = kJFalse);

	virtual ~PlotDir();

	GlovePlotter*		GetPlot();
	void 				NewFileName(const JCharacter* filename);

	void 				WriteSetup(ostream& os);
	void 				ReadSetup(istream& is, const JFloat gloveVersion);

	void 				WriteData(ostream& os, GRaggedFloatTableData* data);
	void 				ReadData(istream& is, GRaggedFloatTableData* data, const JFloat gloveVersion);
	virtual JBoolean	NeedsSave() const;
	GloveHistoryDir*	GetSessionDir();
	JBoolean			AddFitModule(JPlotModuleFit* fit, JPlotDataBase* fitData);
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason);
	virtual JXPM		GetMenuIcon() const;

	void				AddFitProxy(JPlotFitProxy* fit, const JIndex index, const JCharacter* name);

	virtual JBoolean	Close();

	JBoolean			CurveIsFit(const JIndex index) const;

protected:

	virtual JBoolean	OKToClose();
	virtual JBoolean	OKToRevert();
	virtual JBoolean	CanRevert();
	virtual void		DiscardChanges();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	GlovePlotter* 				itsPlot;
	JString 					itsFileName;
	GVarList*					itsVarList;
	JXTextMenu*					itsPlotMenu;
	JXTextMenu*					itsAnalysisMenu;
	JXTextMenu*					itsFitParmsMenu;
	JXTextMenu*					itsDiffMenu;
	JXTextMenu*					itsHelpMenu;
	JIndex						itsXVarIndex;
	GXPlotFunctionDialog*		itsFunctionDialog;
	JPtrArray<JFitBase>*		itsFits;
	FitParmsDir*				itsFitParmsDir;
	JArray<CurveStats>*			itsCurveStats;
	GCurveType					itsCurrentCurveType;
	JXFileDocument*				itsSupervisor;
	JPtrArray<PlotDir>*			itsDiffDirs;
	JBoolean					itsHideOnClose;
	GloveHistoryDir*			itsSessionDir;
	JXPSPrinter*				itsPrinter;
	JX2DPlotEPSPrinter*			itsEPSPrinter;
	JBoolean					itsIsPrintAll;
	FitModuleDialog*			itsFitModuleDialog;
	JIndex						itsCurveForFit;

	JBoolean					itsPlotIsClosing;

private:

	void	WriteCurves(ostream& os, GRaggedFloatTableData* data);
	void 	ReadCurves(istream& is, GRaggedFloatTableData* data);

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
	void	AddFit(JPlotFitFunction* fit, const JIndex plotIndex,
					const GCurveFitType type);

	// not allowed

	PlotDir(const PlotDir& source);
	const PlotDir& operator=(const PlotDir& source);
};

/******************************************************************************
 GetPlot

 ******************************************************************************/

inline GlovePlotter*
PlotDir::GetPlot()
{
	return itsPlot;
}

/******************************************************************************
 GetSessionDir

 ******************************************************************************/

inline GloveHistoryDir*
PlotDir::GetSessionDir()
{
	return itsSessionDir;
}

#endif
