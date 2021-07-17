/******************************************************************************
 GLPlotDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLPlotDir.h"
#include "GLVarList.h"
#include "GLPlotFunctionDialog.h"
#include "GLPlotLinearFit.h"
#include "GLPlotQuadFit.h"
#include "GLPlotFitFunction.h"
#include "GLFitBase.h"
#include "GLFitParmsDir.h"
#include "JPlotDataBase.h"
#include "J2DPlotData.h"
#include "J2DVectorData.h"
#include "GLRaggedFloatTableData.h"
#include "GLHistoryDir.h"
#include "GLFitModule.h"
#include "GLFitModuleDialog.h"
#include "GLPlotApp.h"
#include "GLPlotModuleFit.h"
#include "J2DPlotJFunction.h"
#include "GLPlotter.h"
#include "GLGlobals.h"
#include "GLFitDirector.h"
#include "GLPlotFitProxy.h"

#include <JX2DPlotEPSPrinter.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXDocumentMenu.h>
#include <JXFileDocument.h>
#include <JXApplication.h>
#include <JXDialogDirector.h>
#include <JXPSPrinter.h>
#include <JXCloseDirectorTask.h>
#include <JXHelpManager.h>

#include <JExprParser.h>
#include <JUserNotification.h>

#include <jx_plain_file_small.xpm>

#include <jFStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 2;

// Plot menu

static const JUtf8Byte* kPlotMenuStr =
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

static const JUtf8Byte* kAnalysisMenuStr =
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

static const JUtf8Byte* kHelpMenuStr =
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

GLPlotDir::GLPlotDir
	(
	JXDirector*       supervisor,
	JXFileDocument*   notifySupervisor,
	const JString&    filename,
	const bool    hideOnClose
	)
	:
	JXDocument(supervisor),
	itsFileName(filename)
{
	itsSupervisor       = notifySupervisor;
	itsHideOnClose      = hideOnClose;
	itsPlotIsClosing	= false;

	itsPrinter          = nullptr;

	JCoordinate w 		= 500;
	JCoordinate h       = 400;
	JCoordinate minW	= 400;
	JCoordinate minH    = 300;

	JXWindow* window = jnew JXWindow(this, w,h, filename);
	assert( window != nullptr );

	window->SetMinSize(minW,minH);
	if (hideOnClose)
		{
		window->SetCloseAction(JXWindow::kDeactivateDirector);
		}

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
			JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, w, kJXDefaultMenuBarHeight);
	assert( menuBar != nullptr );

	itsPlotMenu = menuBar->AppendTextMenu(JGetString("PlotMenuTitle::GLPlotDir"));
	itsPlotMenu->SetMenuItems(kPlotMenuStr);
	itsPlotMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPlotMenu);

	itsAnalysisMenu = menuBar->AppendTextMenu(JGetString("AnalysisMenuTitle::GLPlotDir"));
	itsAnalysisMenu->SetMenuItems(kAnalysisMenuStr);
	itsAnalysisMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsAnalysisMenu);

	itsFitParmsMenu = jnew JXTextMenu(itsAnalysisMenu, kFitParmsCmd, menuBar);
	assert( itsFitParmsMenu != nullptr );
	itsFitParmsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitParmsMenu);
	itsAnalysisMenu->DisableItem(kFitParmsCmd);

	itsDiffMenu = jnew JXTextMenu(itsAnalysisMenu, kDiffPlotCmd, menuBar);
	assert( itsDiffMenu != nullptr );
	itsDiffMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDiffMenu);
	itsAnalysisMenu->DisableItem(kDiffPlotCmd);

	itsSessionDir = jnew GLHistoryDir(JXGetApplication());
	assert(itsSessionDir != nullptr);
	JXGetDocumentManager()->DocumentMustStayOpen(itsSessionDir, true);
	ListenTo(itsSessionDir);

	itsPlot =
		jnew GLPlotter(itsSessionDir, menuBar, window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 30, w, h - kJXDefaultMenuBarHeight);
	assert( itsPlot != nullptr );

	ListenTo(itsPlot);

	JXDocumentMenu* windowListMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowListMenu != nullptr );
	menuBar->AppendMenu(windowListMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsVarList	 = jnew GLVarList();
	itsVarList->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);
	itsXVarIndex = 1;
	itsFunctionDialog = nullptr;
	itsFitModuleDialog = nullptr;

	itsFits = jnew JPtrArray<GLFitBase>(JPtrArrayT::kForgetAll);
	assert( itsFits != nullptr );
	ListenTo(itsFits);
	itsFitParmsDir = jnew GLFitParmsDir(this, itsFits);
	assert( itsFitParmsDir != nullptr );
	itsCurveStats = jnew JArray<GloveCurveStats>;
	assert( itsCurveStats != nullptr );
	itsCurrentCurveType = kGDataCurve;

	itsDiffDirs = jnew JPtrArray<GLPlotDir>(JPtrArrayT::kForgetAll);
	assert( itsDiffDirs != nullptr );
	ListenTo(itsDiffDirs);

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	itsPrinter->SetOrientation(JPSPrinter::kLandscape);

	itsEPSPrinter	= jnew JX2DPlotEPSPrinter(GetDisplay());
	assert(itsEPSPrinter != nullptr);
	itsPlot->SetEPSPrinter(itsEPSPrinter);

	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLPlotDir::~GLPlotDir()
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
GLPlotDir::Receive
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

	else if (sender == itsSessionDir && message.Is(GLHistoryDir::kSessionChanged))
		{
		itsSupervisor->DataModified();
		}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kIsEmpty))
		{
		if (!itsPlotIsClosing)
			{
			JXCloseDirectorTask::Close(this);
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
		assert( info != nullptr );
		HandleCurveRemoved(info->GetIndex());
		}

	else if (sender == itsPlotMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePlotMenu(selection->GetIndex());
		}
	else if (sender == itsAnalysisMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleAnalysisMenu(selection->GetIndex());
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsFits &&
		( 	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
		{
		UpdateFitParmsMenu();
		}

	else if (sender == itsDiffDirs &&
		( 	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
		{
		UpdateDiffMenu();
		}


	else if (sender == itsFitParmsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsFitParmsDir->Activate();
		itsFitParmsDir->ShowFit(selection->GetIndex());
		}

	else if (sender == itsDiffMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		GLPlotDir* dir = itsDiffDirs->GetElement(selection->GetIndex());
		dir->Activate();
//		(itsDiffDirs->GetElement(selection->GetIndex()))->Activate();
		}


	else if (sender == itsFunctionDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			JString fnStr = itsFunctionDialog->GetFunctionString();

			JExprParser p(itsVarList);

			JFunction* f;
			if (p.Parse(fnStr, &f))
				{
				PlotFunction(f);
				}
			}
		itsFunctionDialog = nullptr;
		}

	else if (sender == itsFitModuleDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			JIndex index = itsFitModuleDialog->GetFilterIndex();
			JPlotDataBase* data = &(itsPlot->GetCurve(itsCurveForFit));
			GLFitModule* fm;
			JString modName;
			(GLGetApplication())->GetFitModulePath(index, &modName);
			bool success = GLFitModule::Create(&fm, this, data, modName);
			if (!success)
				{
				JGetUserNotification()->ReportError(JGetString("UnknownError::GLFitModule"));
				}
			}
		itsFitModuleDialog = nullptr;
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPageSetupFinished))
		{
		const JPrinter::PageSetupFinished* info =
			dynamic_cast<const JPrinter::PageSetupFinished*>(&message);
		assert(info != nullptr);
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
		assert(info != nullptr);
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
GLPlotDir::NewFileName
	(
	const JString& filename
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
GLPlotDir::WriteSetup
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
GLPlotDir::ReadSetup
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
GLPlotDir::WriteData
	(
	std::ostream& os,
	GLRaggedFloatTableData* data
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
GLPlotDir::WriteCurves
	(
	std::ostream& os,
	GLRaggedFloatTableData* data
	)
{
	const JSize tempCount = itsPlot->GetCurveCount();
	JSize count = 0;
	JSize i;
	for (i = 1; i <= tempCount; i++)
		{
		JPlotDataBase& jpdb = itsPlot->GetCurve(i);
		GloveCurveStats stat = itsCurveStats->GetElement(i);
		if ((stat.type == kGDataCurve) && (jpdb.GetType() == JPlotDataBase::kScatterPlot))
			{
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&jpdb);
			assert( pd != nullptr );
			if (pd->IsValid())
				{
				count++;
				}
			}
		else if (stat.type == kGFitCurve)
			{
			JPlotDataBase& pdb = itsPlot->GetCurve(stat.provider);
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&pdb);
			assert( pd != nullptr );
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
		GloveCurveStats stat = itsCurveStats->GetElement(i);
		if (stat.type == kGDataCurve)
			{
			if (jpdb.GetType() == JPlotDataBase::kScatterPlot)
				{
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&jpdb);
				assert( pd != nullptr );
				if (pd->IsValid())
					{
					os << (int)kGDataCurve << ' ';
					os << (int)jpdb.GetType() << ' ';
					JIndex index;
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetXData())), &index);
					os << index << ' ';
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetYData())), &index);
					os << index << ' ';
					os << JBoolToString(pd->HasXErrors())
					   << JBoolToString(pd->HasYErrors()) << ' ';
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
				assert( vd != nullptr );
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
				GLPlotModuleFit* mf = dynamic_cast<GLPlotModuleFit*>(&jpdb);
				assert( mf != nullptr );
				mf->WriteData(os);
				}
			else
				{
				JPlotDataBase& pdb = itsPlot->GetCurve(stat.provider);
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(&pdb);
				assert( pd != nullptr );
				if (pd->IsValid())
					{
					os << (int)kGFitCurve << ' ';
					os << (int)stat.fitType << ' ';
					os << stat.provider << ' ';

					if (stat.fitType == kGProxyFit)
						{
						GLPlotFitProxy* pf	= dynamic_cast<GLPlotFitProxy*>(&jpdb);
						assert(pf != nullptr);
						pf->WriteData(os);
						}
					}
				}
			}
		else if (stat.type == kGFunctionCurve)
			{
			J2DPlotJFunction* pd = dynamic_cast<J2DPlotJFunction*>(&jpdb);
			assert( pd != nullptr );
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
GLPlotDir::ReadData
	(
	std::istream& is,
	GLRaggedFloatTableData* data,
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
GLPlotDir::ReadCurves
	(
	std::istream& is,
	GLRaggedFloatTableData* data
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
				bool xerrors, yerrors;
				is >> xIndex >> yIndex;
				is >> JBoolFromString(xerrors) >> JBoolFromString(yerrors);
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
										data->GetColPointer(yIndex), true))
					{
					itsPlot->AddCurve(curve, true, JString::empty);

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
										  data->GetColPointer(vyIndex), true))
					{
					itsPlot->AddCurve(curve, true, JString::empty, false, false);
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
				GLPlotModuleFit* mf = jnew GLPlotModuleFit(itsPlot, pdata, is);
				assert(mf != nullptr);
				if (!AddFitModule(mf, pdata))
					{
					jdelete mf;
					}
				}
			else if (ftype == kGProxyFit)
				{
				JPlotDataBase* pdata = &(itsPlot->GetCurve(provider));
				GLPlotFitProxy* pf	= jnew GLPlotFitProxy(itsPlot, pdata, is);
				assert(pf != nullptr);

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
			if (J2DPlotJFunction::Create(&pf, itsPlot, itsVarList, GetDisplay()->GetFontManager(), fnString, itsXVarIndex, min, max))
				{
				itsCurrentCurveType = kGFunctionCurve;
				itsPlot->AddCurve(pf, true, pf->GetFunctionString(), true, false);
				itsCurrentCurveType = kGDataCurve;
				}
			}
		}
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

bool
GLPlotDir::OKToClose()
{
	return true;
}

/******************************************************************************
 OKToRevert

 ******************************************************************************/

