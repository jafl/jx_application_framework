/******************************************************************************
 GLFitDirector.cpp

	<Description>

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GLFitDirector.h>
#include "GLCurveNameList.h"
#include "GLFitDescriptionList.h"
#include "GLFitManager.h"
#include "GLFitParameterTable.h"
#include "GLParmColHeaderWidget.h"
#include "GLChiSqLabel.h"
#include "JPlotFitFunction.h"
#include "JPlotFitLinearEq.h"
#include "JPlotFitExp.h"
#include "JPlotFitPowerLaw.h"
#include "JPlotFitProxy.h"
#include "GLPolyFitDescription.h"
#include "GLBuiltinFitDescription.h"
#include "GLNonLinearFitDescription.h"
#include "GLModuleFitDescription.h"
#include "GLNonLinearFitDialog.h"
#include "GLPolyFitDialog.h"
#include "GVarList.h"
#include "GloveHistoryDir.h"
#include "PlotDir.h"

#include "JPlotFitLinear.h"
#include "JPlotFitNonLinear.h"
#include "JPlotFitModule.h"

#include "GLPrefsMgr.h"
#include "GLGlobals.h"
#include "GLPlotApp.h"

#include "jx_help_specific.xpm"
#include "jx_help_toc.xpm"

#include <J2DPlotData.h>
#include <J2DPlotJFunction.h>
#include <JX2DPlotWidget.h>

#include <JXToolBar.h>

#include <JXApplication.h>
#include <JXColHeaderWidget.h>
#include <JXColorManager.h>
#include <JXDownRect.h>
#include <JXExprWidget.h>
#include <JXHelpManager.h>
#include <JXHorizPartition.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXPSPrinter.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXUpRect.h>
#include <JXVertPartition.h>
#include <JXWindow.h>

#include <JChooseSaveFile.h>
#include <JFunction.h>
#include <JString.h>
#include <JVector.h>

#include <jDirUtil.h>
#include <jParseFunction.h>
#include <jAssert.h>

const JCoordinate kCloseButtonWidth 	= 50;
const JCoordinate kCurrentPrefsVersion	= 1;

static const JCharacter* kFitMenuTitleStr = "Fit";
static const JCharacter* kFitMenuStr =
	"   New non-linear fit   %k Meta-N %i NewNonLinear::FitDirector"
	"  |New polynomial fit   %k Meta-Shift-N %i NewPoly::FitDirector"
	"  |Remove fit   %k Backspace. %i RemoveFit::FitDirector"
	"%l|Fit      %k Meta-F %i Fit::FitDirector"
	"  |Re-fit   %k Meta-R %i ReFit::FitDirector"
	"%l|Show start values %b %k Meta-Shift-S %i ShowStartValues::FitDirector"
	"  |Test start values %k Meta-T %i TestFit::FitDirector"
	"%l|Plot     %k Meta-Shift-P %i Plot::FitDirector"
	"  |Show fit history %i ShowHistory::FitDirector"
	"  |Print    %k Meta-P %i Print::FitDirector"
	"%l|Close    %k Meta-W %i Close::FitDirector";

enum
{
	kNonLinearCmd = 1,
	kPolyCmd,
	kRemoveFitCmd,
	kFitCmd,
	kRefitCmd,
	kShowStartCmd,
	kTestFitCmd,
	kPlotCmd,
	kShowHistoryCmd,
	kPrintCmd,
	kCloseCmd
};

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"   Edit preferences %i Preferences::FitDirector"
	"  |Edit tool bar... %i EditToolBar::FitDirector"
	"%l|Save window size %i SaveWindowSize::FitDirector";

enum
{
	kPrefsCmd = 1,
	kEditToolBarCmd,
	kSaveWindowSizeCmd
};

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"   About"
	"%l|Table of Contents %i TOCHelp::FitDirector"
	"  |This window       %i ThisWindowHelp::FitDirector"
	"%l|Changes"
	"  |Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

const JCharacter* kFitPlotTitle		= "Fit";
const JCharacter* kDiffPlotTitle	= "Residuals";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitDirector::GLFitDirector
	(
	PlotDir* 			supervisor,
	J2DPlotWidget*		plot,
	const JCharacter*	file
	)
	:
	JXWindowDirector(supervisor),
	itsPlot(plot),
	itsCurrentFit(NULL),
	itsTestFunction(NULL),
	itsNLFitDialog(NULL),
	itsPolyFitDialog(NULL),
	itsDir(supervisor),
	itsPrinter(NULL)
{
	BuildWindow();

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFitMenu, kFitCmd);
		itsToolBar->AppendButton(itsFitMenu, kTestFitCmd);
		itsToolBar->AppendButton(itsFitMenu, kRefitCmd);
		itsToolBar->AppendButton(itsFitMenu, kPlotCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFitMenu, kCloseCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}

	itsHistory	= jnew GloveHistoryDir(this);
	assert(itsHistory != NULL);

	JString name;
	JString path;
	JSplitPathAndName(file, &path, &name);
	JString title	= "Fit: " + name;
	GetWindow()->SetTitle(title);

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	itsPrinter->SetOrientation(JPSPrinter::kPortrait);

	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitDirector::~GLFitDirector()
{
}

/******************************************************************************
 BuildWindow


 ******************************************************************************/

