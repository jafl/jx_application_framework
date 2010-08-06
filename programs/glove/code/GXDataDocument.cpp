/******************************************************************************
 GXDataDocument.cpp

	BASE CLASS = JXFileDocument

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXDataDocument.h"
#include "GXRaggedFloatTable.h"
#include "GloveModule.h"
#include "GlovePlotter.h"
#include "GLPlotApp.h"
#include "GXChooseFileImportDialog.h"
#include "GXGetDelimiterDialog.h"
#include "PlotDir.h"
#include "JX2DPlotWidget.h"
#include "J2DPlotData.h"
#include "J2DVectorData.h"
#include "GXRowHeaderWidget.h"
#include "GXColHeaderWidget.h"
#include "GRaggedFloatTableData.h"
#include "GLGlobals.h"
#include "GLHelpText.h"
#include "GLPrefsMgr.h"

#include "filenew.xpm"
#include "fileopen.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"
#include "manual.xpm"

#include "JXToolBar.h"

#include <JXDialogDirector.h>
#include <JXHelpDirector.h>
#include <JXDocumentMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXApplication.h>
#include <JXPSPrinter.h>
#include <JXImage.h>
#include <JXTextButton.h>
#include <jXActionDefs.h>

#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JLatentPG.h>
#include <JOutPipeStream.h>

#include <jErrno.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <sstream>
#include <jFileUtil.h>
#include <jAssert.h>

// File menu information

static const JCharacter* kFileMenuTitleStr  = "File";
static const JCharacter* kFileMenuStr =
	"    New %k Meta-N %i New::GXDataDocument" 
	"  | Open... %k Meta-O %i Open::GXDataDocument"
	"  | Save %k Meta-S %i Save::GXDataDocument"
	"  | Save as..."
	"%l| Export module"
	"%l| Page setup... "
	"  | Print... %k Meta-P %i "kJXPrintAction
	"%l| Close %k Meta-W %i "kJXCloseWindowAction
	"  | Quit %k Meta-Q %i "kJXQuitAction;

static const JCharacter* kGloveFileSignature   = "*** Glove File Format ***";
static const JFileVersion kCurrentGloveVersion = 3;

// version 2:
//	Fixed problem with PlotDir::WriteData() (needed a space)
//	Changed PlotDir to use PWXRead/WriteSetup()

enum
{
	kNewCmd = 1,
	kOpenCmd,
	kSaveCmd,
	kSaveAsCmd,
	kExportCmd,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

const JSize kMaxPlotTitleSize = 20;

static const JCharacter* kWindowListMenuTitleStr = "Windows";

static const JCharacter* kExportMenuTitleStr = "Export modules";
static const JCharacter* kExportMenuStr =
	"Reload %l";

enum
{
	kReloadModuleCmd = 1
};

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"About "
	"%l| Table of Contents %i TOC::GXDataDocument"
	"  | This window %i ThisWindow::GXDataDocument"
	"%l|Changes|Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

enum
{
	kDataPlot = 1,
	kVectorPlot
};

const JCoordinate kButtConBuffer 	= 5;
const JCoordinate kButtConWidth 	= 30;
const JCoordinate kToolBarHeight 	= 40;

const JCoordinate kEditButtonStart	= 130;

static const JCharacter* kInternalModuleNames[] =
{
	"Text (delimited)",
	"Text (fixed-width)"
};

const JSize kInternalModuleCount = sizeof(kInternalModuleNames)/sizeof(JCharacter*);

enum
{
	kDelimitedText = 1,
	kFixedWidthText
};

/******************************************************************************
 Constructor

 ******************************************************************************/

