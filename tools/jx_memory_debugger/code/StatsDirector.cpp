/******************************************************************************
 StatsDirector.cpp

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "StatsDirector.h"
#include "SizeHistogram.h"
#include "FilterRecordsDialog.h"
#include "RecordDirector.h"
#include "RecordList.h"
#include "Record.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXFileInput.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXPGMessageDirector.h>
#include <jx-af/jcore/JMemoryManager.h>
#include <ace/Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

const JSize kRefreshInterval = 1000;	// 1 second (ms)

// File menu

static const JUtf8Byte* kFileMenuStr =
	"Quit %k Meta-Q %i" kJXQuitAction;

enum
{
	kQuitCmd = 1
};

// Data menu

static const JUtf8Byte* kDataMenuStr =
	"    Get allocated records       %i" kGetRecordsAction
	"%l| Show application records %b %i" kShowAppRecordsAction
	"  | Show bucket #1 records   %b %i" kShowBucket1RecordsAction
	"  | Show bucket #2 records   %b %i" kShowBucket2RecordsAction
	"  | Show bucket #3 records   %b %i" kShowBucket3RecordsAction
	"%l| Show library records     %b %i" kShowLibraryRecordsAction
	"%l| Show internal records    %b %i" kShowInternalRecordsAction
	"  | Show unknown records     %b %i" kShowUnknownRecordsAction;

enum
{
	kGetRecordsCmd = 1,
	kShowAppRecordsCmd,
	kShowBucket1RecordsCmd,
	kShowBucket2RecordsCmd,
	kShowBucket3RecordsCmd,
	kShowLibraryRecordsCmd,
	kShowIntervalRecordsCmd,
	kShowUnknownRecordsCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit preferences..."
	"  | Edit tool bar..."
	"  | Mac/Win/X emulation..."
	"%l| Save window setup as default";

enum
{
	kPrefsCmd = 1,
	kEditToolBarCmd,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

// preferences

const JFileVersion kCurrentPrefsVersion	= 0;

// socket

class DebugLink : public JMemoryManager::DebugLink
{
public:

	int open(void* data) override;
};

class LinkAcceptor : public ACE_Acceptor<DebugLink, ACE_LSOCK_ACCEPTOR>
{
public:

	LinkAcceptor(StatsDirector* dir)
		:
		itsDirector(dir)
	{ };

	~LinkAcceptor() override
	{
		close();
		(acceptor()).remove();
	};

	StatsDirector* GetDirector()
	{
		return itsDirector;
	};

private:

	StatsDirector*	itsDirector;	// not owned
};

int DebugLink::open(void* data)
{
	((LinkAcceptor*) data)->GetDirector()->SetLink(this);
	return JMemoryManager::DebugLink::open(data);
};

/******************************************************************************
 Constructor

 *****************************************************************************/