void
GLFitDirector::BuildWindow()
{
	JCoordinate w = 600;
	JCoordinate h = 420;
	JXWindow* window = jnew JXWindow(this, w,h, "Fit");
	assert( window != NULL );
	window->SetCloseAction(JXWindow::kDeactivateDirector);

	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, w,kJXDefaultMenuBarHeight);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(GetPrefsMgr(), kFitToolBarID,
			menuBar,
			window, JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight, w,h - kJXDefaultMenuBarHeight);
	assert( itsToolBar != NULL );

	window->SetMinSize(w, h);

	JSize newHeight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	const JCoordinate kPartitionHandleWidth	= 5;
	const JCoordinate kFitListWidth			= 155;

	JArray<JCoordinate> widths(2);
	widths.AppendElement(kFitListWidth);
	widths.AppendElement(w - kFitListWidth - kPartitionHandleWidth);

	JIndex elasticIndex = 2;

	JArray<JCoordinate> minWidths(2);
	minWidths.AppendElement(100);
	minWidths.AppendElement(300);

	itsMainPartition =
		jnew JXHorizPartition(widths, elasticIndex, minWidths,
							 itsToolBar->GetWidgetEnclosure(),
							 JXWidget::kHElastic,JXWidget::kVElastic,
							 0, 0, w, newHeight);
	assert( itsMainPartition != NULL );

	// This is the first column the contains the curve and fit lists.

	JXContainer* container = itsMainPartition->GetCompartment(1);

	const JCoordinate kCurveListHeight	= 100;
	const JCoordinate kColHeaderHeight	= 20;
	const JCoordinate kExprHeight		= 50;
	const JCoordinate kFitListHeight	= newHeight - kCurveListHeight - 2 * kPartitionHandleWidth - kExprHeight;

	JArray<JCoordinate> heights(3);
	heights.AppendElement(kCurveListHeight);
	heights.AppendElement(kFitListHeight);
	heights.AppendElement(kExprHeight);

	elasticIndex = 2;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendElement(50);
	minHeights.AppendElement(100);
	minHeights.AppendElement(40);

	itsListPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, container,
			JXWidget::kHElastic, JXWidget::kVElastic, 0, 0, kFitListWidth, newHeight);
	assert( itsListPartition != NULL );

	container = itsListPartition->GetCompartment(1);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(container,
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0, kColHeaderHeight,
						   kFitListWidth, kCurveListHeight - kColHeaderHeight);
	assert( scrollbarSet != NULL );

	// This will be the curve list

	itsCurveList	=
		jnew GLCurveNameList(itsDir, itsPlot,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,  kFitListWidth, kCurveListHeight - kColHeaderHeight);
	assert(itsCurveList != NULL);
	ListenTo(itsCurveList);

	JXColHeaderWidget* header =
		jnew JXColHeaderWidget(itsCurveList, scrollbarSet,
			container,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0,
			kFitListWidth,
			kColHeaderHeight);
	assert(header != NULL);

	header->SetColTitle(1, "Curves");

	container = itsListPartition->GetCompartment(2);

	scrollbarSet =
		jnew JXScrollbarSet(container,
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0, kColHeaderHeight,
						   kFitListWidth,
						   kFitListHeight - kColHeaderHeight);
	assert( scrollbarSet != NULL );

	// This will be the fit list

	itsFitList	=
		jnew GLFitDescriptionList(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			kFitListWidth,
			kFitListHeight - kColHeaderHeight);
	assert(itsFitList != NULL);
	ListenTo(itsFitList);

	header =
		jnew JXColHeaderWidget(itsFitList, scrollbarSet,
			container,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0,
			kFitListWidth,
			kColHeaderHeight);
	assert(header != NULL);

	header->SetColTitle(1, "Fits");

	// this is the expression widget that displays the current JFunction

	container = itsListPartition->GetCompartment(3);

	scrollbarSet =
		jnew JXScrollbarSet(container,
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0, 0,
						   kFitListWidth,
						   kExprHeight);
	assert( scrollbarSet != NULL );

	itsExprVarList	= jnew GVarList();
	assert(itsExprVarList != NULL);

	itsExprWidget	=
		jnew JXExprWidget(itsExprVarList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			kFitListWidth,
			kExprHeight);
	assert(itsExprWidget != NULL);
	itsExprWidget->Hide();

	// This is the second column that will contain the parameter table
	// and the plots

	container = itsMainPartition->GetCompartment(2);

	const JCoordinate kParmsTableHeight		= 50;
	const JCoordinate kChiSqHeight			= 20;
	const JCoordinate kTotalParmsHeight		= kParmsTableHeight + kColHeaderHeight + kChiSqHeight;
	const JCoordinate kFirstPlotHeight		= 120;
	const JCoordinate kMinPlotHeight		= 100;

	heights.RemoveAll();
	heights.AppendElement(kTotalParmsHeight);
	heights.AppendElement(kFirstPlotHeight);
	heights.AppendElement(newHeight - kFirstPlotHeight - kTotalParmsHeight - 2 * kPartitionHandleWidth);

	elasticIndex = 2;

	minHeights.RemoveAll();
	minHeights.AppendElement(kTotalParmsHeight - 20);
	minHeights.AppendElement(kMinPlotHeight);
	minHeights.AppendElement(kMinPlotHeight);

	itsPlotPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, container,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, w - kFitListWidth - kPartitionHandleWidth, newHeight);
	assert( itsPlotPartition != NULL );

	container = itsPlotPartition->GetCompartment(1);

	scrollbarSet =
		jnew JXScrollbarSet(container,
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0, kColHeaderHeight,
						   w - kFitListWidth - kPartitionHandleWidth,
						   kParmsTableHeight);
	assert( scrollbarSet != NULL );

	// this will be the parameter table
	itsParameterTable =
		jnew GLFitParameterTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			w - kFitListWidth - kPartitionHandleWidth,
			kParmsTableHeight);
	assert(itsParameterTable != NULL);
	ListenTo(itsParameterTable);

	itsParameterColHeader =
		jnew GLParmColHeaderWidget(itsParameterTable, scrollbarSet,
			container,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0,
			w - kFitListWidth - kPartitionHandleWidth,
			kColHeaderHeight);
	assert(itsParameterColHeader != NULL);

	itsParameterTable->SetColHeaderWidget(itsParameterColHeader);

	itsFitMenu = menuBar->AppendTextMenu(kFitMenuTitleStr);
	itsFitMenu->SetMenuItems(kFitMenuStr);
	itsFitMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsFitMenu);

	const JCoordinate kChiSqLabelWidth	= 170;

	GLChiSqLabel* label =
		jnew GLChiSqLabel(container,
			JXWidget::kFixedLeft, JXWidget::kFixedBottom,
			0, kParmsTableHeight + kColHeaderHeight,
			kChiSqLabelWidth, kChiSqHeight);
	assert(label != NULL);

	JXDownRect* downRect =
		jnew JXDownRect(container,
			JXWidget::kHElastic, JXWidget::kFixedBottom,
			kChiSqLabelWidth, kParmsTableHeight + kColHeaderHeight,
			w - kFitListWidth - kPartitionHandleWidth, kChiSqHeight);
	assert(downRect != NULL);

	itsChiSq =
		jnew JXStaticText("", container,
			JXWidget::kHElastic, JXWidget::kFixedBottom,
			kChiSqLabelWidth + kJXDefaultBorderWidth,
			kParmsTableHeight + kColHeaderHeight + kJXDefaultBorderWidth,
			w - kFitListWidth - kPartitionHandleWidth - 2 * kJXDefaultBorderWidth,
			kChiSqHeight - 2 * kJXDefaultBorderWidth);
	assert(itsChiSq != NULL);
	itsChiSq->SetBackColor(GetColormap()->GetWhiteColor());

	// now add the 2 plots

	container = itsPlotPartition->GetCompartment(2);

	itsFitPlot	=
		jnew JX2DPlotWidget(menuBar, container,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			w - kFitListWidth - kPartitionHandleWidth,
			container->GetApertureHeight());
	assert(itsFitPlot != NULL);
	itsFitPlot->SetTitle(kFitPlotTitle);
	itsFitPlot->SetXLabel(itsPlot->GetXLabel());
	itsFitPlot->SetYLabel(itsPlot->GetYLabel());

	container = itsPlotPartition->GetCompartment(3);

	itsDiffPlot	=
		jnew JX2DPlotWidget(itsFitPlot, container,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0,
			w - kFitListWidth - kPartitionHandleWidth,
			newHeight - kFirstPlotHeight - kTotalParmsHeight - 2 * kPartitionHandleWidth);
	assert(itsDiffPlot != NULL);
	itsDiffPlot->SetTitle(kDiffPlotTitle);
	itsDiffPlot->SetXLabel(itsPlot->GetXLabel());
	itsDiffPlot->SetYLabel(itsPlot->GetYLabel());
	itsDiffPlot->ShowFrame(kJFalse);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd, jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, JXPM(jx_help_specific));

	itsCurveList->SetCurrentCurveIndex(1);

	GetPrefsMgr()->ReadFitDirectorSetup(this);
}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
GLFitDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFitMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFitMenu(selection->GetIndex());
		}
	else if (sender == itsFitMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFitMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}
	else if (sender == itsCurveList && message.Is(GLCurveNameList::kCurveSelected))
		{
		const GLCurveNameList::CurveSelected* info =
			dynamic_cast<const GLCurveNameList::CurveSelected*>(&message);
		assert(info != NULL);
		JPlotDataBase& curve = itsPlot->GetCurve(info->GetIndex());

		RemoveFit();
		RemoveCurves();

		itsParameterTable->ClearValues();
		itsChiSq->SetText("");

		// add new curve.
		itsFitPlot->AddCurve(&curve, kJFalse, itsPlot->GetCurveName(info->GetIndex()));
		itsFitPlot->ProtectCurve(1, kJTrue);
		}
	else if (sender == itsFitList && message.Is(GLFitDescriptionList::kFitSelected))
		{
		const GLFitDescriptionList::FitSelected* info =
			dynamic_cast<const GLFitDescriptionList::FitSelected*>(&message);
		assert(info != NULL);
		const GLFitDescription& fd	= GetFitManager()->GetFitDescription(info->GetIndex());
		itsParameterTable->SetFitDescription(fd);
		RemoveFit();
		itsChiSq->SetText("");
		JFunction* f;
		if (JParseFunction(fd.GetFitFunctionString(), const_cast<GLFitDescription&>(fd).GetVarList(), &f))
			{
			itsExprWidget->SetFunction(const_cast<GLFitDescription&>(fd).GetVarList(), f);
			itsExprWidget->Show();
			}
		}
	else if (sender == itsFitList && message.Is(GLFitDescriptionList::kFitInitiated))
		{
		const GLFitDescriptionList::FitInitiated* info =
			dynamic_cast<const GLFitDescriptionList::FitInitiated*>(&message);
		assert(info != NULL);
		const GLFitDescription& fd	= GetFitManager()->GetFitDescription(info->GetIndex());
		if (!itsParameterTable->BeginEditingStartValues())
			{
			Fit();
			AddHistoryText();
			}
		}
	else if (sender == itsParameterTable && message.Is(GLFitParameterTable::kValueChanged))
		{
		const GLFitParameterTable::ValueChanged* info =
			dynamic_cast<const GLFitParameterTable::ValueChanged*>(&message);
		assert(info != NULL);
		JIndex index;
		JBoolean ok	= itsFitList->GetCurrentFitIndex(&index);
		assert(ok);
		GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count	= parms.GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			fd.GetVarList()->SetValue(i + 1, parms.GetElement(i));
			}
		GetWindow()->Refresh();
		}
	else if (sender == itsNLFitDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			GLNonLinearFitDescription* fit	=
				jnew GLNonLinearFitDescription(itsNLFitDialog->GetFitName(),
					itsNLFitDialog->GetFunctionString(),
					itsNLFitDialog->GetDerivativeString(),
					itsNLFitDialog->GetVarList().GetVariables());
			assert(fit != NULL);
			GetFitManager()->AddFitDescription(fit);
			}
		itsNLFitDialog	= NULL;
		}
	else if (sender == itsPolyFitDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			JArray<JIndex> powers;
			itsPolyFitDialog->GetPowers(&powers);
			GLPolyFitDescription* fit	=
				jnew GLPolyFitDescription(itsPolyFitDialog->GetFitName(),
					powers);
			assert(fit != NULL);
			GetFitManager()->AddFitDescription(fit);
			}
		itsPolyFitDialog	= NULL;
		}
	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			Print();
			}
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 HandleFitMenu

 ******************************************************************************/