GXDataDocument::GXDataDocument
	(
	JXDirector*			supervisor,
	const JCharacter*	fileName,
	const JBoolean		onDisk
	)
	:
	JXFileDocument(supervisor, fileName, onDisk, kJFalse, ".glv")
{
	itsData = new GRaggedFloatTableData(0.0);
	assert( itsData != NULL );

	itsPlotWindows = new JPtrArray<PlotDir>(JPtrArrayT::kForgetAll);
	assert( itsPlotWindows != NULL );

	itsPrinter          = NULL;
	itsFileImportDialog = NULL;
	itsDelimiterDialog  = NULL;

	BuildWindow();

	itsPrinter = new JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	ListenTo(itsPrinter);

	itsPlotNumber = 1;
	UpdateExportMenu();
	itsListenToData = kJTrue;
	ListenTo(itsData);

	if (onDisk)
		{
		LoadFile(fileName);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXDataDocument::~GXDataDocument()
{
	delete itsData;
	delete itsPrinter;
	delete itsPlotWindows;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXDataDocument::BuildWindow()
{
	JSize w = 453;
	JSize h = 360;

    JXWindow* window = new JXWindow(this, w,h, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop,
                    0,0, w, kJXDefaultMenuBarHeight);
    assert( menuBar != NULL );

    JXToolBar* toolBar =
    	new JXToolBar(GetPrefsMgr(), kDataToolBarID, menuBar,
    				150, 150, window,
                    JXWidget::kHElastic, JXWidget::kVElastic,
                    0,kJXDefaultMenuBarHeight,
                    w,h - kJXDefaultMenuBarHeight);
    assert( toolBar != NULL );

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	JXImage* image = new JXImage(GetDisplay(), JXPM(filenew));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kNewCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(fileopen));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kOpenCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(filefloppy));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kSaveCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(fileprint));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kPrintCmd, image, kJFalse);

	const JCoordinate scrollheight = 
		toolBar->GetWidgetEnclosure()->GetBoundsHeight();
	
    itsScrollbarSet =
        new JXScrollbarSet(toolBar->GetWidgetEnclosure(), 
                    JXWidget::kHElastic, JXWidget::kVElastic,
                    0,0,
                    w, scrollheight);
    assert( itsScrollbarSet != NULL );

	AdjustWindowTitle();

	// layout table and headers

	const JCoordinate kRowHeaderWidth  = 30;
	const JCoordinate kColHeaderHeight = 20;

	JXContainer* encl = itsScrollbarSet->GetScrollEnclosure();
	JRect enclApG     = encl->GetApertureGlobal();

	JXTextButton* okButton =
		new JXTextButton("OK", encl,
						JXWidget::kFixedLeft, JXWidget::kFixedTop,
						0, 0, kRowHeaderWidth-2, kColHeaderHeight-2);
	assert(okButton != NULL);

	itsTable =
		new GXRaggedFloatTable(this, okButton, itsData, 6,
							menuBar, itsScrollbarSet, encl,
						   	JXWidget::kHElastic, JXWidget::kVElastic,
						   	kRowHeaderWidth,kColHeaderHeight,
						   	enclApG.width()  - kRowHeaderWidth,
						   	enclApG.height() - kColHeaderHeight);
	assert( itsTable != NULL );

	enclApG = encl->GetApertureGlobal();	// JXScrollableWidget forces a change in this

	GXRowHeaderWidget* rowHeader =
		new GXRowHeaderWidget(itsTable, itsScrollbarSet, encl,
							  JXWidget::kFixedLeft, JXWidget::kVElastic,
							  0,kColHeaderHeight, kRowHeaderWidth,
							  enclApG.height() - kColHeaderHeight);
	assert( rowHeader != NULL );

	GXColHeaderWidget* colHeader =
		new GXColHeaderWidget(itsTable, itsScrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  kRowHeaderWidth,0, enclApG.width() - kRowHeaderWidth,
							  kColHeaderHeight);
	colHeader->TurnOnColResizing(20);
	assert( colHeader != NULL );

	JXDocumentMenu* windowListMenu =
		new JXDocumentMenu(kWindowListMenuTitleStr, menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowListMenu != NULL );
	menuBar->AppendMenu(windowListMenu);

	itsExportMenu = new JXTextMenu(itsFileMenu, kExportCmd, menuBar);
	assert( itsExportMenu != NULL );
	itsExportMenu->SetMenuItems(kExportMenuStr);
	itsExportMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsExportMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);


	image = new JXImage(GetDisplay(), JXPM(manual));
	assert(image != NULL);
	itsHelpMenu->SetItemImage(kTOCCmd, image, kJFalse);

	toolBar->LoadPrefs();
	if (toolBar->IsEmpty())
		{
		toolBar->AppendButton(itsFileMenu, kNewCmd);
		toolBar->AppendButton(itsFileMenu, kOpenCmd);
		toolBar->AppendButton(itsFileMenu, kSaveCmd);
		toolBar->NewGroup();
		toolBar->AppendButton(itsFileMenu, kPrintCmd);

		itsTable->LoadDefaultToolButtons(toolBar);
		
		toolBar->NewGroup();
		toolBar->AppendButton(itsHelpMenu, kTOCCmd);
		}

}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GXDataDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsExportMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleExportMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert(info != NULL);
		if (info->Successful())
			{
			itsTable->PrintRealTable(*itsPrinter);
			}
		}

	else if (sender == itsFileImportDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			LoadImportFile();
			}
		itsFileImportDialog = NULL;
		}

	else if (sender == itsDelimiterDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			LoadDelimitedFile();
			}
		itsDelimiterDialog = NULL;
		}

	else if (sender == itsData && itsListenToData)
		{
		DataModified();
		}

	else if (message.Is(J2DPlotWidget::kPlotChanged) ||
			 message.Is(J2DPlotWidget::kTitleChanged) ||
			 message.Is(J2DPlotWidget::kIsEmpty))
		{
		DataModified();
		}

	else
		{
		JXFileDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
GXDataDocument::UpdateFileMenu()
{
	itsFileMenu->EnableItem(kOpenCmd);
	itsFileMenu->EnableItem(kNewCmd);
	if (NeedsSave())
		{
		itsFileMenu->EnableItem(kSaveCmd);
		}
	itsFileMenu->EnableItem(kSaveAsCmd);
	itsFileMenu->EnableItem(kPageSetupCmd);
	itsFileMenu->EnableItem(kCloseCmd);
	itsFileMenu->EnableItem(kQuitCmd);

	if (!itsData->IsEmpty())
		{
		itsFileMenu->EnableItem(kPrintCmd);
		}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
GXDataDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
		{
		(GLGetApplication())->NewFile();
		}
	else if (index == kOpenCmd)
		{
		JString filename;
		if (JGetChooseSaveFile()->ChooseFile("Select Data File", "", &filename))
			{
			(GLGetApplication())->OpenFile(filename);
			}
		}
	else if (index == kSaveCmd)
		{
		SaveInCurrentFile();
		}
	else if (index == kSaveAsCmd)
		{
		SaveInNewFile();
		}
	else if (index == kPageSetupCmd)
		{
		itsPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintCmd && itsTable->EndEditing())
		{
		itsPrinter->BeginUserPrintSetup();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}

	else if (index == kQuitCmd)
		{
		(GLGetApplication())->Quit();
		}
}

/******************************************************************************
 LoadFile (private)

 ******************************************************************************/

void
GXDataDocument::LoadFile
	(
	const JCharacter* fileName
	)
{
	ifstream is(fileName);

	if (is.bad())
		{
		JGetUserNotification()->ReportError("Error opening file.");
		}
	else
		{
		const JString str = JReadLine(is);
		if (str == kGloveFileSignature)
			{
			if (!LoadNativeFile(is))
				{
				JGetUserNotification()->ReportError(
					"This file was created by a newer version of Glove.  "
					"You need the newest version in order to open it.");
				}
			}
		else
			{
			is.close();
			FileChanged(fileName, kJFalse);
			itsCurrentFileName = fileName;
			ChooseFileFilter();
			}
		}
}

/******************************************************************************
 LoadNativeFile (private)

 ******************************************************************************/

JBoolean
GXDataDocument::LoadNativeFile
	(
	istream& is
	)
{
	JFloat version;
	is >> version;

	if (version > (JFloat) kCurrentGloveVersion)
		{
		return kJFalse;
		}

	itsListenToData = kJFalse;
	itsTable->ReadData(is, version);
	if (version > 0.5)
		{
		ReadPlotData(is, version);
		GetWindow()->ReadGeometry(is);
		JXScrollbar* vscroll = itsScrollbarSet->GetVScrollbar();
		vscroll->ReadSetup(is);
		JXScrollbar* hscroll = itsScrollbarSet->GetHScrollbar();
		hscroll->ReadSetup(is);
		}
	itsListenToData = kJTrue;
	DataReverted();
	return kJTrue;
}

/******************************************************************************
 ChooseFileFilter (private)

 ******************************************************************************/

void
GXDataDocument::ChooseFileFilter()
{
	assert (itsFileImportDialog == NULL);
	itsFileImportDialog = new GXChooseFileImportDialog(this, itsCurrentFileName);
	assert (itsFileImportDialog != NULL);
	ListenTo(itsFileImportDialog);
	itsFileImportDialog->BeginDialog();
}

/******************************************************************************
 LoadImportFile (private)

 ******************************************************************************/

void
GXDataDocument::LoadImportFile()
{
	JIndex filterIndex = itsFileImportDialog->GetFilterIndex();
	if (filterIndex <= kInternalModuleCount)
		{
		LoadInternalFile(filterIndex);
		}
	else
		{
		JString filter;
		if (!((GLGetApplication())->GetImportModulePath(filterIndex - kInternalModuleCount, &filter)))
			{
			return;
			}

		const JCharacter* argv[] = { filter, itsCurrentFileName, NULL };

		int inFD;
		pid_t pid;
		JError err = JExecute(argv, sizeof(argv), &pid,
							  kJIgnoreConnection, NULL,
							  kJCreatePipe, &inFD,
							  kJIgnoreConnection, NULL);

		if (!err.OK())
			{
			JGetUserNotification()->ReportError("Error executing filter.");
			return;
			}

		ifstream ip;
		JString tempName;
		if (JConvertToStream(inFD, &ip, &tempName))
			{
			int type;
			ip >> type;

			if (type == kGloveDataError)
				{
				JGetUserNotification()->ReportError("This file is not readable by this filter.");
				return;
				}

			itsData->ShouldBroadcast(kJFalse);
			itsListenToData = kJFalse;

			JLatentPG pg(10);
			pg.VariableLengthProcessBeginning("Loading file...", kJTrue, kJFalse);
			JBoolean keepGoing = kJTrue;

			if (type == kGloveMatrixDataFormat)
				{
				JSize colCount;
				ip >> colCount;

				for (JIndex i = 1; i <= colCount; i++)
					{
					itsData->AppendCol();
					}

				while (keepGoing)
					{
					for (JIndex i = 1; i <= colCount && keepGoing; i++)
						{
						JFloat value;
						ip >> value;
						if (ip.fail() || (ip.eof() && i < colCount))
							{
							keepGoing = kJFalse;
							break;
							}
						itsData->AppendElement(i, value);
						keepGoing = pg.IncrementProgress();
						}
					}
				}

			else if (type == kGloveRaggedDataFormat)
				{
				JSize colCount;
				ip >> colCount;

				for (JIndex colIndex=1; colIndex <= colCount && keepGoing; colIndex++)
					{
					itsData->AppendCol();

					JSize rowCount;
					ip >> rowCount;

					for (JIndex rowIndex=1; rowIndex <= rowCount && keepGoing; rowIndex++)
						{
						JFloat value;
						ip >> value;
						if (ip.fail() || (ip.eof() && rowIndex < rowCount))
							{
							keepGoing = kJFalse;
							break;
							}
						itsData->AppendElement(colIndex, value);

						keepGoing = pg.IncrementProgress();
						}
					}
				}

			ip.close();
			JRemoveFile(tempName);

			pg.ProcessFinished();
			}

		itsListenToData = kJTrue;
		itsData->ShouldBroadcast(kJTrue);
		}
}

/******************************************************************************
 CreateNewPlot

 ******************************************************************************/

void
GXDataDocument::CreateNewPlot
	(
	const JIndex			type,
	const JArray<JFloat>& 	xCol,
	const JArray<JFloat>* 	x2Col,
	const JArray<JFloat>& 	yCol,
	const JArray<JFloat>* 	y2Col,
	const JBoolean			linked,
	const JCharacter*		label
	)
{
	JString str = "Plot " + JString(itsPlotNumber);
	itsPlotNumber++;

	PlotDir* plotDir = new PlotDir(this, this, GetFileName());
	assert (plotDir != NULL);
	itsPlotWindows->Append(plotDir);

	JX2DPlotWidget* plot = plotDir->GetPlot();
	assert( plot != NULL );
	ListenTo(plot);

	AddToPlot(itsPlotWindows->GetElementCount(), type, xCol, x2Col, yCol, y2Col, linked, label);

	plot->SetTitle(str);
}


/******************************************************************************
 AddToPlot

 ******************************************************************************/

void
GXDataDocument::AddToPlot
	(
	const JIndex			plotIndex,
	const JIndex			type,
	const JArray<JFloat>&	xCol,
	const JArray<JFloat>*	x2Col,
	const JArray<JFloat>&	yCol,
	const JArray<JFloat>*	y2Col,
	const JBoolean			linked,
	const JCharacter*		label
	)
{
	assert( itsPlotWindows->IndexValid(plotIndex) );

	PlotDir* plotDir = itsPlotWindows->NthElement(plotIndex);
	assert (plotDir != NULL);

	JX2DPlotWidget* plot = plotDir->GetPlot();
	assert( plot != NULL );
	ListenTo(plot);

	if (type == kDataPlot)
		{
		J2DPlotData* curve;
		if (J2DPlotData::Create(&curve, xCol, yCol, linked))
			{
			plot->AddCurve(curve, kJTrue, label);
			}
		else
			{
			JGetUserNotification()->ReportError("The data columns must have the same number of elements.");
			return;
			}
		if (x2Col != NULL)
			{
			curve->SetXErrors(*x2Col);
			}
		if (y2Col != NULL)
			{
			curve->SetYErrors(*y2Col);
			}
		}
	else if (type == kVectorPlot)
		{
		J2DVectorData* curve;
		if (J2DVectorData::Create(&curve, xCol, yCol, *x2Col, *y2Col, linked))
			{
			plot->AddCurve(curve, kJTrue, label);
			}
		else
			{
			JGetUserNotification()->ReportError("The data columns must have the same number of elements.");
			return;
			}
		}
	plotDir->Activate();
	DataModified();
}

/******************************************************************************
 GetPlotNames

 ******************************************************************************/

void
GXDataDocument::GetPlotNames
	(
	JPtrArray<JString>& names
	)
{
	JString* name;

	const JIndex index = itsPlotWindows->GetElementCount();

	for (JIndex i = 1; i <= index; i++)
		{
		PlotDir* plotDir = itsPlotWindows->NthElement(i);
		assert (plotDir != NULL);

		JX2DPlotWidget* plot = plotDir->GetPlot();
		assert( plot != NULL );
		name = new JString(plot->GetTitle());
		if (name->GetLength() > kMaxPlotTitleSize)
			{
			name->RemoveSubstring(kMaxPlotTitleSize, name->GetLength());
			name->Append("...");
			}
		names.Append(name);
		}
}

/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void
GXDataDocument::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex index;
	const PlotDir* dir = (const PlotDir*) theDirector;
	if (itsPlotWindows->Find(dir, &index))
		{
		itsPlotWindows->Remove(dir);
		DataModified();
		}
}

