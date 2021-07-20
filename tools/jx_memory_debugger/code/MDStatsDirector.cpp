/******************************************************************************
 MDStatsDirector.cpp

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDStatsDirector.h"
#include "MDSizeHistogram.h"
#include "MDFilterRecordsDialog.h"
#include "MDRecordDirector.h"
#include "MDRecordList.h"
#include "MDRecord.h"
#include "mdGlobals.h"
#include "mdActionDefs.h"
#include <JXMacWinPrefsDialog.h>
#include <JXHelpManager.h>
#include <JXWDManager.h>
#include <JXWDMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXFileInput.h>
#include <JXColorManager.h>
#include <JXImage.h>
#include <JXChooseSaveFile.h>
#include <JXTimerTask.h>
#include <JXPGMessageDirector.h>
#include <JMemoryManager.h>
#include <ace/Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <JProcess.h>
#include <jFileUtil.h>
#include <jErrno.h>
#include <jAssert.h>

const JSize kRefreshInterval = 1;		// seconds

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Get allocated records           %i" kMDGetRecords
	"%l| Quit                  %k Meta-Q %i" kJXQuitAction;

enum
{
	kGetRecordsCmd = 1,
	kQuitCmd
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

	virtual int open(void* data);
};

class MDLinkAcceptor : public ACE_Acceptor<DebugLink, ACE_LSOCK_ACCEPTOR>
{
public:

	MDLinkAcceptor(MDStatsDirector* dir)
		:
		itsDirector(dir)
		{ };

	virtual ~MDLinkAcceptor()
	{
		close();
		(acceptor()).remove();
	};

	MDStatsDirector* GetDirector()
	{
		return itsDirector;
	};

private:

	MDStatsDirector*	itsDirector;	// not owned
};

int DebugLink::open(void* data)
{
	(((MDLinkAcceptor*) data)->GetDirector())->SetLink(this);
	return JMemoryManager::DebugLink::open(data);
};

/******************************************************************************
 Constructor

 *****************************************************************************/