void
GLFitDirector::HandleFitMenu
	(
	const JIndex index
	)
{
	if (index == kNonLinearCmd)
		{
		itsNLFitDialog	= jnew GLNonLinearFitDialog(this);
		assert(itsNLFitDialog != NULL);
		itsNLFitDialog->BeginDialog();
		ListenTo(itsNLFitDialog);
		}
	else if (index == kPolyCmd)
		{
		itsPolyFitDialog	= jnew GLPolyFitDialog(this);
		assert(itsPolyFitDialog != NULL);
		itsPolyFitDialog->BeginDialog();
		ListenTo(itsPolyFitDialog);
		}
	else if (index == kRemoveFitCmd)
		{
		JIndex index;
		if (itsFitList->GetCurrentFitIndex(&index))
			{
			itsExprWidget->ClearFunction();
			itsExprWidget->Hide();
			GetFitManager()->RemoveFitDescription(index);
			}
		}
	else if (index == kFitCmd)
		{
		Fit();
		AddHistoryText();
		}
	else if (index == kTestFitCmd)
		{
		TestFit();
		}
	else if (index == kShowStartCmd)
		{
		itsParameterTable->ShowStartCol(!itsParameterTable->IsShowingStartCol());
		}
	else if (index == kRefitCmd)
		{
		Refit();
		AddHistoryText(kJTrue);
		}
	else if (index == kPlotCmd)
		{
//		JString fn	= itsCurrentFit->GetFitFunctionString();
// 		need to check - via
// 		JFunction* f;
//		if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
//			{
//			return kJTrue;
//			}
//		whether or not the function is valid.

		JIndex index;
		JBoolean ok	= itsCurveList->GetCurrentCurveIndex(&index);
		JPlotDataBase* data	= &(itsPlot->GetCurve(index));
		assert(itsCurrentFit != NULL);
		JPlotFitProxy* proxy	= jnew JPlotFitProxy(itsCurrentFit, itsPlot, data);
		assert(proxy != NULL);
		JIndex findex;
		ok	= itsFitList->GetCurrentFitIndex(&findex);
		assert(ok);
		const GLFitDescription& fd	= GetFitManager()->GetFitDescription(findex);
		itsDir->AddFitProxy(proxy, index, fd.GetFnName());
		}
	else if (index == kShowHistoryCmd)
		{
		itsHistory->Activate();
		}
	else if (index == kPrintCmd)
		{
		itsPrinter->BeginUserPrintSetup();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
}

/******************************************************************************
 UpdateFitMenu

 ******************************************************************************/

void
GLFitDirector::UpdateFitMenu()
{
	JIndex index;
	if (itsCurveList->GetCurrentCurveIndex(&index) &&
		itsFitList->GetCurrentFitIndex(&index))
		{
		itsFitMenu->EnableItem(kFitCmd);
		const GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
		if (fd.RequiresStartValues())
			{
			itsFitMenu->EnableItem(kTestFitCmd);
			itsFitMenu->CheckItem(kShowStartCmd);
			itsFitMenu->DisableItem(kShowStartCmd);
			if (itsCurrentFit != NULL)
				{
				itsFitMenu->EnableItem(kRefitCmd);
				}
			}
		else if (fd.CanUseStartValues())
			{
			itsFitMenu->EnableItem(kShowStartCmd);
			if (itsParameterTable->IsShowingStartCol())
				{
				itsFitMenu->CheckItem(kShowStartCmd);
				itsFitMenu->EnableItem(kTestFitCmd);
				}
			if (itsCurrentFit != NULL)
				{
				itsFitMenu->EnableItem(kRefitCmd);
				}
			}
		if (GetFitManager()->FitIsRemovable(index))
			{
			itsFitMenu->EnableItem(kRemoveFitCmd);
			}
		if (itsCurrentFit != NULL)
			{
			itsFitMenu->EnableItem(kPlotCmd);
			itsFitMenu->EnableItem(kPrintCmd);
			}
		}
	itsFitMenu->EnableItem(kNonLinearCmd);
	itsFitMenu->EnableItem(kPolyCmd);
	itsFitMenu->EnableItem(kShowHistoryCmd);
	itsFitMenu->EnableItem(kCloseCmd);
}

/******************************************************************************
 HandlePrefsMenu


 ******************************************************************************/

void
GLFitDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
		{
//		GWIGetPrefsMgr()->EditPrefs();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kSaveWindowSizeCmd)
		{
		GetPrefsMgr()->WriteFitDirectorSetup(this);
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
GLFitDirector::HandleHelpMenu
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
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection("GLFitHelp");
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
}

/******************************************************************************
 Prefs (protected)

 ******************************************************************************/

void
GLFitDirector::ReadPrefs
	(
	std::istream& input
	)
{
	int version;
	input >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}
	GetWindow()->ReadGeometry(input);
	itsMainPartition->ReadGeometry(input);
	itsListPartition->ReadGeometry(input);
	itsPlotPartition->ReadGeometry(input);
	if (version >= 1)
		{
		itsPrinter->ReadXPSSetup(input);
		}
}

void
GLFitDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ' ;
	GetWindow()->WriteGeometry(output);
	itsMainPartition->WriteGeometry(output);
	itsListPartition->WriteGeometry(output);
	itsPlotPartition->WriteGeometry(output);
	itsPrinter->WriteXPSSetup(output);
}