/******************************************************************************
 WriteTextFile

 ******************************************************************************/

void
GXDataDocument::WriteTextFile
	(
	ostream& output,
	const JBoolean safetySave
	)
	const
{
	output << kGloveFileSignature << endl;
	output << kCurrentGloveVersion << endl;
	itsTable->WriteData(output);

	JSize plotCount = itsPlotWindows->GetElementCount();
	output << plotCount << " ";

	for (JSize i = 1; i <= plotCount; i++)
		{
		PlotDir* plotDir = itsPlotWindows->NthElement(i);
		assert (plotDir != NULL);
		plotDir->WriteSetup(output);
		plotDir->WriteData(output, itsData);
		}
	GetWindow()->WriteGeometry(output);
	JXScrollbar* vscroll = itsScrollbarSet->GetVScrollbar();
	vscroll->WriteSetup(output);
	JXScrollbar* hscroll = itsScrollbarSet->GetHScrollbar();
	hscroll->WriteSetup(output);
}

/******************************************************************************
 ReadPlotData

 ******************************************************************************/

void
GXDataDocument::ReadPlotData
	(
	istream& 		is,
	const JFloat	gloveVersion
	)
{
	JSize count;
	is >> count;
	for (JSize i = 1; i <= count; i++)
		{
		PlotDir* plotDir = new PlotDir(this, this, GetFileName());
		assert (plotDir != NULL);

		plotDir->ReadSetup(is, gloveVersion);
		plotDir->ReadData(is, itsData, gloveVersion);
		plotDir->Activate();
		itsPlotWindows->Append(plotDir);
		}
	itsPlotNumber = count + 1;
}

