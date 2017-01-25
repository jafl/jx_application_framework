/******************************************************************************
 PlotDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "PlotDir.h"
#include "GVarList.h"
#include "GXPlotFunctionDialog.h"
#include "JPlotLinearFit.h"
#include "JPlotQuadFit.h"
#include "JPlotFitFunction.h"
#include "JFitBase.h"
#include "FitParmsDir.h"
#include "JPlotDataBase.h"
#include "J2DPlotData.h"
#include "J2DVectorData.h"
#include "GRaggedFloatTableData.h"
#include "GloveHistoryDir.h"
#include "FitModule.h"
#include "FitModuleDialog.h"
#include "GLPlotApp.h"
#include "JPlotModuleFit.h"
#include "J2DPlotJFunction.h"
#include "GlovePlotter.h"
//#include "gloveplotdoc.h"
#include "GLGlobals.h"
#include "GLHelpText.h"
#include "GLClosePlotDirTask.h"
#include "GLFitDirector.h"
#include "JPlotFitProxy.h"

#include <JX2DPlotEPSPrinter.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXDocumentMenu.h>
#include <JXFileDocument.h>
#include <JXApplication.h>
#include <JXDialogDirector.h>
#include <JXPSPrinter.h>
#include <JXHelpDirector.h>

#include <JFunction.h>
#include <JFunctionWithVar.h>
#include <JExprNodeList.h>
#include <JFunctionType.h>
#include <JPtrArray-JString.h>
#include <JUserNotification.h>

#include <jParseFunction.h>

#include <jx_plain_file_small.xpm>

#include <jFStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 2;

static const JCharacter* kWindowListMenuTitleStr = "Windows";

// Plot menu

static const JCharacter* kPlotMenuTitleStr = "Plot";
static const JCharacter* kPlotMenuStr =
	"    Page setup..."
	"  | Print...         %k Meta-P"
	"  | Print session..."
	"%l| Print plot as EPS..."
	"  | Print marks as EPS..."
	"%l| Close            %k Meta-W";

enum
{
	kPageSetupCmd = 1,
	kPrintCmd,
	kPrintSessionCmd,
	kPrintPlotEPSCmd,
	kPrintMarksEPSCmd,
	kCloseCmd
};

// Analysis menu

static const JCharacter* kAnalysisMenuTitleStr = "Analysis";
static const JCharacter* kAnalysisMenuStr =
	"    Plot function..."
	"%l| Open fit window %k Meta-F"
	"%l| Fit parameters"
	"  | Residual plots";

enum
{
	kPlotFunctionCmd = 1,
	kFitWindowCmd,
	kFitParmsCmd,
	kDiffPlotCmd
};

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"About %l| Table of Contents | This window "
	"%l|Changes|Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

PlotDir::PlotDir
	(
	JXDirector*       supervisor,
	JXFileDocument*   notifySupervisor,
	const JCharacter* filename,
	const JBoolean    hideOnClose
	)
	:
	JXDocument(supervisor),
	itsFileName(filename)
{
	itsSupervisor       = notifySupervisor;
	itsHideOnClose      = hideOnClose;
	itsPlotIsClosing	= kJFalse;

	itsPrinter          = NULL;

	JCoordinate w 		= 500;
	JCoordinate h       = 400;
	JCoordinate minW	= 400;
	JCoordinate minH    = 300;

	JXWindow* window = jnew JXWindow(this, w,h, filename);
	assert( window != NULL );

	window->SetMinSize(minW,minH);
	if (hideOnClose)
		{
		window->SetCloseAction(JXWindow::kDeactivateDirector);
		}

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
			JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, w, kJXDefaultMenuBarHeight);
	assert( menuBar != NULL );

	itsPlotMenu = menuBar->AppendTextMenu(kPlotMenuTitleStr);
	itsPlotMenu->SetMenuItems(kPlotMenuStr);
	itsPlotMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPlotMenu);

	itsAnalysisMenu = menuBar->AppendTextMenu(kAnalysisMenuTitleStr);
	itsAnalysisMenu->SetMenuItems(kAnalysisMenuStr);
	itsAnalysisMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsAnalysisMenu);

	itsFitParmsMenu = jnew JXTextMenu(itsAnalysisMenu, kFitParmsCmd, menuBar);
	assert( itsFitParmsMenu != NULL );
	itsFitParmsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitParmsMenu);
	itsAnalysisMenu->DisableItem(kFitParmsCmd);

	itsDiffMenu = jnew JXTextMenu(itsAnalysisMenu, kDiffPlotCmd, menuBar);
	assert( itsDiffMenu != NULL );
	itsDiffMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDiffMenu);
	itsAnalysisMenu->DisableItem(kDiffPlotCmd);

	itsSessionDir =
		jnew GloveHistoryDir(JXGetApplication());
	assert(itsSessionDir != NULL);
	JXGetDocumentManager()->DocumentMustStayOpen(itsSessionDir, kJTrue);
	ListenTo(itsSessionDir);

	itsPlot =
		jnew GlovePlotter(itsSessionDir, menuBar, window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 30, w, h - kJXDefaultMenuBarHeight);
	assert( itsPlot != NULL );

	ListenTo(itsPlot);

	JXDocumentMenu* windowListMenu =
		jnew JXDocumentMenu(kWindowListMenuTitleStr, menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowListMenu != NULL );
	menuBar->AppendMenu(windowListMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsVarList	 = jnew GVarList();
	itsVarList->AddVariable("x",0);
	itsXVarIndex = 1;
	itsFunctionDialog = NULL;
	itsFitModuleDialog = NULL;

	itsFits = jnew JPtrArray<JFitBase>(JPtrArrayT::kForgetAll);
	assert( itsFits != NULL );
	ListenTo(itsFits);
	itsFitParmsDir = jnew FitParmsDir(this, itsFits);
	assert( itsFitParmsDir != NULL );
	itsCurveStats = jnew JArray<CurveStats>;
	assert( itsCurveStats != NULL );
	itsCurrentCurveType = kGDataCurve;

	itsDiffDirs = jnew JPtrArray<PlotDir>(JPtrArrayT::kForgetAll);
	assert( itsDiffDirs != NULL );
	ListenTo(itsDiffDirs);

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	itsPrinter->SetOrientation(JPSPrinter::kLandscape);

	itsEPSPrinter	= jnew JX2DPlotEPSPrinter(GetDisplay());
	assert(itsEPSPrinter != NULL);
	itsPlot->SetEPSPrinter(itsEPSPrinter);

	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PlotDir::~PlotDir()
{
	jdelete itsFits;
	jdelete itsCurveStats;
//	for (JSize i = itsDiffDirs->GetElementCount(); i >= 1; i++)
//		{
//		itsDiffDirs->GetElement(i)->Close();
//		}
	jdelete itsDiffDirs;
	itsSessionDir->Close();
	jdelete itsVarList;
	jdelete itsPrinter;
	jdelete itsEPSPrinter;
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
PlotDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kTitleChanged))
		{
		JString title = itsFileName + ":  " + itsPlot->GetTitle();
		GetWindow()->SetTitle(title);
		JString sessiontitle = "Glove session  -  " + title;
		(itsSessionDir->GetWindow())->SetTitle(sessiontitle);
		}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kPlotChanged))
		{
		itsSupervisor->DataModified();
		}

	else if (sender == itsSessionDir && message.Is(GloveHistoryDir::kSessionChanged))
		{
		itsSupervisor->DataModified();
		}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kIsEmpty))
		{
		if (!itsPlotIsClosing)
			{
			GLClosePlotDirTask* task = jnew GLClosePlotDirTask(this);
			assert(task != NULL);
			task->Go();
			}
		}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveAdded))
		{
		HandleCurveAdded();
		}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveRemoved))
		{
		const J2DPlotWidget::CurveRemoved* info =
			dynamic_cast<const J2DPlotWidget::CurveRemoved*>(&message);
		assert( info != NULL );
		HandleCurveRemoved(info->GetIndex());
		}

	else if (sender == itsPlotMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePlotMenu(selection->GetIndex());
		}
	else if (sender == itsAnalysisMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleAnalysisMenu(selection->GetIndex());
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsFits &&
		( 	message.Is(JOrderedSetT::kElementsInserted) ||
			message.Is(JOrderedSetT::kElementsRemoved)))
		{
		UpdateFitParmsMenu();
		}

	else if (sender == itsDiffDirs &&
		( 	message.Is(JOrderedSetT::kElementsInserted) ||
			message.Is(JOrderedSetT::kElementsRemoved)))
		{
		UpdateDiffMenu();
		}


	else if (sender == itsFitParmsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsFitParmsDir->Activate();
		itsFitParmsDir->ShowFit(selection->GetIndex());
		}

	else if (sender == itsDiffMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		PlotDir* dir = itsDiffDirs->GetElement(selection->GetIndex());
		dir->Activate();
//		(itsDiffDirs->GetElement(selection->GetIndex()))->Activate();
		}


	else if (sender == itsFunctionDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			JString fnStr = itsFunctionDialog->GetFunctionString();
			JFunction* f;
			if (JParseFunction(fnStr, itsVarList, &f))
				{
				PlotFunction(f);
				}
			}
		itsFunctionDialog = NULL;
		}

	else if (sender == itsFitModuleDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			JIndex index = itsFitModuleDialog->GetFilterIndex();
			JPlotDataBase* data = &(itsPlot->GetCurve(itsCurveForFit));
			FitModule* fm;
			JString modName;
			(GLGetApplication())->GetFitModulePath(index, &modName);
			JBoolean success = FitModule::Create(&fm, this, data, modName);
			if (!success)
				{
				JGetUserNotification()->ReportError("Unknown module error.");
				}
			}
		itsFitModuleDialog = NULL;
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPageSetupFinished))
		{
		const JPrinter::PageSetupFinished* info =
			dynamic_cast<const JPrinter::PageSetupFinished*>(&message);
		assert(info != NULL);
		if (info->Changed())
			{
			itsSupervisor->DataModified();
			// for WYSISYG: adjust display to match new paper size
			}
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			itsPlot->Print(*itsPrinter);
			if (itsIsPrintAll)
				{
				itsFitParmsDir->SendAllToSession();
				itsSessionDir->Print();
				}
			}
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 NewFileName

 ******************************************************************************/

