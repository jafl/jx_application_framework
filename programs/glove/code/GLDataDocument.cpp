/******************************************************************************
 GLDataDocument.cpp

	BASE CLASS = JXFileDocument

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLDataDocument.h"
#include "GLRaggedFloatTable.h"
#include "GloveModule.h"
#include "GLPlotter.h"
#include "GLPlotApp.h"
#include "GLChooseFileImportDialog.h"
#include "GLGetDelimiterDialog.h"
#include "GLPlotDir.h"
#include "JX2DPlotWidget.h"
#include "J2DPlotData.h"
#include "J2DVectorData.h"
#include "GLRowHeaderWidget.h"
#include "GLColHeaderWidget.h"
#include "GLRaggedFloatTableData.h"
#include "GLGlobals.h"
#include "GLPrefsMgr.h"

#include "filenew.xpm"
#include "fileopen.xpm"
#include "filefloppy.xpm"
#include "fileprint.xpm"
#include "manual.xpm"

#include "JXToolBar.h"

#include <JXDialogDirector.h>
#include <JXDocumentMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXHelpManager.h>
#include <JXPSPrinter.h>
#include <JXImage.h>
#include <JXTextButton.h>
#include <jXActionDefs.h>

#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JLatentPG.h>
#include <JOutPipeStream.h>
#include <JStringIterator.h>

#include <jErrno.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <sstream>
#include <jFileUtil.h>
#include <jAssert.h>

static const JUtf8Byte* kGloveFileSignature = "*** Glove File Format ***";
static const JFileVersion kCurrentGloveVersion = 3;

// version 2:
//	Fixed problem with GLPlotDir::WriteData() (needed a space)
//	Changed GLPlotDir to use PWXRead/WriteSetup()

// File menu information

static const JUtf8Byte* kFileMenuStr =
	"    New %k Meta-N %i New::GLDataDocument"
	"  | Open... %k Meta-O %i Open::GLDataDocument"
	"  | Save %k Meta-S %i Save::GLDataDocument"
	"  | Save as..."
	"%l| Export module"
	"%l| Page setup... "
	"  | Print... %k Meta-P %i " kJXPrintAction
	"%l| Close %k Meta-W %i " kJXCloseWindowAction
	"  | Quit %k Meta-Q %i " kJXQuitAction;

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

static const JUtf8Byte* kExportMenuStr =
	"Reload %l";

enum
{
	kReloadModuleCmd = 1
};

static const JUtf8Byte* kHelpMenuStr =
	"About "
	"%l| Table of Contents %i TOC::GLDataDocument"
	"  | This window %i ThisWindow::GLDataDocument"
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

static const JUtf8Byte* kInternalModuleNames[] =
{
	"Text (delimited)",
	"Text (fixed-width)"
};

const JSize kInternalModuleCount = sizeof(kInternalModuleNames)/sizeof(JUtf8Byte*);

enum
{
	kDelimitedText = 1,
	kFixedWidthText
};

/******************************************************************************
 Constructor

 ******************************************************************************/