/******************************************************************************
 Fit (private)

 ******************************************************************************/

void
GLFitDirector::Fit()
{
	JXGetApplication()->DisplayBusyCursor();
	RemoveFit();
	JIndex index;
	JBoolean ok	= itsCurveList->GetCurrentCurveIndex(&index);
	JPlotDataBase* data	= &(itsPlot->GetCurve(index));
	ok	= itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	const GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
	if (fd.GetType() == GLFitDescription::kPolynomial)
		{
		JArray<JIndex> powers;
		const GLPolyFitDescription& pd	= dynamic_cast<const GLPolyFitDescription&>(fd);
		pd.GetPowers(&powers);
		assert(ok);

		JFloat xmax, xmin, ymax, ymin;
		JPlotFitLinearEq* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitLinearEq(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitLinearEq(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		fit->InitializePolynomial(powers);
		if (itsParameterTable->IsShowingStartCol())
			{
			JVector p(const_cast<GLFitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count	= p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
				{
				p.SetElement(i, parms.GetElement(i));
				}
			fit->JPlotFitBase::GenerateFit(p, 0);
			}
		else
			{
			fit->GenerateFit();
			}
		itsCurrentFit = fit;
		}
	else if (fd.GetType() == GLFitDescription::kNonLinear)
		{
		GLNonLinearFitDescription& nd	=
			dynamic_cast<GLNonLinearFitDescription&>(const_cast<GLFitDescription&>(fd));

		JFloat xmax, xmin, ymax, ymin;
		JPlotFitNonLinear* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitNonLinear(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitNonLinear(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		fit->SetVarList(nd.GetVarList());
		fit->SetFunction(nd.GetFunctionString());
		JString fp	= nd.GetFunctionPrimedString();
		if (!fp.IsEmpty())
			{
			fit->SetFPrimed(fp);
			}
		JVector p(nd.GetVarList()->GetVariableCount() - 1);
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count	= p.GetDimensionCount();
		for (JIndex i = 1; i <= count; i++)
			{
			p.SetElement(i, parms.GetElement(i));
			}
		fit->SetInitialParameters(p);
		itsCurrentFit = fit;
		}
	else if (fd.GetType() == GLFitDescription::kBLinear)
		{
		JFloat xmax, xmin, ymax, ymin;
		JPlotFitLinear* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitLinear(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitLinear(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		if (itsParameterTable->IsShowingStartCol())
			{
			JVector p(const_cast<GLFitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count	= p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
				{
				p.SetElement(i, parms.GetElement(i));
				}
			fit->JPlotFitBase::GenerateFit(p, 0);
			}
		else
			{
			fit->GenerateFit();
			}
		itsCurrentFit = fit;
		}
	else if (fd.GetType() == GLFitDescription::kBExp)
		{
		JFloat xmax, xmin, ymax, ymin;
		JPlotFitExp* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitExp(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitExp(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		if (itsParameterTable->IsShowingStartCol())
			{
			JVector p(const_cast<GLFitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count	= p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
				{
				p.SetElement(i, parms.GetElement(i));
				}
			fit->JPlotFitBase::GenerateFit(p, 0);
			}
		else
			{
			fit->GenerateFit();
			}
		itsCurrentFit = fit;
		}
	else if (fd.GetType() == GLFitDescription::kBPower)
		{
		JFloat xmax, xmin, ymax, ymin;
		JPlotFitPowerLaw* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitPowerLaw(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitPowerLaw(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		if (itsParameterTable->IsShowingStartCol())
			{
			JVector p(const_cast<GLFitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count	= p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
				{
				p.SetElement(i, parms.GetElement(i));
				}
			fit->JPlotFitBase::GenerateFit(p, 0);
			}
		else
			{
			fit->GenerateFit();
			}
		itsCurrentFit = fit;
		}
	else if (fd.GetType() == GLFitDescription::kModule)
		{
		GLModuleFitDescription& md	=
			dynamic_cast<GLModuleFitDescription&>(const_cast<GLFitDescription&>(fd));
		JFloat xmax, xmin, ymax, ymin;
		JPlotFitModule* fit;
		if (itsFitPlot->IsUsingRange())
			{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew JPlotFitModule(itsFitPlot, data, xmin, xmax, ymin, ymax);
			assert(fit != NULL);
			}
		else
			{
			data->GetXRange(&xmin, &xmax);
			fit = jnew JPlotFitModule(itsFitPlot, data, xmin, xmax);
			assert(fit != NULL);
			}
		fit->SetFitModule(md.GetFitModule());
		JVector p(md.GetParameterCount());
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count	= p.GetDimensionCount();
		for (JIndex i = 1; i <= count; i++)
			{
			p.SetElement(i, parms.GetElement(i));
			}
		fit->SetInitialParameters(p);
		itsCurrentFit = fit;
		}
	itsFitPlot->AddCurve(itsCurrentFit, kJFalse, fd.GetFnName());
	itsDiffPlot->AddCurve(itsCurrentFit->GetDiffData(), kJFalse, fd.GetFnName());
	const JSize count	= itsCurrentFit->GetParameterCount();
	for (JIndex i = 1; i <= count; i++)
		{
		JFloat value;
		JBoolean ok	= itsCurrentFit->GetParameter(i, &value);
		assert(ok);
		JFloat error = 0;
		itsCurrentFit->GetParameterError(i, &error);
		itsParameterTable->SetValue(i, value, error);
		if (itsCurrentFit->GetGoodnessOfFit(&value))
			{
			itsChiSq->SetText(JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
			}
		else
			{
			itsChiSq->SetText("");
			}

		}
}

/******************************************************************************
 Refit (private)

 ******************************************************************************/

void
GLFitDirector::Refit()
{
	itsParameterTable->CopyParmValuesToStart();
	Fit();
}

/******************************************************************************
 Plot (private)

 ******************************************************************************/

void
GLFitDirector::Plot()
{
}

/******************************************************************************
 RemoveCurves (private)

 ******************************************************************************/

void
GLFitDirector::RemoveCurves()
{
	// remove fit curves.
	const JSize count	= itsFitPlot->GetCurveCount();
	if (count >= 1)
		{
		itsFitPlot->ProtectCurve(1, kJFalse);
		itsFitPlot->RemoveCurve(1);
		}
}

/******************************************************************************
 RemoveFit (private)

 ******************************************************************************/

void
GLFitDirector::RemoveFit()
{
	// remove fit.
	const JSize count	= itsFitPlot->GetCurveCount();
	for (JIndex i = 2; i <= count; i++)
		{
		itsFitPlot->ProtectCurve(2, kJFalse);
		itsFitPlot->RemoveCurve(2);
		}

	// remove diff curve.
	const JSize fcount	= itsDiffPlot->GetCurveCount();
	for (JIndex i = 1; i <= fcount; i++)
		{
		itsDiffPlot->ProtectCurve(1, kJFalse);
		itsDiffPlot->RemoveCurve(1);
		}

	jdelete itsCurrentFit;
	itsCurrentFit	= NULL;
	jdelete itsTestFunction;
	itsTestFunction	= NULL;
	GetWindow()->Refresh();
}

/******************************************************************************
 TestFit (private)

 ******************************************************************************/

void
GLFitDirector::TestFit()
{
	RemoveFit();
	JIndex index;
	JBoolean ok	= itsCurveList->GetCurrentCurveIndex(&index);
	JPlotDataBase* data	= &(itsPlot->GetCurve(index));
	ok	= itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
	const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
	const JSize count	= parms.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		fd.GetVarList()->SetValue(i + 1, parms.GetElement(i));
		}
	JFloat xmin, xmax;
	data->GetXRange(&xmin, &xmax);
	ok	= J2DPlotJFunction::Create(&itsTestFunction, itsFitPlot, fd.GetVarList(),
			fd.GetFitFunctionString(), 1, xmin, xmax);
	if (ok)
		{
		itsFitPlot->AddCurve(itsTestFunction, kJFalse, fd.GetFnName());
		}

}

/******************************************************************************
 AddHistoryText (private)

 ******************************************************************************/

void
GLFitDirector::AddHistoryText
	(
	const JBoolean refit
	)
{
	JString str;
	if (refit)
		{
		str	= "Refit\n";
		}
	else
		{
		str	= "Fit\n";
		}
	itsHistory->AppendText(str, kJFalse);
	JIndex index;
	JBoolean ok	= itsCurveList->GetCurrentCurveIndex(&index);
	assert(ok);
	str	= "Curve: " + itsPlot->GetCurveName(index) + "\n";
	itsHistory->AppendText(str, kJFalse);
	ok	= itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	const GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
	str	= "Fit type: " + fd.GetFnName() + "\n";
	itsHistory->AppendText(str, kJFalse);
	str.Clear();
	itsParameterTable->GetValueString(&str);
	itsHistory->AppendText(str, kJFalse);
	str = "Reduced Chi^2: " + itsChiSq->GetText() + "\n";
	itsHistory->AppendText(str, kJFalse);
	itsHistory->AppendText("\n\n", kJFalse);
}

/******************************************************************************
 Print

 ******************************************************************************/

void
GLFitDirector::Print()
{
	if (itsPrinter->OpenDocument())
		{
		if (itsPrinter->NewPage())
			{
			JCoordinate kTopMargin	= 20;
			JCoordinate kLeftMargin	= 20;
			JCoordinate width		= (JCoordinate)(itsPrinter->GetPageWidth() * 0.8);
			JCoordinate tableD		= 70;
			JCoordinate kPlotSep	= 20;
			JCoordinate kExprX		= itsPrinter->GetPageWidth() - kLeftMargin - itsExprWidget->GetBoundsWidth();
			JCoordinate kLMargin	= (itsPrinter->GetPageWidth() - width)/2;

			// draw header info
			JString str	= "Curve: ";
			JIndex index;
			JBoolean ok	= itsCurveList->GetCurrentCurveIndex(&index);
			assert(ok);
			str	+= + itsPlot->GetCurveName(index);
			itsPrinter->String(kLeftMargin, 0, str);

			str	= "Fit: ";
			ok	= itsFitList->GetCurrentFitIndex(&index);
			assert(ok);
			const GLFitDescription& fd	= GetFitManager()->GetFitDescription(index);
			str += fd.GetFnName();
			itsPrinter->String(kLeftMargin, kPlotSep, str);

			str = "Normalized chi-squared (chi^2/n-";
			str += JString(fd.GetParameterCount(), JString::kBase10) + "): ";
			str += itsChiSq->GetText();
			itsPrinter->String(kLeftMargin, kPlotSep*2, str);

			// draw expression widget
			JRect eRect	= itsExprWidget->GetPrintBounds();
			itsExprWidget->DrawForPrint(*itsPrinter, JPoint(itsPrinter->GetPageWidth() - eRect.width(), 0));

			if (eRect.height() > tableD)
				{
				tableD	= eRect.height() + 5;
				}

			// draw table
			itsParameterTable->DrawForPrint(*itsPrinter, JPoint(0, tableD), kJFalse, kJTrue);
			JCoordinate parmHeight = itsParameterTable->GetBoundsHeight() + itsParameterColHeader->GetBoundsHeight();

			// draw plots
			JCoordinate height = (itsPrinter->GetPageHeight() - tableD - parmHeight)/2;
			JRect r;//(0, 0, height, width);
			r.left		= kLMargin;
			r.right		= r.left + width;
			r.top		= tableD + parmHeight;
			r.bottom	= r.top + height;
//			itsPrinter->ShiftOrigin(kLMargin, tableD + parmHeight);
			itsFitPlot->Print(*itsPrinter);//, r);
			r.Shift(0, height);
			itsDiffPlot->Print(*itsPrinter);//, r);
			}
		itsPrinter->CloseDocument();
		}
}