void
PlotDir::NewFileName
	(
	const JCharacter* filename
	)
{
	itsFileName = filename;
	JString title = itsFileName + ":  " + itsPlot->GetTitle();
	GetWindow()->SetTitle(title);
	JString sessiontitle = "Glove session  -  " + title;
	(itsSessionDir->GetWindow())->SetTitle(sessiontitle);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
PlotDir::WriteSetup
	(
	std::ostream& os
	)
{
	GetWindow()->WriteGeometry(os);
	itsPrinter->WriteXPSSetup(os);
	itsEPSPrinter->WriteX2DEPSSetup(os);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
PlotDir::ReadSetup
	(
	std::istream& 		is,
	const JFloat	gloveVersion
	)
{
	GetWindow()->ReadGeometry(is);
	itsPrinter->ReadXPSSetup(is);
	if (gloveVersion >= 3)
		{
		itsEPSPrinter->ReadX2DEPSSetup(is);
		}
}

/******************************************************************************
 WriteData

 ******************************************************************************/

void
PlotDir::WriteData
	(
	std::ostream& os,
	GRaggedFloatTableData* data
	)
{
	os << kCurrentSetupVersion << ' ';

	WriteCurves(os, data);
	itsPlot->PWXWriteSetup(os);
	os << ' ';
	itsPlot->PWWriteCurveSetup(os);
	os << ' ';

	const JSize diffCount = itsDiffDirs->GetElementCount();
	os << diffCount << ' ';
	for (JIndex i = 1; i <= diffCount; i++)
		{
		(itsDiffDirs->GetElement(i))->WriteSetup(os);
		(itsDiffDirs->GetElement(i))->WriteData(os, data);
		}
	itsSessionDir->WriteData(os);
}

/******************************************************************************
 WriteCurves

 ******************************************************************************/

void
PlotDir::WriteCurves
	(
	std::ostream& os,
	GRaggedFloatTableData* data
	)
{
	const JSize tempCount = itsPlot->GetCurveCount();
	JSize count = 0;
	JSize i;
	for (i = 1; i <= tempCount; i++)
		{
		JPlotDataBase& jpdb = itsPlot->GetCurve(i);
		CurveStats stat = itsCurveStats->GetElement(i);
		if ((stat.type == kGDataCurve) && (jpdb.GetType() == JPlotDataBase::kScatterPlot))
			{
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&jpdb);
			assert( pd != NULL );
			if (pd->IsValid())
				{
				count++;
				}
			}
		else if (stat.type == kGFitCurve)
			{
			JPlotDataBase& pdb = itsPlot->GetCurve(stat.provider);
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&pdb);
			assert( pd != NULL );
			if (pd->IsValid())
				{
				count++;
				}
			}
		else
			{
			count++;
			}
		}

	os << count << ' ';
	for (i = 1; i <= tempCount; i++)
		{
		JPlotDataBase& jpdb = itsPlot->GetCurve(i);
		CurveStats stat = itsCurveStats->GetElement(i);
		if (stat.type == kGDataCurve)
			{
			if (jpdb.GetType() == JPlotDataBase::kScatterPlot)
				{
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&jpdb);
				assert( pd != NULL );
				if (pd->IsValid())
					{
					os << (int)kGDataCurve << ' ';
					os << (int)jpdb.GetType() << ' ';
					JIndex index;
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetXData())), &index);
					os << index << ' ';
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetYData())), &index);
					os << index << ' ';
					os << pd->HasXErrors() << ' ';
					os << pd->HasYErrors() << ' ';
					if (pd->HasXErrors())
						{
						const JArray<JFloat>* testArray;
						pd->GetXPErrorData(&testArray);
						data->FindColumn(testArray, &index);
						os << index << ' ';
						}
					if (pd->HasYErrors())
						{
						const JArray<JFloat>* testArray;
						pd->GetYPErrorData(&testArray);
						data->FindColumn(testArray, &index);
						os << index << ' ';
						}
					}
				}
			else if (jpdb.GetType() == JPlotDataBase::kVectorPlot)
				{
				J2DVectorData* vd = dynamic_cast<J2DVectorData*>(&jpdb);
				assert( vd != NULL );
				os << (int)kGDataCurve << ' ';
				os << (int)jpdb.GetType() << ' ';
				JIndex index;
				const JArray<JFloat>* carray;
				carray	= &(vd->GetXData());
				JArray<JFloat>* array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetYData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetVXData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetVYData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				}
			}
		else if (stat.type == kGFitCurve)
			{
			if (stat.fitType == kGModFit)
				{
				os << (int)kGFitCurve << ' ';
				os << (int)stat.fitType << ' ';
				os << stat.provider << ' ';
				JPlotModuleFit* mf = dynamic_cast<JPlotModuleFit*>(&jpdb);
				assert( mf != NULL );
				mf->WriteData(os);
				}
			else
				{
				JPlotDataBase& pdb = itsPlot->GetCurve(stat.provider);
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&pdb);
				assert( pd != NULL );
				if (pd->IsValid())
					{
					os << (int)kGFitCurve << ' ';
					os << (int)stat.fitType << ' ';
					os << stat.provider << ' ';

					if (stat.fitType == kGProxyFit)
						{
						JPlotFitProxy* pf	= dynamic_cast<JPlotFitProxy*>(&jpdb);
						assert(pf != NULL);
						pf->WriteData(os);
						}
					}
				}
			}
		else if (stat.type == kGFunctionCurve)
			{
			J2DPlotJFunction* pd = dynamic_cast<J2DPlotJFunction*>(&jpdb);
			assert( pd != NULL );
			os << (int)kGFunctionCurve << ' ';
			os << pd->GetFunctionString() << ' ';
			}
		else
			{
			os << (int)kGDiffCurve << ' ';
			}
		}
}