GLDataDocument::GLDataDocument
	(
	JXDirector*		supervisor,
	const JString&	fileName,
	const bool	onDisk
	)
	:
	JXFileDocument(supervisor, fileName, onDisk, false, ".glv")
{
	itsData = jnew GLRaggedFloatTableData(0.0);
	assert( itsData != nullptr );

	itsPlotWindows = jnew JPtrArray<GLPlotDir>(JPtrArrayT::kForgetAll);
	assert( itsPlotWindows != nullptr );

	itsPrinter          = nullptr;
	itsFileImportDialog = nullptr;
	itsDelimiterDialog  = nullptr;

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	ListenTo(itsPrinter);

	itsPlotNumber = 1;
	UpdateExportMenu();
	itsListenToData = true;
	ListenTo(itsData);

	if (onDisk)
		{
		LoadFile(fileName);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLDataDocument::~GLDataDocument()
{
	jdelete itsData;
	jdelete itsPrinter;
	jdelete itsPlotWindows;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLDataDocument::BuildWindow()
{
	JSize w = 453;
	JSize h = 360;

	auto* window = jnew JXWindow(this, w,h, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop,
					0,0, w, kJXDefaultMenuBarHeight);
	assert( menuBar != nullptr );

	auto* toolBar =
		jnew JXToolBar(GLGetPrefsMgr(), kDataToolBarID, menuBar,
					window,
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,kJXDefaultMenuBarHeight,
					w,h - kJXDefaultMenuBarHeight);
	assert( toolBar != nullptr );

	window->SetMinSize(150, 150);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	auto* image = jnew JXImage(GetDisplay(), JXPM(filenew));
	assert(image != nullptr);
	itsFileMenu->SetItemImage(kNewCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(fileopen));
	assert(image != nullptr);
	itsFileMenu->SetItemImage(kOpenCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(filefloppy));
	assert(image != nullptr);
	itsFileMenu->SetItemImage(kSaveCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(fileprint));
	assert(image != nullptr);
	itsFileMenu->SetItemImage(kPrintCmd, image, true);

	const JCoordinate scrollheight =
		toolBar->GetWidgetEnclosure()->GetBoundsHeight();

	itsScrollbarSet =
		jnew JXScrollbarSet(toolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,0,
					w, scrollheight);
	assert( itsScrollbarSet != nullptr );

	AdjustWindowTitle();

	// layout table and headers

	const JCoordinate kRowHeaderWidth  = 30;
	const JCoordinate kColHeaderHeight = 20;

	JXContainer* encl = itsScrollbarSet->GetScrollEnclosure();
	JRect enclApG     = encl->GetApertureGlobal();

	auto* okButton =
		jnew JXTextButton(JGetString("OKLabel::JXGlobal"), encl,
						JXWidget::kFixedLeft, JXWidget::kFixedTop,
						0, 0, kRowHeaderWidth-2, kColHeaderHeight-2);
	assert(okButton != nullptr);

	itsTable =
		jnew GLRaggedFloatTable(this, okButton, itsData, 6,
							menuBar, itsScrollbarSet, encl,
							JXWidget::kHElastic, JXWidget::kVElastic,
							kRowHeaderWidth,kColHeaderHeight,
							enclApG.width()  - kRowHeaderWidth,
							enclApG.height() - kColHeaderHeight);
	assert( itsTable != nullptr );

	enclApG = encl->GetApertureGlobal();	// JXScrollableWidget forces a change in this

	auto* rowHeader =
		jnew GLRowHeaderWidget(itsTable, itsScrollbarSet, encl,
							  JXWidget::kFixedLeft, JXWidget::kVElastic,
							  0,kColHeaderHeight, kRowHeaderWidth,
							  enclApG.height() - kColHeaderHeight);
	assert( rowHeader != nullptr );

	auto* colHeader =
		jnew GLColHeaderWidget(itsTable, itsScrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  kRowHeaderWidth,0, enclApG.width() - kRowHeaderWidth,
							  kColHeaderHeight);
	colHeader->TurnOnColResizing(20);
	assert( colHeader != nullptr );

	auto* windowListMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowListMenu != nullptr );
	menuBar->AppendMenu(windowListMenu);

	itsExportMenu = jnew JXTextMenu(itsFileMenu, kExportCmd, menuBar);
	assert( itsExportMenu != nullptr );
	itsExportMenu->SetMenuItems(kExportMenuStr);
	itsExportMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsExportMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	image = jnew JXImage(GetDisplay(), JXPM(manual));
	assert(image != nullptr);
	itsHelpMenu->SetItemImage(kTOCCmd, image, true);

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
GLDataDocument::Receive
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsExportMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleExportMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const auto* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert(info != nullptr);
		if (info->Successful())
			{
			itsTable->PrintRealTable(*itsPrinter);
			}
		}

	else if (sender == itsFileImportDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			LoadImportFile();
			}
		itsFileImportDialog = nullptr;
		}

	else if (sender == itsDelimiterDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			LoadDelimitedFile();
			}
		itsDelimiterDialog = nullptr;
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
GLDataDocument::UpdateFileMenu()
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
GLDataDocument::HandleFileMenu
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
		if (JGetChooseSaveFile()->ChooseFile(JGetString("OpenFilePrompt::GLDataDocument"), JString::empty, &filename))
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
GLDataDocument::LoadFile
	(
	const JString& fileName
	)
{
	std::ifstream is(fileName.GetBytes());

	if (is.bad())
		{
		JGetUserNotification()->ReportError(JGetString("OpenFailed::GLDataDocument"));
		}
	else
		{
		const JString str = JReadLine(is);
		if (str == kGloveFileSignature)
			{
			if (!LoadNativeFile(is))
				{
				JGetUserNotification()->ReportError(JGetString("FileTooNew::GLDataDocument"));
				}
			}
		else
			{
			is.close();
			FileChanged(fileName, false);
			itsCurrentFileName = fileName;
			ChooseFileFilter();
			}
		}
}

/******************************************************************************
 LoadNativeFile (private)

 ******************************************************************************/

bool
GLDataDocument::LoadNativeFile
	(
	std::istream& is
	)
{
	JFloat version;
	is >> version;

	if (version > (JFloat) kCurrentGloveVersion)
		{
		return false;
		}

	itsListenToData = false;
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
	itsListenToData = true;
	DataReverted();
	return true;
}

/******************************************************************************
 ChooseFileFilter (private)

 ******************************************************************************/

void
GLDataDocument::ChooseFileFilter()
{
	assert (itsFileImportDialog == nullptr);
	itsFileImportDialog = jnew GLChooseFileImportDialog(this, itsCurrentFileName);
	assert (itsFileImportDialog != nullptr);
	ListenTo(itsFileImportDialog);
	itsFileImportDialog->BeginDialog();
}

/******************************************************************************
 LoadImportFile (private)

 ******************************************************************************/

void
GLDataDocument::LoadImportFile()
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

		const JUtf8Byte* argv[] = { filter.GetBytes(), itsCurrentFileName.GetBytes(), nullptr };

		int inFD;
		pid_t pid;
		JError err = JExecute(argv, sizeof(argv), &pid,
							  kJIgnoreConnection, nullptr,
							  kJCreatePipe, &inFD,
							  kJIgnoreConnection, nullptr);

		if (!err.OK())
			{
			JGetUserNotification()->ReportError(JGetString("FilterError::GLDataDocument"));
			return;
			}

		std::ifstream ip;
		JString tempName;
		if (JConvertToStream(inFD, &ip, &tempName))
			{
			int type;
			ip >> type;

			if (type == kGloveDataError)
				{
				JGetUserNotification()->ReportError(JGetString("FilterFileError::GLDataDocument"));
				return;
				}

			itsData->ShouldBroadcast(false);
			itsListenToData = false;

			JLatentPG pg(10);
			pg.VariableLengthProcessBeginning(JGetString("FilterProcess::GLDataDocument"), true, false);
			bool keepGoing = true;

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
							keepGoing = false;
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
							keepGoing = false;
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

		itsListenToData = true;
		itsData->ShouldBroadcast(true);
		}
}