bool
GLPlotDir::OKToRevert()
{
	return true;
}

/******************************************************************************
 CanRevert

 ******************************************************************************/

bool
GLPlotDir::CanRevert()
{
	return true;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

bool
GLPlotDir::NeedsSave()
	const
{
	return false;
}

/******************************************************************************
 HandlePlotMenu

 ******************************************************************************/

void
GLPlotDir::HandlePlotMenu
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
		itsIsPrintAll = false;
		}
	else if (index == kPrintSessionCmd)
		{
		itsPrinter->BeginUserPrintSetup();
		itsIsPrintAll = true;
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
GLPlotDir::HandleAnalysisMenu
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
		assert(dir != nullptr);
		dir->Activate();
		}
}

/******************************************************************************
 CreateFunction

 ******************************************************************************/

void
GLPlotDir::CreateFunction()
{
	assert(itsFunctionDialog == nullptr);
	itsFunctionDialog =
		jnew GLPlotFunctionDialog(this, itsVarList);
	assert( itsFunctionDialog != nullptr);
	ListenTo(itsFunctionDialog);
	itsFunctionDialog->BeginDialog();
}

/******************************************************************************
 PlotFunction

 ******************************************************************************/

void
GLPlotDir::PlotFunction
	(
	JFunction* f
	)
{
	JFloat min, max, inc;
	itsPlot->GetXScale(&min, &max, &inc);
	J2DPlotJFunction* pf = jnew J2DPlotJFunction(itsPlot, itsVarList, f, true, itsXVarIndex, min, max);
	itsCurrentCurveType = kGFunctionCurve;
	itsPlot->AddCurve(pf, true, f->Print(), true, false);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 UpdateFitParmsMenu

 ******************************************************************************/

void
GLPlotDir::UpdateFitParmsMenu()
{
	itsFitParmsMenu->RemoveAllItems();
	GLBuildColumnMenus("FitMenuItem::GLGlobal", itsFits->GetElementCount(),
					   itsFitParmsMenu, nullptr);

	if (itsFits->IsEmpty())
		{
		itsAnalysisMenu->DisableItem(kFitParmsCmd);
		}
}

/******************************************************************************
 UpdateDiffMenu

 ******************************************************************************/

void
GLPlotDir::UpdateDiffMenu()
{
	itsDiffMenu->RemoveAllItems();
	GLBuildColumnMenus("DiffMenuItem::GLPlotDir", itsDiffDirs->GetElementCount(),
					   itsDiffMenu, nullptr);

	if (itsDiffDirs->IsEmpty())
		{
		itsAnalysisMenu->DisableItem(kDiffPlotCmd);
		}
}

/******************************************************************************
 SelectFitModule

 ******************************************************************************/

void
GLPlotDir::SelectFitModule()
{
	assert(itsFitModuleDialog == nullptr);
	itsFitModuleDialog =
		jnew GLFitModuleDialog(this);
	assert( itsFitModuleDialog != nullptr);
	ListenTo(itsFitModuleDialog);
	itsFitModuleDialog->BeginDialog();
}

/******************************************************************************
 HandleCurveAdded

 ******************************************************************************/

void
GLPlotDir::HandleCurveAdded()
{
	GloveCurveStats stats;
	stats.type = itsCurrentCurveType;
	stats.provider = 0;
	stats.fitNumber = 0;
	itsCurveStats->AppendElement(stats);
}

/******************************************************************************
 HandleCurveRemoved

 ******************************************************************************/

void
GLPlotDir::HandleCurveRemoved
	(
	const JIndex index
	)
{
	GloveCurveStats stats = itsCurveStats->GetElement(index);
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
			GloveCurveStats statsi = itsCurveStats->GetElement(i);
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
			GloveCurveStats statsi = itsCurveStats->GetElement(i);
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
GLPlotDir::RemoveFit
	(
	const JIndex index
	)
{
	GloveCurveStats stats = itsCurveStats->GetElement(index);
	itsFits->RemoveElement(stats.fitNumber);
	GLPlotDir* dir = itsDiffDirs->GetElement(stats.fitNumber);
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
GLPlotDir::NewFit
	(
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	JPlotDataBase* data = &(itsPlot->GetCurve(plotindex));
	GLPlotFitFunction* df = nullptr;
	if (type == kGLinearFit)
		{
		GLPlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew GLPlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew GLPlotLinearFit(itsPlot, data, xmin, xmax);
			}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
		}
	else if (type == kGExpFit)
		{
		GLPlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew GLPlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax, false, true);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew GLPlotLinearFit(itsPlot, data, xmin, xmax, false, true);
			}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
//		GLPlotExpFit* ef = jnew GLPlotExpFit(itsPlot, data);
//		assert(ef != nullptr);
//		itsFits->Append(ef);
//		df = ef;
		}
	else if (type == kGQuadFit)
		{
		GLPlotQuadFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
			{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew GLPlotQuadFit(itsPlot, data, xmin, xmax, ymin, ymax);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			lf = jnew GLPlotQuadFit(itsPlot, data, xmin, xmax);
			}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
		}
	else
		{
		assert( false );
		}
	AddFit(df, plotindex, type);
}