/******************************************************************************
 ReadData

 ******************************************************************************/

void
PlotDir::ReadData
	(
	std::istream& is,
	GRaggedFloatTableData* data,
	const JFloat gloveVersion
	)
{
	JFileVersion version;
	is >> version;

	if (version == 0)
		{
		itsPlot->PWReadSetup(is);
		ReadCurves(is,data);
		}
	else if (version == 1)
		{
		ReadCurves(is,data);
		itsPlot->PWReadSetup(is);
		}
	else
		{
		ReadCurves(is,data);
		itsPlot->PWXReadSetup(is);
		}

	if (version <= kCurrentSetupVersion)
		{
		itsPlot->PWReadCurveSetup(is);

		JSize diffCount;
		is >> diffCount;
		assert(diffCount == itsDiffDirs->GetElementCount());
		for (JIndex i = 1; i <= diffCount; i++)
			{
			(itsDiffDirs->GetElement(i))->ReadSetup(is, gloveVersion);
			(itsDiffDirs->GetElement(i))->ReadData(is, data, gloveVersion);
			}

		itsSessionDir->ReadData(is);
		}
}

/******************************************************************************
 ReadCurves (private)

 ******************************************************************************/

void
PlotDir::ReadCurves
	(
	std::istream& is,
	GRaggedFloatTableData* data
	)
{
	JSize curveCount;
	is >> curveCount;

	for (JIndex j = 1; j <= curveCount; j++)
		{
		GCurveType type;
		int temp;
		is >> temp;
		type = (GCurveType)temp;
		if (type == kGDataCurve)
			{
			is >> temp;
			if (temp == JPlotDataBase::kScatterPlot)
				{
				JIndex xIndex, yIndex, xErrIndex, yErrIndex;
				JBoolean xerrors, yerrors;
				is >> xIndex >> yIndex;
				is >> xerrors >> yerrors;
				if (xerrors)
					{
					is >> xErrIndex;
					}
				if (yerrors)
					{
					is >> yErrIndex;
					}
				itsCurrentCurveType = kGDataCurve;
				J2DPlotData* curve;
				if (J2DPlotData::Create(&curve, data->GetColPointer(xIndex),
										data->GetColPointer(yIndex), kJTrue))
					{
					itsPlot->AddCurve(curve, kJTrue, "");

					if (xerrors)
						{
						curve->SetXErrors(data->GetColPointer(xErrIndex));
						}
					if (yerrors)
						{
						curve->SetYErrors(data->GetColPointer(yErrIndex));
						}
					}
				}
			else if (temp == JPlotDataBase::kVectorPlot)
				{
				JIndex xIndex, yIndex, vxIndex, vyIndex;
				is >> xIndex >> yIndex >> vxIndex >> vyIndex;
				itsCurrentCurveType = kGDataCurve;
				J2DVectorData* curve;
				if (J2DVectorData::Create(&curve, data->GetColPointer(xIndex),
										  data->GetColPointer(yIndex),
										  data->GetColPointer(vxIndex),
										  data->GetColPointer(vyIndex), kJTrue))
					{
					itsPlot->AddCurve(curve, kJTrue, "", kJFalse, kJFalse);
					}
				}
			}
		else if (type == kGFitCurve)
			{
			GCurveFitType ftype;
			int temp;
			is >> temp;
			ftype = (GCurveFitType)temp;
			JIndex provider;
			is >> provider;
			if (ftype == kGModFit)
				{
				JPlotDataBase* pdata = &(itsPlot->GetCurve(provider));
				JPlotModuleFit* mf = jnew JPlotModuleFit(itsPlot, pdata, is);
				assert(mf != NULL);
				if (!AddFitModule(mf, pdata))
					{
					jdelete mf;
					}
				}
			else if (ftype == kGProxyFit)
				{
				JPlotDataBase* pdata = &(itsPlot->GetCurve(provider));
				JPlotFitProxy* pf	= jnew JPlotFitProxy(itsPlot, pdata, is);
				assert(pf != NULL);

				itsFits->Append(pf);
				AddFit(pf, provider, kGProxyFit);
				}
			else
				{
				NewFit(provider, ftype);
				}
			}
		else if (type == kGFunctionCurve)
			{
			JString fnString;
			is >> fnString;
			J2DPlotJFunction* pf;
			JFloat min, max, inc;
			itsPlot->GetXScale(&min, &max, &inc);
			if (J2DPlotJFunction::Create(&pf, itsPlot, itsVarList, fnString, itsXVarIndex, min, max))
				{
				itsCurrentCurveType = kGFunctionCurve;
				itsPlot->AddCurve(pf, kJTrue, pf->GetFunctionString(), kJTrue, kJFalse);
				itsCurrentCurveType = kGDataCurve;
				}
			}
		}
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JBoolean
PlotDir::OKToClose()
{
	return kJTrue;
}

/******************************************************************************
 OKToRevert

 ******************************************************************************/

JBoolean
PlotDir::OKToRevert()
{
	return kJTrue;
}

/******************************************************************************
 CanRevert

 ******************************************************************************/

JBoolean
PlotDir::CanRevert()
{
	return kJTrue;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

JBoolean
PlotDir::NeedsSave()
	const
{
	return kJFalse;
}

/******************************************************************************
 HandlePlotMenu

 ******************************************************************************/

void
PlotDir::HandlePlotMenu
	(
	const JIndex index
	)
{
	if (index == kPageSetupCmd)
		{
		itsPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsPrinter->BeginUserPrintSetup();
		itsIsPrintAll = kJFalse;
		}
	else if (index == kPrintSessionCmd)
		{
		itsPrinter->BeginUserPrintSetup();
		itsIsPrintAll = kJTrue;
		}

	else if (index == kPrintPlotEPSCmd)
		{
		itsPlot->PrintPlotEPS();
		}
	else if (index == kPrintMarksEPSCmd)
		{
		itsPlot->PrintMarksEPS();
		}

	else if (index == kCloseCmd)
		{
		if (itsHideOnClose)
			{
			Deactivate();
			}
		else
			{
			Close();
			}
		}
}

/******************************************************************************
 HandleAnalysisMenu

 ******************************************************************************/

void
PlotDir::HandleAnalysisMenu
	(
	const JIndex index
	)
{
	if (index == kPlotFunctionCmd)
		{
		CreateFunction();
		}
	else if (index == kFitWindowCmd)
		{
		GLFitDirector* dir = jnew GLFitDirector(this, itsPlot, itsFileName);
		assert(dir != NULL);
		dir->Activate();
		}
}

/******************************************************************************
 CreateFunction

 ******************************************************************************/

void
PlotDir::CreateFunction()
{
	assert(itsFunctionDialog == NULL);
	itsFunctionDialog =
		jnew GXPlotFunctionDialog(this, itsVarList);
	assert( itsFunctionDialog != NULL);
	ListenTo(itsFunctionDialog);
	itsFunctionDialog->BeginDialog();
}

/******************************************************************************
 PlotFunction

 ******************************************************************************/

void
PlotDir::PlotFunction
	(
	JFunction* f
	)
{
	JFloat min, max, inc;
	itsPlot->GetXScale(&min, &max, &inc);
	J2DPlotJFunction* pf = jnew J2DPlotJFunction(itsPlot, itsVarList, f, kJTrue, itsXVarIndex, min, max);
	itsCurrentCurveType = kGFunctionCurve;
	itsPlot->AddCurve(pf, kJTrue, f->Print(), kJTrue, kJFalse);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 UpdateFitParmsMenu

 ******************************************************************************/

void
PlotDir::UpdateFitParmsMenu()
{
	itsFitParmsMenu->RemoveAllItems();
	const JSize count = itsFits->GetElementCount();
	JSize i;
	for (i = 1; i <= count; i++)
		{
		JString numS(i);
		JString str = "Fit " + numS;
		itsFitParmsMenu->AppendItem(str);
		}
	if (count == 0)
		{
		itsAnalysisMenu->DisableItem(kFitParmsCmd);
		}

}

/******************************************************************************
 UpdateDiffMenu

 ******************************************************************************/

void
PlotDir::UpdateDiffMenu()
{
	itsDiffMenu->RemoveAllItems();
	const JSize count = itsDiffDirs->GetElementCount();
	JSize i;
	for (i = 1; i <= count; i++)
		{
		JString numS(i);
		JString str = "Residual plot " + numS;
		itsDiffMenu->AppendItem(str);
		}
	if (count == 0)
		{
		itsAnalysisMenu->DisableItem(kDiffPlotCmd);
		}

}

/******************************************************************************
 SelectFitModule

 ******************************************************************************/

void
PlotDir::SelectFitModule()
{
	assert(itsFitModuleDialog == NULL);
	itsFitModuleDialog =
		jnew FitModuleDialog(this);
	assert( itsFitModuleDialog != NULL);
	ListenTo(itsFitModuleDialog);
	itsFitModuleDialog->BeginDialog();
}

/******************************************************************************
 HandleCurveAdded

 ******************************************************************************/

void
PlotDir::HandleCurveAdded()
{
	CurveStats stats;
	stats.type = itsCurrentCurveType;
	stats.provider = 0;
	stats.fitNumber = 0;
	itsCurveStats->AppendElement(stats);
}

/******************************************************************************
 HandleCurveRemoved

 ******************************************************************************/

void
PlotDir::HandleCurveRemoved
	(
	const JIndex index
	)
{
	CurveStats stats = itsCurveStats->GetElement(index);
	if (stats.type == kGFitCurve)
		{
		RemoveFit(index);
		}
	itsCurveStats->RemoveElement(index);
	const JSize ccount = itsCurveStats->GetElementCount();
	JSize count = ccount;
	JSize i;
	if (stats.type == kGDataCurve)
		{
		i = index;
		while (i <= count)
			{
			CurveStats statsi = itsCurveStats->GetElement(i);
			if (statsi.type == kGFitCurve)
				{
				if (statsi.provider == index)
					{
					itsPlot->RemoveCurve(i);
					count --;
					}
				else
					{
					statsi.provider--;
					itsCurveStats->SetElement(i, statsi);
					i++;
					}
				}
			else
				{
				i++;
				}
			}
		}
	else if (stats.type == kGFunctionCurve)
		{
		for (i = index + 1; i <= count; i++)
			{
			CurveStats statsi = itsCurveStats->GetElement(i);
			if (statsi.type == kGFitCurve)
				{
				statsi.provider--;
				itsCurveStats->SetElement(i, statsi);
				}
			}
		}
}

/******************************************************************************
 RemoveFit

 ******************************************************************************/

void
PlotDir::RemoveFit
	(
	const JIndex index
	)
{
	CurveStats stats = itsCurveStats->GetElement(index);
	itsFits->RemoveElement(stats.fitNumber);
	PlotDir* dir = itsDiffDirs->GetElement(stats.fitNumber);
	dir->Close();
	itsDiffDirs->RemoveElement(stats.fitNumber);
	const JSize count = itsCurveStats->GetElementCount();
	JSize i;
	for (i = index + 1; i <= count; i++)
		{
		stats = itsCurveStats->GetElement(i);
		if (stats.type == kGFitCurve)
			{
			stats.provider--;
			stats.fitNumber--;
			itsCurveStats->SetElement(i, stats);
			}
		}
}

/******************************************************************************
 NewFit

 ******************************************************************************/

void
PlotDir::NewFit
	(
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	JPlotDataBase* data = &(itsPlot->GetCurve(plotindex));
	JPlotFitFunction* df;
	if (type == kGLinearFit)
		{
		JPlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew JPlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew JPlotLinearFit(itsPlot, data, xmin, xmax);
			}
		assert(lf != NULL);
		itsFits->Append(lf);
		df = lf;
		}
	else if (type == kGExpFit)
		{
		JPlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew JPlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax, kJFalse, kJTrue);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew JPlotLinearFit(itsPlot, data, xmin, xmax, kJFalse, kJTrue);
			}
		assert(lf != NULL);
		itsFits->Append(lf);
		df = lf;
//		JPlotExpFit* ef = jnew JPlotExpFit(itsPlot, data);
//		assert(ef != NULL);
//		itsFits->Append(ef);
//		df = ef;
		}
	else if (type == kGQuadFit)
		{
		JPlotQuadFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew JPlotQuadFit(itsPlot, data, xmin, xmax, ymin, ymax);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew JPlotQuadFit(itsPlot, data, xmin, xmax);
			}
		assert(lf != NULL);
		itsFits->Append(lf);
		df = lf;
		}
	else
		{
		assert( kJFalse );
		}
	AddFit(df, plotindex, type);
}