/******************************************************************************
 CreateNewPlot

 ******************************************************************************/

void
GLDataDocument::CreateNewPlot
	(
	const JIndex			type,
	const JArray<JFloat>& 	xCol,
	const JArray<JFloat>* 	x2Col,
	const JArray<JFloat>& 	yCol,
	const JArray<JFloat>* 	y2Col,
	const bool			linked,
	const JString&			label
	)
{
	JString str((JUInt64) itsPlotNumber);
	const JUtf8Byte* map[] =
		{
		"i", str.GetBytes()
		};
	str = JGetString("PlotTitle::GLDataDocument", map, sizeof(map));
	itsPlotNumber++;

	auto* plotDir = jnew GLPlotDir(this, this, GetFileName());
	assert (plotDir != nullptr);
	itsPlotWindows->Append(plotDir);

	JX2DPlotWidget* plot = plotDir->GetPlot();
	assert( plot != nullptr );
	ListenTo(plot);

	AddToPlot(itsPlotWindows->GetElementCount(), type, xCol, x2Col, yCol, y2Col, linked, label);

	plot->SetTitle(str);
}


/******************************************************************************
 AddToPlot

 ******************************************************************************/

void
GLDataDocument::AddToPlot
	(
	const JIndex			plotIndex,
	const JIndex			type,
	const JArray<JFloat>&	xCol,
	const JArray<JFloat>*	x2Col,
	const JArray<JFloat>&	yCol,
	const JArray<JFloat>*	y2Col,
	const bool			linked,
	const JString&			label
	)
{
	assert( itsPlotWindows->IndexValid(plotIndex) );

	GLPlotDir* plotDir = itsPlotWindows->GetElement(plotIndex);
	assert (plotDir != nullptr);

	JX2DPlotWidget* plot = plotDir->GetPlot();
	assert( plot != nullptr );
	ListenTo(plot);

	if (type == kDataPlot)
		{
		J2DPlotData* curve;
		if (J2DPlotData::Create(&curve, xCol, yCol, linked))
			{
			plot->AddCurve(curve, true, label);
			}
		else
			{
			JGetUserNotification()->ReportError(JGetString("MismatchedColumns::GLDataDocument"));
			return;
			}
		if (x2Col != nullptr)
			{
			curve->SetXErrors(*x2Col);
			}
		if (y2Col != nullptr)
			{
			curve->SetYErrors(*y2Col);
			}
		}
	else if (type == kVectorPlot)
		{
		J2DVectorData* curve;
		if (J2DVectorData::Create(&curve, xCol, yCol, *x2Col, *y2Col, linked))
			{
			plot->AddCurve(curve, true, label);
			}
		else
			{
			JGetUserNotification()->ReportError(JGetString("MismatchedColumns::GLDataDocument"));
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
GLDataDocument::GetPlotNames
	(
	JPtrArray<JString>& names
	)
{
	JString* name;

	const JIndex index = itsPlotWindows->GetElementCount();

	for (JIndex i = 1; i <= index; i++)
		{
		GLPlotDir* plotDir = itsPlotWindows->GetElement(i);
		assert (plotDir != nullptr);

		JX2DPlotWidget* plot = plotDir->GetPlot();
		assert( plot != nullptr );
		name = jnew JString(plot->GetTitle());
		if (name->GetCharacterCount() > kMaxPlotTitleSize)
			{
			JStringIterator iter(name, kJIteratorStartAfter, kMaxPlotTitleSize);
			iter.RemoveAllNext();
			name->Append(JGetString("Ellipsis::GLDataDocument"));
			}
		names.Append(name);
		}
}

/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void
GLDataDocument::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex index;
	const auto* dir = (const GLPlotDir*) theDirector;
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
GLDataDocument::WriteTextFile
	(
	std::ostream& output,
	const bool safetySave
	)
	const
{
	output << kGloveFileSignature << std::endl;
	output << kCurrentGloveVersion << std::endl;
	itsTable->WriteData(output);

	JSize plotCount = itsPlotWindows->GetElementCount();
	output << plotCount << " ";

	for (JSize i = 1; i <= plotCount; i++)
		{
		GLPlotDir* plotDir = itsPlotWindows->GetElement(i);
		assert (plotDir != nullptr);
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
GLDataDocument::ReadPlotData
	(
	std::istream& 		is,
	const JFloat	gloveVersion
	)
{
	JSize count;
	is >> count;
	for (JSize i = 1; i <= count; i++)
		{
		auto* plotDir = jnew GLPlotDir(this, this, GetFileName());
		assert (plotDir != nullptr);

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
GLDataDocument::HandleExportMenu
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
		JString filename;
		if (JGetChooseSaveFile()->SaveFile(
				JGetString("ExportFileName::GLDataDocument"),
				JGetString("ExportPrompt::GLDataDocument"),
				JString::empty, &filename))
			{
			const JUtf8Byte* argv[] = { modName.GetBytes(), filename.GetBytes(), nullptr };

			int inFD, outFD;
			pid_t pid;
			JError err = JExecute(argv, sizeof(argv), &pid,
								  kJCreatePipe, &outFD,
								  kJCreatePipe, &inFD,
								  kJIgnoreConnection, nullptr);

			if (!err.OK())
				{
				JGetUserNotification()->ReportError(JGetString("ModuleError::GLDataDocument"));
				return;
				}

			std::ifstream ip;
			JString tempName;
			if (JConvertToStream(inFD, &ip, &tempName))
				{
				JOutPipeStream op(outFD, true);
				assert( op.good() );

				int type;
				ip >> type;

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
GLDataDocument::UpdateExportMenu()
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
		itsExportMenu->AppendItem(*(names->GetElement(i)));
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
GLDataDocument::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("GLTableHelp");
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
 DiscardChanges

 ******************************************************************************/

void
GLDataDocument::DiscardChanges()
{

}

/******************************************************************************
 GetInternalModuleName (public)

 ******************************************************************************/

JString
GLDataDocument::GetInternalModuleName
	(
	const JIndex index
	)
	const
{
	assert(index <= kInternalModuleCount);
	assert(index > 0);
	return JString(kInternalModuleNames[index - 1], JString::kNoCopy);
}

/******************************************************************************
 GetInternalModuleCount (public)

 ******************************************************************************/

JSize
GLDataDocument::GetInternalModuleCount()
{
	return kInternalModuleCount;
}

/******************************************************************************
 LoadInternalFile (private)

 ******************************************************************************/

void
GLDataDocument::LoadInternalFile
	(
	const JIndex index
	)
{
	if (index == kDelimitedText)
		{
		assert(itsDelimiterDialog == nullptr);
		itsDelimiterDialog =
			jnew GLGetDelimiterDialog(this, itsFileImportDialog->GetFileText());
		assert(itsDelimiterDialog != nullptr);
		ListenTo(itsDelimiterDialog);
		itsDelimiterDialog->BeginDialog();
		}
	else if (index == kFixedWidthText)
		{
		JGetUserNotification()->ReportError(JGetString("LoadFixedWidthFile::GLDataDocument"));
		}
}

/******************************************************************************
 LoadDelimitedFile (private)

 ******************************************************************************/

void
GLDataDocument::LoadDelimitedFile()
{
	assert(itsDelimiterDialog != nullptr);

	const GLGetDelimiterDialog::DelimiterType type = itsDelimiterDialog->GetDelimiterType();
	JUtf8Byte delim;
	if (type == GLGetDelimiterDialog::kChar)
		{
		delim = itsDelimiterDialog->GetCharacter();
		}
	else if (type == GLGetDelimiterDialog::kSpace)
		{
		delim = ' ';
		}
	else if (type == GLGetDelimiterDialog::kTab)
		{
		delim = '\t';
		}

	std::ifstream is(itsCurrentFileName.GetBytes());
	if (is.bad())
		{
		JGetUserNotification()->ReportError(JGetString("OpenFailed::GLDataDocument"));
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

	itsData->ShouldBroadcast(false);
	itsListenToData = false;

	const bool hasComments = itsDelimiterDialog->HasComments();
	const JString& commentStr  = itsDelimiterDialog->GetCommentString();

	JLatentPG pg(100);
	pg.VariableLengthProcessBeginning(JGetString("FilterProcess::GLDataDocument"), true, false);

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
		std::string s(line.GetRawBytes(), line.GetByteCount());
		std::istringstream iss(s);

		JIndex col = 1;
		if (type == GLGetDelimiterDialog::kWhiteSpace)
			{
			while (true)
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
			bool found = true;
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

	itsListenToData = true;
	itsData->ShouldBroadcast(true);
}