/******************************************************************************
 AddDiffCurve

 ******************************************************************************/

void
GLPlotDir::AddDiffCurve
	(
	JPlotDataBase* ddata
	)
{
	itsCurrentCurveType = kGDiffCurve;
	itsPlot->AddCurve(ddata, false, JGetString("DiffCurveName::GLPlotDir"), false, true);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 AddFitModule

 ******************************************************************************/

bool
GLPlotDir::AddFitModule
	(
	GLPlotModuleFit* fit,
	JPlotDataBase* fitData
	)
{
	JIndex plotindex;
	bool found = itsPlot->GetCurveIndex(fitData, &plotindex);
	if (!found)
		{
		return false;
		}
	itsFits->Append(fit);
	AddFit(fit, plotindex, kGModFit);
	return true;
}

/******************************************************************************
 AddFit

 ******************************************************************************/

void
GLPlotDir::AddFit
	(
	GLPlotFitFunction* fit,
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	itsCurrentCurveType = kGFitCurve;
	JIndex i = itsPlot->AddCurve(fit, true, fit->GetFunctionString(), true, false);
	itsCurrentCurveType = kGDataCurve;
	GloveCurveStats stats = itsCurveStats->GetElement(i);
	stats.type 		= kGFitCurve;
	stats.provider	= plotindex;
	stats.fitNumber = itsFits->GetElementCount();
	stats.fitType	= type;
	itsCurveStats->SetElement(i, stats);
	itsAnalysisMenu->EnableItem(kFitParmsCmd);
	itsAnalysisMenu->EnableItem(kDiffPlotCmd);

	GLPlotDir* dir = jnew GLPlotDir(this, itsSupervisor, itsFileName, true);
	assert( dir != nullptr );
	JXGetDocumentManager()->DocumentMustStayOpen(dir, true);
	JPlotDataBase* ddata = fit->GetDiffData();
	dir->AddDiffCurve(ddata);
	J2DPlotWidget* plot = dir->GetPlot();
	JString numS(itsFits->GetElementCount(), 0);
	JString str = "Residual Plot " + numS;
	plot->SetTitle(str);
	plot->SetXLabel(itsPlot->GetXLabel());
	plot->SetYLabel(itsPlot->GetYLabel());
	plot->ProtectCurve(1, true);
	plot->ShowFrame(false);
	itsDiffDirs->Append(dir);
}

/******************************************************************************
 AddFitProxy (public)

 ******************************************************************************/

void
GLPlotDir::AddFitProxy
	(
	GLPlotFitProxy*		fit,
	const JIndex		index,
	const JString&		name
	)
{
	itsFits->Append(fit);
	AddFit(fit, index, kGProxyFit);
	itsPlot->SetCurveName(itsPlot->GetCurveCount(), name);
}

/******************************************************************************
 CurveIsFit (public)

 ******************************************************************************/

bool
GLPlotDir::CurveIsFit
	(
	const JIndex index
	)
	const
{
	assert(itsCurveStats->IndexValid(index));
	GloveCurveStats stat = itsCurveStats->GetElement(index);
	if (stat.type == kGFitCurve)
		{
		return true;
		}
	return false;
}


/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
GLPlotDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		GLGetApplication()->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		JXGetHelpManager()->ShowTOC();
		}
	else if (index == kThisWindowCmd)
		{
		JXGetHelpManager()->ShowSection("GLPlotHelp");
		}
	else if (index == kChangesCmd)
		{
		JXGetHelpManager()->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		JXGetHelpManager()->ShowCredits();
		}
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
GLPlotDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
GLPlotDir::DiscardChanges()
{
}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
GLPlotDir::GetMenuIcon()
	const
{
	return JXPM(jx_plain_file_small);
}

/******************************************************************************
 Close (public)

 ******************************************************************************/

bool
GLPlotDir::Close()
{
	itsPlotIsClosing = true;
	return JXDocument::Close();
}