MDStatsDirector::MDStatsDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(MDGetPrefsManager(), kMDStatsDirectorID),
	itsAcceptor(nullptr),
	itsLink(nullptr),
	itsProcess(nullptr),
	itsPingTask(nullptr),
	itsMessageDir(nullptr),
	itsRequestRecordsDialog(nullptr)
{
	BuildWindow();

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((MDGetPrefsManager())->GetWindowSize(kMDStatsDirectorWindSizeID,
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

MDStatsDirector::~MDStatsDirector()
{
	JPrefObject::WritePrefs();

	CloseLink(true);
	DeleteDebugAcceptor();
}

/******************************************************************************
 SetLink

 *****************************************************************************/

void
MDStatsDirector::SetLink
	(
	JMemoryManager::DebugLink* link
	)
{
	assert( itsLink == nullptr );
	itsLink = link;
	JMemoryManager::SetProtocol(itsLink);
	ListenTo(itsLink);

	DeleteDebugAcceptor();

	itsPingTask = jnew JXTimerTask(kRefreshInterval * 1000);
	assert( itsPingTask != nullptr );
	itsPingTask->Start();
	ListenTo(itsPingTask);
}

/******************************************************************************
 CloseLink (private)

 *****************************************************************************/

void
MDStatsDirector::CloseLink
	(
	const bool deleteProcess
	)
{
	jdelete itsLink;
	itsLink = nullptr;

	if (deleteProcess)
		{
		jdelete itsProcess;
		itsProcess = nullptr;
		}

	jdelete itsPingTask;
	itsPingTask = nullptr;
}

/******************************************************************************
 OpenDebugAcceptor

 ******************************************************************************/

void
MDStatsDirector::OpenDebugAcceptor()
{
	const JError err = JCreateTempFile(&itsSocketName);

	itsAcceptor = jnew MDLinkAcceptor(this);
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
MDStatsDirector::DeleteDebugAcceptor()
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
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
MDStatsDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(MDGetPrefsManager(), kMDStatsToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDStatsDirector"));
	window->SetWMClass(MDGetWMClassInstance(), MDGetMainWindowClass());
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
		jnew JXStaticText(JGetString("binaryLabel::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 50,20);
	assert( binaryLabel != nullptr );
	binaryLabel->SetToLabel();

	itsProgramInput =
		jnew JXFileInput(statsEncl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 370,20);
	assert( itsProgramInput != nullptr );

	itsChooseProgramButton =
		jnew JXTextButton(JGetString("itsChooseProgramButton::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 60,20);
	assert( itsChooseProgramButton != nullptr );
	itsChooseProgramButton->SetShortcuts(JGetString("itsChooseProgramButton::MDStatsDirector::shortcuts::statsLayout"));

	auto* argsLabel =
		jnew JXStaticText(JGetString("argsLabel::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 50,20);
	assert( argsLabel != nullptr );
	argsLabel->SetToLabel();

	itsArgsInput =
		jnew JXInputField(statsEncl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 60,30, 370,20);
	assert( itsArgsInput != nullptr );

	itsRunProgramButton =
		jnew JXTextButton(JGetString("itsRunProgramButton::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 430,30, 60,20);
	assert( itsRunProgramButton != nullptr );
	itsRunProgramButton->SetShortcuts(JGetString("itsRunProgramButton::MDStatsDirector::shortcuts::statsLayout"));

	auto* blocksLabel =
		jnew JXStaticText(JGetString("blocksLabel::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 50,20);
	assert( blocksLabel != nullptr );
	blocksLabel->SetToLabel();

	itsAllocatedBlocksDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,60, 90,20);
	assert( itsAllocatedBlocksDisplay != nullptr );

	auto* bytesLabel =
		jnew JXStaticText(JGetString("bytesLabel::MDStatsDirector::statsLayout"), statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 50,20);
	assert( bytesLabel != nullptr );
	bytesLabel->SetToLabel();

	itsAllocatedBytesDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, statsEncl,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,60, 90,20);
	assert( itsAllocatedBytesDisplay != nullptr );

	auto* deallocLabel =
		jnew JXStaticText(JGetString("deallocLabel::MDStatsDirector::statsLayout"), statsEncl,
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
		jnew MDSizeHistogram(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic, 0,0, 100,100);
	assert( itsAllocatedHisto != nullptr );
	itsAllocatedHisto->FitToEnclosure();

	ListenTo(itsProgramInput);

	ListenTo(itsChooseProgramButton);
	ListenTo(itsRunProgramButton);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "MDStatsDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsProgramInput->AppendEditMenu(menuBar);
	itsArgsInput->ShareEditMenu(itsProgramInput);
	itsAllocatedBlocksDisplay->ShareEditMenu(itsProgramInput);
	itsAllocatedBytesDisplay->ShareEditMenu(itsProgramInput);
	itsDeallocatedBlocksDisplay->ShareEditMenu(itsProgramInput);

	auto* windowsMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != nullptr );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MDStatsDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "MDStatsDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

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

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
MDStatsDirector::UpdateDisplay()
{
	itsRunProgramButton->SetActive(!itsProgramInput->GetText()->IsEmpty());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MDStatsDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPingTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		RequestRunningStats();
		}

	else if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		HandleResponse();
		}
	else if (sender == itsLink && message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
		CloseLink(false);
		}
	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		CloseLink(true);
		ReadExitStats();
		FinishProgram();
		}

	else if (sender == itsChooseProgramButton && message.Is(JXButton::kPushed))
		{
		ChooseProgram();
		}
	else if (sender == itsRunProgramButton && message.Is(JXButton::kPushed))
		{
		RunProgram();
		}

	else if (sender == itsProgramInput &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}

	else if (sender == itsRequestRecordsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			JMemoryManager::RecordFilter filter;
			itsRequestRecordsDialog->BuildFilter(&filter);
			RequestRecords(filter);
			}
		itsRequestRecordsDialog = nullptr;
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseProgram (private)

 ******************************************************************************/

void
MDStatsDirector::ChooseProgram()
{
	JString fullName;
	if ((JXGetChooseSaveFile())->ChooseFile(JString::empty, JString::empty, itsProgramInput->GetText()->GetText(), &fullName))
		{
		itsProgramInput->GetText()->SetText(fullName);
		}
}

/******************************************************************************
 RunProgram (private)

 ******************************************************************************/

void
MDStatsDirector::RunProgram()
{
	if (itsProgramInput->GetText()->IsEmpty())
		{
		return;
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

	JString cmd = itsProgramInput->GetText()->GetText();
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
MDStatsDirector::FinishProgram()
{
	// remember to update RunProgram()

	itsChooseProgramButton->Show();
	itsRunProgramButton->Show();
	itsProgramInput->Activate();
	itsProgramInput->Focus();
	itsArgsInput->Activate();
}

/******************************************************************************
 SendRequest (private)

 ******************************************************************************/

void
MDStatsDirector::SendRequest
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
MDStatsDirector::HandleResponse()
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
		std::cerr << "MDStatsDirector::HandleResponse received version (" << vers;
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
		ReceiveRecords(input, JGetString("QueryRecordsWindowTitle::MDStatsDirector"));
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
MDStatsDirector::RequestRunningStats()
{
	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ' << JMemoryManager::kRunningStatsMessage;

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
MDStatsDirector::ReceiveRunningStats
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
MDStatsDirector::ReceiveExitStats
	(
	std::istream& input
	)
{
	input >> itsExitStatsFile;
}

/******************************************************************************
 ReadExitStats (private)

	We don't need to check any version number because we already received
	itsExitStatsFile.

 ******************************************************************************/

void
MDStatsDirector::ReadExitStats()
{
	if (itsExitStatsFile.IsEmpty())
		{
		return;
		}

	std::ifstream input(itsExitStatsFile.GetBytes());

	while (true)
		{
		long type;
		input >> type;
		if (input.eof() || !input.good())
			{
			break;
			}
		else if (type == JMemoryManager::kErrorMessage)
			{
			ReceiveErrorMessage(input);
			}
		else if (type == JMemoryManager::kRunningStatsMessage)
			{
			ReceiveRunningStats(input);
			}
		else if (type == JMemoryManager::kRecordsMessage)
			{
			ReceiveRecords(input, JGetString("ExitRecordsWindowTitle::MDStatsDirector"));
			GetWindow()->SetTitle(JGetString("ExitWindowTitle::MDStatsDirector"));
			}
		else
			{
			break;
			}
		}

	input.close();
	JRemoveFile(itsExitStatsFile);
	itsExitStatsFile.Clear();

	if (itsMessageDir != nullptr)
		{
		itsMessageDir->ProcessFinished();
		}
}

/******************************************************************************
 RequestRecords (private)

 ******************************************************************************/

void
MDStatsDirector::RequestRecords
	(
	const JMemoryManager::RecordFilter& filter
	)
{
	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ' << JMemoryManager::kRecordsMessage;
	output << ' ';
	filter.Write(output);

	SendRequest(output);
}

/******************************************************************************
 ReceiveRecords (private)

 ******************************************************************************/

void
MDStatsDirector::ReceiveRecords
	(
	std::istream&	input,
	const JString&	windowTitle
	)
{
	auto* list = jnew MDRecordList;
	assert( list != nullptr );

	while (true)
		{
		bool keepGoing;
		input >> JBoolFromString(keepGoing);
		if (!keepGoing)
			{
			break;
			}

		auto* record = jnew MDRecord(input);
		assert( record != nullptr );
		list->AddRecord(record);
		}

	auto* dir = jnew MDRecordDirector(this, list, windowTitle);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 ReceiveErrorMessage (private)

 ******************************************************************************/

void
MDStatsDirector::ReceiveErrorMessage
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
MDStatsDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kGetRecordsCmd, itsProcess != nullptr);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MDStatsDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kGetRecordsCmd)
		{
		assert( itsRequestRecordsDialog == nullptr );
		itsRequestRecordsDialog = jnew MDFilterRecordsDialog(this);
		assert( itsRequestRecordsDialog != nullptr );
		itsRequestRecordsDialog->BeginDialog();
		ListenTo(itsRequestRecordsDialog);
		}

	else if (index == kQuitCmd)
		{
		MDGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
MDStatsDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MDStatsDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
		{
		(MDGetPrefsManager())->EditPrefs();
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
		(MDGetPrefsManager())->SaveWindowSize(kMDStatsDirectorWindSizeID, GetWindow());
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
MDStatsDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
MDStatsDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		MDGetApplication()->DisplayAbout();
		}

	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("MDOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection("MDMainHelp");
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
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MDStatsDirector::ReadPrefs
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
MDStatsDirector::WritePrefs
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