/******************************************************************************
 HandleExportMenu

 ******************************************************************************/

void
GXDataDocument::HandleExportMenu
	(
	const JIndex index
	)
{
	if (index == kReloadModuleCmd)
		{
		UpdateExportMenu();
		}
	else
		{
		JString modName;
		(GLGetApplication())->GetExportModulePath(index - 1, &modName);
		JString tempname = "Untitled.dat";
		JString filename;
		if (JGetChooseSaveFile()->SaveFile(tempname, "Export file as:", "", &filename))
			{
			const JCharacter* argv[] = { modName, filename, NULL };

			int inFD, outFD;
			pid_t pid;
			JError err = JExecute(argv, sizeof(argv), &pid,
								  kJCreatePipe, &outFD,
								  kJCreatePipe, &inFD,
								  kJIgnoreConnection, NULL);

			if (!err.OK())
				{
				JGetUserNotification()->ReportError("Error executing module.");
				return;
				}

			ifstream ip;
			JString tempName;
			if (JConvertToStream(inFD, &ip, &tempName))
				{
				JOutPipeStream op(outFD, kJTrue);
				assert( op.good() );

				int type;
				ip >> type;

				JSize colCount;
				JFloat value;

				if (type == kGloveMatrixDataFormat)
					{
					itsTable->ExportDataMatrix(op);
					}
				else
					{
					itsTable->ExportData(op);
					}

				ip.close();
				JRemoveFile(tempName);
				}
			}
		}
}