/******************************************************************************
 AddDiffCurve

 ******************************************************************************/

void
PlotDir::AddDiffCurve
	(
	JPlotDataBase* ddata
	)
{
	itsCurrentCurveType = kGDiffCurve;
	itsPlot->AddCurve(ddata, kJFalse, "Residual data", kJFalse, kJTrue);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 AddFitModule

 ******************************************************************************/

JBoolean
PlotDir::AddFitModule
	(
	JPlotModuleFit* fit,
	JPlotDataBase* fitData
	)
{
	JIndex plotindex;
	JBoolean found = itsPlot->GetCurveIndex(fitData, &plotindex);
	if (!found)
		{
		return kJFalse;
		}
	itsFits->Append(fit);
	AddFit(fit, plotindex, kGModFit);
	return kJTrue;
}

/******************************************************************************
 AddFit

 ******************************************************************************/

void
PlotDir::AddFit
	(
	JPlotFitFunction* fit,
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	itsCurrentCurveType = kGFitCurve;
	JIndex i = itsPlot->AddCurve(fit, kJTrue, fit->GetFunctionString(), kJTrue, kJFalse);
	itsCurrentCurveType = kGDataCurve;
	CurveStats stats = itsCurveStats->GetElement(i);
	stats.type 		= kGFitCurve;
	stats.provider	= plotindex;
	stats.fitNumber = itsFits->GetElementCount();
	stats.fitType	= type;
	itsCurveStats->SetElement(i, stats);
	itsAnalysisMenu->EnableItem(kFitParmsCmd);
	itsAnalysisMenu->EnableItem(kDiffPlotCmd);

	PlotDir* dir = jnew PlotDir(this, itsSupervisor, itsFileName, kJTrue);
	assert( dir != NULL );
	JXGetDocumentManager()->DocumentMustStayOpen(dir, kJTrue);
	JPlotDataBase* ddata = fit->GetDiffData();
	dir->AddDiffCurve(ddata);
	J2DPlotWidget* plot = dir->GetPlot();
	JString numS(itsFits->GetElementCount(), 0);
	JString str = "Residual Plot " + numS;
	plot->SetTitle(str);
	plot->SetXLabel(itsPlot->GetXLabel());
	plot->SetYLabel(itsPlot->GetYLabel());
	plot->ProtectCurve(1, kJTrue);
	plot->ShowFrame(kJFalse);
	itsDiffDirs->Append(dir);
}

/******************************************************************************
 AddFitProxy (public)

 ******************************************************************************/

void
PlotDir::AddFitProxy
	(
	JPlotFitProxy*		fit,
	const JIndex		index,
	const JCharacter*	name
	)
{
	itsFits->Append(fit);
	AddFit(fit, index, kGProxyFit);
	itsPlot->SetCurveName(itsPlot->GetCurveCount(), name);
}

/******************************************************************************
 CurveIsFit (public)

 ******************************************************************************/

JBoolean
PlotDir::CurveIsFit
	(
	const JIndex index
	)
	const
{
	assert(itsCurveStats->IndexValid(index));
	CurveStats stat = itsCurveStats->GetElement(index);
	if (stat.type == kGFitCurve)
		{
		return kJTrue;
		}
	return kJFalse;
}


/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
PlotDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(GLGetApplication())->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kGLPlotHelpName);
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kGLChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kGLCreditsName);
		}
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
PlotDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{

}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
PlotDir::DiscardChanges()
{

}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
PlotDir::GetMenuIcon()
	const
{
	return JXPM(jx_plain_file_small);
}

/******************************************************************************
 Close (public)

 ******************************************************************************/

JBoolean
PlotDir::Close()
{
	itsPlotIsClosing = kJTrue;
	return JXDocument::Close();
}