StatsDirector::StatsDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kStatsDirectorID),
	itsAcceptor(nullptr),
	itsLink(nullptr),
	itsProcess(nullptr),
	itsPingTask(nullptr),
	itsMessageDir(nullptr)
{
	itsDataFilter.includeApp     = true;
	itsDataFilter.includeBucket1 = true;
	itsDataFilter.includeBucket2 = true;
	itsDataFilter.includeBucket3 = true;

	BuildWindow();

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kStatsDirectorWindSizeID,
										 &desktopLoc, &w, &h))
	{
		JXWindow* window = GetWindow();
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

StatsDirector::~StatsDirector()
{
	JPrefObject::WritePrefs();

	CloseLink();
	DeleteDebugAcceptor();
}

/******************************************************************************
 SetLink

 *****************************************************************************/

void
StatsDirector::SetLink
	(
	JMemoryManager::DebugLink* link
	)
{
	assert( itsLink == nullptr );
	itsLink = link;
	JMemoryManager::SetProtocol(itsLink);
	ListenTo(itsLink);
	DeleteDebugAcceptor();

	itsPingTask = jnew JXFunctionTask(kRefreshInterval, std::bind(&StatsDirector::RequestRunningStats, this));
	assert( itsPingTask != nullptr );
	itsPingTask->Start();
}

/******************************************************************************
 CloseLink (private)

 *****************************************************************************/

void
StatsDirector::CloseLink()
{
	jdelete itsLink;
	itsLink = nullptr;

	jdelete itsProcess;
	itsProcess = nullptr;

	jdelete itsPingTask;
	itsPingTask = nullptr;

	FinishProgram();
}

/******************************************************************************
 OpenDebugAcceptor

 ******************************************************************************/

void
StatsDirector::OpenDebugAcceptor()
{
	const JError err = JCreateTempFile(&itsSocketName);

	itsAcceptor = jnew LinkAcceptor(this);
	assert( itsAcceptor != nullptr );

	JRemoveFile(itsSocketName);
	ACE_UNIX_Addr addr(itsSocketName.GetBytes());
	if (itsAcceptor->open(addr) == -1)
	{
		std::cerr << "error trying to create socket: " << jerrno() << std::endl;
		exit(1);
	}
}

/******************************************************************************
 DeleteDebugAcceptor (private)

 ******************************************************************************/

void
StatsDirector::DeleteDebugAcceptor()
{
	if (itsAcceptor != nullptr)
	{
		jdelete itsAcceptor;
		itsAcceptor = nullptr;
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "md_main_window_icon.xpm"
#include <jx-af/image/jx/jx_help_specific.xpm>
#include <jx-af/image/jx/jx_help_toc.xpm>

void
StatsDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kStatsToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::StatsDirector"));
	window->SetWMClass(GetWMClassInstance(), GetMainWindowClass());
	window->SetMinSize(200, 200);

	auto* image = jnew JXImage(GetDisplay(), md_main_window_icon);
	assert( image != nullptr );
	window->SetIcon(image);

	JXWidgetSet* statsEncl = itsToolBar->GetWidgetEnclosure();

// begin statsLayout

	const JRect statsLayout_Frame    = statsEncl->GetFrame();
	const JRect statsLayout_Aperture = statsEncl->GetAperture();
	statsEncl->AdjustSize(500 - statsLayout_Aperture.width(), 90 - statsLayout_Aperture.height());

	auto* binaryLabel =
		jnew JXStaticText(JGetString("binaryLabel::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 50,20);
	assert( binaryLabel != nullptr );
	binaryLabel->SetToLabel();

	itsProgramInput =
		jnew JXFileInput(statsEncl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 370,20);
	assert( itsProgramInput != nullptr );

	itsChooseProgramButton =
		jnew JXTextButton(JGetString("itsChooseProgramButton::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 60,20);
	assert( itsChooseProgramButton != nullptr );
	itsChooseProgramButton->SetShortcuts(JGetString("itsChooseProgramButton::StatsDirector::shortcuts::statsLayout"));

	auto* argsLabel =
		jnew JXStaticText(JGetString("argsLabel::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 50,20);
	assert( argsLabel != nullptr );
	argsLabel->SetToLabel();

	itsArgsInput =
		jnew JXInputField(statsEncl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,30, 370,20);
	assert( itsArgsInput != nullptr );

	itsRunProgramButton =
		jnew JXTextButton(JGetString("itsRunProgramButton::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,30, 60,20);
	assert( itsRunProgramButton != nullptr );
	itsRunProgramButton->SetShortcuts(JGetString("itsRunProgramButton::StatsDirector::shortcuts::statsLayout"));

	auto* blocksLabel =
		jnew JXStaticText(JGetString("blocksLabel::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 50,20);
	assert( blocksLabel != nullptr );
	blocksLabel->SetToLabel();

	itsAllocatedBlocksDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,60, 90,20);
	assert( itsAllocatedBlocksDisplay != nullptr );

	auto* bytesLabel =
		jnew JXStaticText(JGetString("bytesLabel::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 50,20);
	assert( bytesLabel != nullptr );
	bytesLabel->SetToLabel();

	itsAllocatedBytesDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,60, 90,20);
	assert( itsAllocatedBytesDisplay != nullptr );

	auto* deallocLabel =
		jnew JXStaticText(JGetString("deallocLabel::StatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,60, 80,20);
	assert( deallocLabel != nullptr );
	deallocLabel->SetToLabel();

	itsDeallocatedBlocksDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 390,60, 90,20);
	assert( itsDeallocatedBlocksDisplay != nullptr );

	statsEncl->SetSize(statsLayout_Frame.width(), statsLayout_Frame.height());

// end statsLayout

	itsAllocatedBlocksDisplay->SetToLabel();
	itsAllocatedBytesDisplay->SetToLabel();
	itsDeallocatedBlocksDisplay->SetToLabel();

	const JCoordinate headerHeight = itsAllocatedBlocksDisplay->GetFrame().bottom + 10;
	const JCoordinate histoHeight  = statsEncl->GetBoundsHeight() - headerHeight;

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,headerHeight, 100,histoHeight);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure(true, false);

	itsAllocatedHisto =
		jnew SizeHistogram(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic, 0,0, 100,100);
	assert( itsAllocatedHisto != nullptr );
	itsAllocatedHisto->FitToEnclosure();

	ListenTo(itsProgramInput->GetText(), std::function([this](const JStyledText::TextSet&)
	{
		UpdateDisplay();
	}));

	ListenTo(itsProgramInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		UpdateDisplay();
	}));

	ListenTo(itsChooseProgramButton, std::function([this](const JXButton::Pushed&)
	{
		ChooseProgram();
	}));

	ListenTo(itsRunProgramButton, std::function([this](const JXButton::Pushed&)
	{
		RunProgram();
	}));

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "StatsDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&StatsDirector::UpdateFileMenu,
		&StatsDirector::HandleFileMenu);

	itsDataMenu = menuBar->AppendTextMenu(JGetString("DataMenuTitle::StatsDirector"));
	itsDataMenu->SetMenuItems(kDataMenuStr, "StatsDirector");
	itsDataMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDataMenu->AttachHandlers(this,
		&StatsDirector::UpdateDataMenu,
		&StatsDirector::HandleDataMenu);

	itsProgramInput->AppendEditMenu(menuBar);
	itsArgsInput->ShareEditMenu(itsProgramInput);
	itsAllocatedBlocksDisplay->ShareEditMenu(itsProgramInput);
	itsAllocatedBytesDisplay->ShareEditMenu(itsProgramInput);
	itsDeallocatedBlocksDisplay->ShareEditMenu(itsProgramInput);

	itsProgramInput->ShouldRequireWritable(false);
	itsProgramInput->ShouldRequireExecutable(true);

	auto* windowsMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != nullptr );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "StatsDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &StatsDirector::HandlePrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "StatsDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsHelpMenu->AttachHandler(this, &StatsDirector::HandleHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	// must be done after creating widgets

	JPrefObject::ReadPrefs();

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kQuitCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
	}

	GetDisplay()->GetWDManager()->DirectorCreated(this);
	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
StatsDirector::UpdateDisplay()
{
	itsRunProgramButton->SetActive(!itsProgramInput->GetText()->IsEmpty());
}

/******************************************************************************
 ChooseProgram (private)

 ******************************************************************************/

void
StatsDirector::ChooseProgram()
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, itsProgramInput->GetText()->GetText());
	if (dlog->DoDialog())
	{
		itsProgramInput->GetText()->SetText(dlog->GetFullName());
	}
}

/******************************************************************************
 RunProgram (private)

 ******************************************************************************/

void
StatsDirector::RunProgram()
{
	JString fullName;
	if (!itsProgramInput->GetFile(&fullName))
	{
		return;
	}

	JString path, name;
	if (JSplitPathAndName(fullName, &path, &name))
	{
		JChangeDirectory(path);
	}

	OpenDebugAcceptor();

	setenv("JMM_PIPE",             itsSocketName.GetBytes(), 1);
	setenv("JMM_RECORD_ALLOCATED", "yes",                    1);
	setenv("JMM_BROADCAST_ERRORS", "yes",                    1);
	setenv("JMM_NO_PRINT_ERRORS",  "yes",                    1);
	setenv("MALLOC_CHECK_",        "2",                      1);

	JUtf8Byte* v = getenv("JMM_INITIALIZE");
	if (v == nullptr || JString::Compare(v, "no", JString::kIgnoreCase) == 0)
	{
		setenv("JMM_INITIALIZE", "default", 1);
	}

	v = getenv("JMM_SHRED");
	if (v == nullptr || JString::Compare(v, "no", JString::kIgnoreCase) == 0)
	{
		setenv("JMM_SHRED", "default", 1);
	}

	JString cmd = fullName;
	cmd        += " ";
	cmd        += itsArgsInput->GetText()->GetText();

	const JError err = JProcess::Create(&itsProcess, cmd);
	if (err.OK())
	{
		// remember to update FinishProgram()

		itsChooseProgramButton->Hide();
		itsRunProgramButton->Hide();
		itsProgramInput->Deactivate();
		itsArgsInput->Deactivate();
		itsAllocatedBlocksDisplay->SetBackColor(itsAllocatedBlocksDisplay->GetFocusColor());
		itsAllocatedBlocksDisplay->GetText()->SetText(JString::empty);
		itsAllocatedBytesDisplay->SetBackColor(itsAllocatedBytesDisplay->GetFocusColor());
		itsAllocatedBytesDisplay->GetText()->SetText(JString::empty);
		itsDeallocatedBlocksDisplay->SetBackColor(itsDeallocatedBlocksDisplay->GetFocusColor());
		itsDeallocatedBlocksDisplay->GetText()->SetText(JString::empty);
		ListenTo(itsProcess);
	}
	else
	{
		err.ReportIfError();
	}

	unsetenv("JMM_PIPE");
	unsetenv("JMM_RECORD_ALLOCATED");
	unsetenv("JMM_INITIALIZE");
	unsetenv("JMM_SHRED");
	unsetenv("JMM_BROADCAST_ERRORS");
	unsetenv("JMM_NO_PRINT_ERRORS");
	unsetenv("MALLOC_CHECK_");
}

/******************************************************************************
 FinishProgram (private)

 ******************************************************************************/

void
StatsDirector::FinishProgram()
{
	// remember to update RunProgram()

	itsChooseProgramButton->Show();
	itsRunProgramButton->Show();
	itsProgramInput->Activate();
	itsProgramInput->Focus();
	itsArgsInput->Activate();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StatsDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		HandleResponse();
	}
	else if (sender == itsLink && message.Is(JMessageProtocolT::kReceivedDisconnect))
	{
		CloseLink();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 SendRequest (private)

 ******************************************************************************/

void
StatsDirector::SendRequest
	(
	std::ostringstream& data
	)
{
	std::string s = data.str();
	itsLink->SendMessage(JString(s));
}

/******************************************************************************
 HandleResponse (private)

 ******************************************************************************/

void
StatsDirector::HandleResponse()
{
	assert( itsLink != nullptr );

	JString text;
	const bool ok = itsLink->GetNextMessage(&text);
	assert( ok );

	std::string s(text.GetBytes(), text.GetByteCount());
	std::istringstream input(s);

	JFileVersion vers;
	input >> vers;
	if (vers != kJMemoryManagerDebugVersion)
	{
		std::cerr << "StatsDirector::HandleResponse received version (" << vers;
		std::cerr << ") different than expected (" << kJMemoryManagerDebugVersion << ")" << std::endl;
		return;
	}

	long type;
	input >> type;

	if (type == JMemoryManager::kRunningStatsMessage)
	{
		ReceiveRunningStats(input);
	}
	else if (type == JMemoryManager::kExitStatsMessage)
	{
		ReceiveExitStats(input);
	}
	else if (type == JMemoryManager::kRecordsMessage)
	{
		ReceiveRecords(input, JGetString("QueryRecordsWindowTitle::StatsDirector"), true);
	}
	else if (type == JMemoryManager::kErrorMessage)
	{
		ReceiveErrorMessage(input);
	}
}

/******************************************************************************
 RequestRunningStats (private)

 ******************************************************************************/

void
StatsDirector::RequestRunningStats()
{
	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ' << JMemoryManager::kRunningStatsMessage;
	output << ' ';
	itsDataFilter.Write(output);

	SendRequest(output);
}

/******************************************************************************
 ReceiveRunningStats (private)

 ******************************************************************************/

inline void
mdSetValue
	(
	JXStaticText*	field,
	const JSize		value,
	const bool	raw = true
	)
{
	const bool hadSelection = field->HasSelection();
	JString s;
	if (raw)
	{
		s = JString((JUInt64) value);
	}
	else
	{
		s = JPrintFileSize(value);
	}
	field->GetText()->SetText(s);
	if (hadSelection)
	{
		field->SelectAll();
	}
}

void
StatsDirector::ReceiveRunningStats
	(
	std::istream& input
	)
{
	JSize total;
	input >> total;
	mdSetValue(itsAllocatedBlocksDisplay, total);

	JSize value;
	input >> value;
	mdSetValue(itsAllocatedBytesDisplay, value, false);

	input >> value;
	mdSetValue(itsDeallocatedBlocksDisplay, value);

	JSize count;
	input >> count;
	assert( count == JMemoryManager::kHistogramSlotCount );

	JSize histo[ JMemoryManager::kHistogramSlotCount ];
	for (JUnsignedOffset i=0; i<count; i++)
	{
		input >> histo[i];
	}

	itsAllocatedHisto->SetValues(total, histo);
}

/******************************************************************************
 ReceiveExitStats (private)

 ******************************************************************************/

void
StatsDirector::ReceiveExitStats
	(
	std::istream& input
	)
{
	JString fileName;
	input >> fileName;

	std::ifstream input2(fileName.GetBytes());

	while (true)
	{
		long type;
		input2 >> type;
		if (input2.eof() || !input2.good())
		{
			break;
		}
		else if (type == JMemoryManager::kErrorMessage)
		{
			ReceiveErrorMessage(input2);
		}
		else if (type == JMemoryManager::kRunningStatsMessage)
		{
			ReceiveRunningStats(input2);
		}
		else if (type == JMemoryManager::kRecordsMessage)
		{
			ReceiveRecords(input2, JGetString("ExitRecordsWindowTitle::StatsDirector"), false);
			GetWindow()->SetTitle(JGetString("ExitWindowTitle::StatsDirector"));
		}
		else
		{
			break;
		}
	}

	input2.close();
	JRemoveFile(fileName);

	if (itsMessageDir != nullptr)
	{
		itsMessageDir->ProcessFinished();
	}
}

/******************************************************************************
 RequestRecords (private)

 ******************************************************************************/

void
StatsDirector::RequestRecords()
{
	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ' << JMemoryManager::kRecordsMessage;
	output << ' ';
	itsDataFilter.Write(output);

	SendRequest(output);
}

/******************************************************************************
 ReceiveRecords (private)

 ******************************************************************************/

void
StatsDirector::ReceiveRecords
	(
	std::istream&	input,
	const JString&	windowTitle,
	const bool		showIfEmpty
	)
{
	auto* list = jnew RecordList;
	assert( list != nullptr );

	while (true)
	{
		bool keepGoing;
		input >> JBoolFromString(keepGoing);
		if (!keepGoing)
		{
			break;
		}

		auto* record = jnew Record(input);
		assert( record != nullptr );
		list->AddRecord(record);
	}

	if (!list->IsEmpty() || showIfEmpty)
	{
		auto* dir = jnew RecordDirector(this, list, windowTitle);
		assert( dir != nullptr );
		dir->Activate();
	}
}

/******************************************************************************
 ReceiveErrorMessage (private)

 ******************************************************************************/

void
StatsDirector::ReceiveErrorMessage
	(
	std::istream& input
	)
{
	JString msg;
	input >> msg;

	if (itsMessageDir == nullptr)
	{
		itsMessageDir = jnew JXPGMessageDirector(this);
		assert( itsMessageDir != nullptr );
		itsMessageDir->Activate();
	}

	itsMessageDir->AddMessageLine(msg);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
StatsDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
StatsDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateDataMenu (private)

 ******************************************************************************/

void
StatsDirector::UpdateDataMenu()
{
	itsDataMenu->SetItemEnabled(kGetRecordsCmd, itsProcess != nullptr);

	if (itsDataFilter.includeApp)
	{
		itsDataMenu->CheckItem(kShowAppRecordsCmd);
	}

	if (itsDataFilter.includeBucket1)
	{
		itsDataMenu->CheckItem(kShowBucket1RecordsCmd);
	}

	if (itsDataFilter.includeBucket2)
	{
		itsDataMenu->CheckItem(kShowBucket2RecordsCmd);
	}

	if (itsDataFilter.includeBucket3)
	{
		itsDataMenu->CheckItem(kShowBucket3RecordsCmd);
	}

	if (itsDataFilter.includeLibrary)
	{
		itsDataMenu->CheckItem(kShowLibraryRecordsCmd);
	}

	if (itsDataFilter.includeInternal)
	{
		itsDataMenu->CheckItem(kShowIntervalRecordsCmd);
	}

	if (itsDataFilter.includeUnknown)
	{
		itsDataMenu->CheckItem(kShowUnknownRecordsCmd);
	}
}

/******************************************************************************
 HandleDataMenu (private)

 ******************************************************************************/

void
StatsDirector::HandleDataMenu
	(
	const JIndex index
	)
{
	if (index == kGetRecordsCmd)
	{
		auto* dlog = jnew FilterRecordsDialog(itsDataFilter);
		assert( dlog != nullptr );
		if (dlog->DoDialog())
		{
			dlog->BuildFilter(&itsDataFilter);
			RequestRecords();
		}
	}

	else if (index == kShowAppRecordsCmd)
	{
		itsDataFilter.includeApp = !itsDataFilter.includeApp;
	}

	else if (index == kShowBucket1RecordsCmd)
	{
		itsDataFilter.includeBucket1 = !itsDataFilter.includeBucket1;
	}

	else if (index == kShowBucket2RecordsCmd)
	{
		itsDataFilter.includeBucket2 = !itsDataFilter.includeBucket2;
	}

	else if (index == kShowBucket3RecordsCmd)
	{
		itsDataFilter.includeBucket3 = !itsDataFilter.includeBucket3;
	}

	else if (index == kShowLibraryRecordsCmd)
	{
		itsDataFilter.includeLibrary = !itsDataFilter.includeLibrary;
	}

	else if (index == kShowIntervalRecordsCmd)
	{
		itsDataFilter.includeInternal = !itsDataFilter.includeInternal;
	}

	else if (index == kShowUnknownRecordsCmd)
	{
		itsDataFilter.includeUnknown = !itsDataFilter.includeUnknown;
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
StatsDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
	{
		GetPrefsManager()->EditPrefs();
	}
	else if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		GetPrefsManager()->SaveWindowSize(kStatsDirectorWindSizeID, GetWindow());
	}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
StatsDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		GetApplication()->DisplayAbout();
	}

	else if (index == kTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kThisWindowCmd)
	{
		JXGetHelpManager()->ShowSection("MainHelp");
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
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
StatsDirector::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
	{
		return;
	}

	GetWindow()->ReadGeometry(input);

	JString s;
	input >> s;
	itsProgramInput->GetText()->SetText(s);

	input >> s;
	itsArgsInput->GetText()->SetText(s);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
StatsDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsProgramInput->GetText()->GetText();
	output << ' ' << itsArgsInput->GetText()->GetText();
}