/******************************************************************************
 UpdateExportMenu

 ******************************************************************************/

void
GXDataDocument::UpdateExportMenu()
{
	const JSize mCount = itsExportMenu->GetItemCount();
	JSize i;
	for (i = 2; i <= mCount; i++)
		{
		itsExportMenu->RemoveItem(2);
		}

	(GLGetApplication())->ReloadExportModules();
	JPtrArray<JString>* names = (GLGetApplication())->GetExportModules();
	for (i = 1; i <= names->GetElementCount(); i++)
		{
		itsExportMenu->AppendItem(*(names->NthElement(i)));
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
GXDataDocument::HandleHelpMenu
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
		JXGetHelpManager()->ShowTOC();
		}
	else if (index == kThisWindowCmd)
		{
		JXGetHelpManager()->ShowSection(kGLTableHelpName);
		}
	else if (index == kChangesCmd)
		{
		JXGetHelpManager()->ShowSection(kGLChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		JXGetHelpManager()->ShowSection(kGLCreditsName);
		}
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
GXDataDocument::DiscardChanges()
{

}

/******************************************************************************
 GetInternalModuleName (public)

 ******************************************************************************/

const JCharacter*
GXDataDocument::GetInternalModuleName
	(
	const JIndex index
	)
{
	assert(index <= kInternalModuleCount);
	assert(index > 0);
	return kInternalModuleNames[index - 1];
}

/******************************************************************************
 GetInternalModuleCount (public)

 ******************************************************************************/

JSize
GXDataDocument::GetInternalModuleCount()
{
	return kInternalModuleCount;
}

/******************************************************************************
 LoadInternalFile (private)

 ******************************************************************************/

void
GXDataDocument::LoadInternalFile
	(
	const JIndex index
	)
{
	if (index == kDelimitedText)
		{
		assert(itsDelimiterDialog == NULL);
		itsDelimiterDialog =
			new GXGetDelimiterDialog(this, itsFileImportDialog->GetFileText());
		assert(itsDelimiterDialog != NULL);
		ListenTo(itsDelimiterDialog);
		itsDelimiterDialog->BeginDialog();
		}
	else if (index == kFixedWidthText)
		{
		(JGetUserNotification())->ReportError("Fixed width is not yet implemented.");
		}
}

/******************************************************************************
 LoadDelimitedFile (private)

 ******************************************************************************/

void
GXDataDocument::LoadDelimitedFile()
{
	assert(itsDelimiterDialog != NULL);

	const GXGetDelimiterDialog::DelimiterType type = itsDelimiterDialog->GetDelimiterType();
	JCharacter delim;
	if (type == GXGetDelimiterDialog::kChar)
		{
		delim = itsDelimiterDialog->GetCharacter();
		}
	else if (type == GXGetDelimiterDialog::kSpace)
		{
		delim = ' ';
		}
	else if (type == GXGetDelimiterDialog::kTab)
		{
		delim = '\t';
		}

	ifstream is(itsCurrentFileName);
	if (is.bad())
		{
		JGetUserNotification()->ReportError("Error opening file.");
		return;
		}

	if (itsDelimiterDialog->IsSkippingLines())
		{
		const JSize count = itsDelimiterDialog->GetSkipLineCount();
		for (JIndex i = 1; i <= count; i++)
			{
			JIgnoreUntil(is, '\n');
			}
		}

	itsData->ShouldBroadcast(kJFalse);
	itsListenToData = kJFalse;

	const JBoolean hasComments = itsDelimiterDialog->HasComments();
	const JString& commentStr  = itsDelimiterDialog->GetCommentString();

	JLatentPG pg(100);
	pg.VariableLengthProcessBeginning("Loading file...", kJTrue, kJFalse);

	JIndex row = 0;
	JString line, strVal;
	while (is.good())
		{
		line = JReadLine(is);
		if (line.IsEmpty() || (hasComments && line.BeginsWith(commentStr)))
			{
			continue;
			}

		row++;
		std::string s(line.GetCString(), line.GetLength());
		std::istringstream iss(s);

		JIndex col = 1;
		if (type == GXGetDelimiterDialog::kWhiteSpace)
			{
			while (1)
				{
				JFloat value;
				iss >> value;
				if (iss.fail())
					{
					if (col == 1)
						{
						row--;
						}
					break;
					}

				itsData->SetElement(row, col, value);
				col++;
				}
			}
		else
			{
			JBoolean found = kJTrue;
			while (found)
				{
				strVal = JReadUntil(iss, delim, &found);
				JFloat value;
				if (strVal.ConvertToFloat(&value))
					{
					itsData->SetElement(row, col, value);
					}
				col++;
				}
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}

	pg.ProcessFinished();

	itsListenToData = kJTrue;
	itsData->ShouldBroadcast(kJTrue);
}
