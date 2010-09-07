/******************************************************************************
 MDStatsDirector.cc

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright © 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <mdStdInc.h>
#include "MDStatsDirector.h"
#include "MDFilterRecordsDialog.h"
#include "MDRecordDirector.h"
#include "MDRecordList.h"
#include "MDRecord.h"
#include "mdHelpText.h"
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
#include <JXColormap.h>
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

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Get allocated records           %i" kMDGetRecords
	"%l| Quit                  %k Meta-Q %i" kJXQuitAction;

enum
{
	kGetRecordsCmd = 1,
	kQuitCmd
};

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
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

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
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
	itsLink(NULL),
	itsProcess(NULL),
	itsPingTask(NULL),
	itsMessageDir(NULL),
	itsRequestRecordsDialog(NULL)
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

	CloseLink(kJTrue);
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
	assert( itsLink == NULL );
	itsLink = link;
	JMemoryManager::SetProtocol(itsLink);
	ListenTo(itsLink);

	DeleteDebugAcceptor();

	itsPingTask = new JXTimerTask(kRefreshInterval * 1000);
	assert( itsPingTask != NULL );
	itsPingTask->Start();
	ListenTo(itsPingTask);
}

/******************************************************************************
 CloseLink (private)

 *****************************************************************************/

void
MDStatsDirector::CloseLink
	(
	const JBoolean deleteProcess
	)
{
	DeleteDebugLink();

	if (deleteProcess)
		{
		delete itsProcess;
		itsProcess = NULL;
		}

	delete itsPingTask;
	itsPingTask = NULL;
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
	const JCoordinate minWidth  = 200;
	const JCoordinate minHeight = 200;

// begin JXLayout

    JXWindow* window = new JXWindow(this, 500,300, "");
    assert( window != NULL );

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
    assert( menuBar != NULL );

    itsToolBar =
        new JXToolBar(MDGetPrefsManager(), kMDStatsToolBarID, menuBar, minWidth, minHeight, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
    assert( itsToolBar != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDStatsDirector"));
	window->SetWMClass(MDGetWMClassInstance(), MDGetMainWindowClass());

	JXImage* image = new JXImage(GetDisplay(), md_main_window_icon);
	assert( image != NULL );
	window->SetIcon(image);

	JXContainer* statsEncl = itsToolBar->GetWidgetEnclosure();

// begin statsLayout

    const JRect statsLayout_Frame    = statsEncl->GetFrame();
    const JRect statsLayout_Aperture = statsEncl->GetAperture();
    statsEncl->AdjustSize(500 - statsLayout_Aperture.width(), 90 - statsLayout_Aperture.height());

    JXStaticText* obj1_statsLayout =
        new JXStaticText(JGetString("obj1_statsLayout::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 50,20);
    assert( obj1_statsLayout != NULL );
    obj1_statsLayout->SetToLabel();

    itsProgramInput =
        new JXFileInput(statsEncl,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 60,10, 370,20);
    assert( itsProgramInput != NULL );

    itsChooseProgramButton =
        new JXTextButton(JGetString("itsChooseProgramButton::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 430,10, 60,20);
    assert( itsChooseProgramButton != NULL );
    itsChooseProgramButton->SetShortcuts(JGetString("itsChooseProgramButton::MDStatsDirector::shortcuts::statsLayout"));

    JXStaticText* obj2_statsLayout =
        new JXStaticText(JGetString("obj2_statsLayout::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 50,20);
    assert( obj2_statsLayout != NULL );
    obj2_statsLayout->SetToLabel();

    itsArgsInput =
        new JXInputField(statsEncl,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 60,30, 370,20);
    assert( itsArgsInput != NULL );

    itsRunProgramButton =
        new JXTextButton(JGetString("itsRunProgramButton::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 430,30, 60,20);
    assert( itsRunProgramButton != NULL );
    itsRunProgramButton->SetShortcuts(JGetString("itsRunProgramButton::MDStatsDirector::shortcuts::statsLayout"));

    JXStaticText* obj3_statsLayout =
        new JXStaticText(JGetString("obj3_statsLayout::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 50,20);
    assert( obj3_statsLayout != NULL );
    obj3_statsLayout->SetToLabel();

    itsAllocatedBlocksDisplay =
        new JXStaticText("", kJFalse, kJTrue, NULL, statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,60, 90,20);
    assert( itsAllocatedBlocksDisplay != NULL );

    JXStaticText* obj4_statsLayout =
        new JXStaticText(JGetString("obj4_statsLayout::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 50,20);
    assert( obj4_statsLayout != NULL );
    obj4_statsLayout->SetToLabel();

    itsAllocatedBytesDisplay =
        new JXStaticText("", kJFalse, kJTrue, NULL, statsEncl,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 210,60, 90,20);
    assert( itsAllocatedBytesDisplay != NULL );

    JXStaticText* obj5_statsLayout =
        new JXStaticText(JGetString("obj5_statsLayout::MDStatsDirector::statsLayout"), statsEncl,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,60, 80,20);
    assert( obj5_statsLayout != NULL );
    obj5_statsLayout->SetToLabel();

    itsDeallocatedBlocksDisplay =
        new JXStaticText("", kJFalse, kJTrue, NULL, statsEncl,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 390,60, 90,20);
    assert( itsDeallocatedBlocksDisplay != NULL );

    statsEncl->SetSize(statsLayout_Frame.width(), statsLayout_Frame.height());

// end statsLayout

	itsAllocatedBlocksDisplay->SetToLabel();
	itsAllocatedBytesDisplay->SetToLabel();
	itsDeallocatedBlocksDisplay->SetToLabel();

	const JCoordinate headerHeight = itsAllocatedBlocksDisplay->GetFrame().bottom + 10;
	const JCoordinate histoHeight  = statsEncl->GetBoundsHeight() - headerHeight;

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,headerHeight, 100,histoHeight);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure(kJTrue, kJFalse);

	itsProgramInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsProgramInput);

	ListenTo(itsChooseProgramButton);
	ListenTo(itsRunProgramButton);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "MDStatsDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsProgramInput->AppendEditMenu(menuBar);
	itsArgsInput->ShareEditMenu(itsProgramInput);
	itsAllocatedBlocksDisplay->ShareEditMenu(itsProgramInput);
	itsAllocatedBytesDisplay->ShareEditMenu(itsProgramInput);
	itsDeallocatedBlocksDisplay->ShareEditMenu(itsProgramInput);

	JXWDMenu* windowsMenu =
		new JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != NULL );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MDStatsDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
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

	((GetDisplay())->GetWDManager())->DirectorCreated(this);
	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
MDStatsDirector::UpdateDisplay()
{
	itsRunProgramButton->SetActive(!itsProgramInput->IsEmpty());
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
		CloseLink(kJFalse);
		}
	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		CloseLink(kJTrue);
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
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateDisplay();
		}

	else if (sender == itsRequestRecordsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			JMemoryManager::RecordFilter filter;
			itsRequestRecordsDialog->BuildFilter(&filter);
			RequestRecords(filter);
			}
		itsRequestRecordsDialog = NULL;
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
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
	if ((JXGetChooseSaveFile())->ChooseFile("", NULL, itsProgramInput->GetText(), &fullName))
		{
		itsProgramInput->SetText(fullName);
		}
}

/******************************************************************************
 RunProgram (private)

 ******************************************************************************/

void
MDStatsDirector::RunProgram()
{
	if (itsProgramInput->IsEmpty())
		{
		return;
		}

	OpenDebugAcceptor();

	setenv("JMM_PIPE",             itsSocketName, 1);
	setenv("JMM_RECORD_ALLOCATED", "yes",         1);
	setenv("JMM_BROADCAST_ERRORS", "yes",         1);
	setenv("JMM_NO_PRINT_ERRORS",  "yes",         1);
	setenv("MALLOC_CHECK_",        "2",           1);

	JCharacter* v = getenv("JMM_INITIALIZE");
	if (v == NULL || JStringCompare(v, "no", kJFalse) == 0)
		{
		setenv("JMM_INITIALIZE", "default", 1);
		}

	v = getenv("JMM_SHRED");
	if (v == NULL || JStringCompare(v, "no", kJFalse) == 0)
		{
		setenv("JMM_SHRED", "default", 1);
		}

	JString cmd = itsProgramInput->GetText();
	cmd        += " ";
	cmd        += itsArgsInput->GetText();

	const JError err = JProcess::Create(&itsProcess, cmd);
	if (err.OK())
		{
		// remember to update FinishProgram()

		itsChooseProgramButton->Hide();
		itsRunProgramButton->Hide();
		itsProgramInput->Deactivate();
		itsArgsInput->Deactivate();
		itsAllocatedBlocksDisplay->SetBackColor(itsAllocatedBlocksDisplay->GetFocusColor());
		itsAllocatedBlocksDisplay->SetText("");
		itsAllocatedBytesDisplay->SetBackColor(itsAllocatedBytesDisplay->GetFocusColor());
		itsAllocatedBytesDisplay->SetText("");
		itsDeallocatedBlocksDisplay->SetBackColor(itsDeallocatedBlocksDisplay->GetFocusColor());
		itsDeallocatedBlocksDisplay->SetText("");
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
	itsLink->SendMessage(s.c_str(), s.length());
}

/******************************************************************************
 HandleResponse (private)

 ******************************************************************************/

void
MDStatsDirector::HandleResponse()
{
	assert( itsLink != NULL );

	JString text;
	const JBoolean ok = itsLink->GetNextMessage(&text);
	assert( ok );

	std::string s(text, text.GetLength());
	std::istringstream input(s);

	JFileVersion vers;
	input >> vers;
	if (vers != kJMemoryManagerDebugVersion)
		{
		cerr << "MDStatsDirector::HandleResponse received version (" << vers;
		cerr << ") different than expected (" << kJMemoryManagerDebugVersion << ")" << endl;
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
	const JBoolean	raw = kJTrue
	)
{
	const JBoolean hadSelection = field->HasSelection();
	JString s;
	if (raw)
		{
		s = JString(value, JString::kBase10);
		}
	else
		{
		s = JPrintFileSize(value);
		}
	field->SetText(s);
	if (hadSelection)
		{
		field->SelectAll();
		}
}

void
MDStatsDirector::ReceiveRunningStats
	(
	istream& input
	)
{
	JSize value;
	input >> value;
	mdSetValue(itsAllocatedBlocksDisplay, value);

	input >> value;
	mdSetValue(itsAllocatedBytesDisplay, value, kJFalse);

	input >> value;
	mdSetValue(itsDeallocatedBlocksDisplay, value);

	JSize count;
	input >> count;
	assert( count == JMemoryManager::kHistogramSlotCount );

	JSize histo[ JMemoryManager::kHistogramSlotCount ];
	for (JIndex i=0; i<count; i++)
		{
		input >> histo[i];
		}
}

/******************************************************************************
 ReceiveExitStats (private)

 ******************************************************************************/

void
MDStatsDirector::ReceiveExitStats
	(
	istream& input
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

	std::ifstream input(itsExitStatsFile);

	while (1)
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
			(GetWindow())->SetTitle(JGetString("ExitWindowTitle::MDStatsDirector"));
			}
		else
			{
			break;
			}
		}

	input.close();
	JRemoveFile(itsExitStatsFile);
	itsExitStatsFile.Clear();

	if (itsMessageDir != NULL)
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
	istream&			input,
	const JCharacter*	windowTitle
	)
{
	MDRecordList* list = new MDRecordList;
	assert( list != NULL );

	while (1)
		{
		JBoolean keepGoing;
		input >> keepGoing;
		if (!keepGoing)
			{
			break;
			}

		MDRecord* record = new MDRecord(input);
		assert( record != NULL );
		list->AddRecord(record);
		}

	MDRecordDirector* dir = new MDRecordDirector(this, list, windowTitle);
	assert( dir != NULL );
	dir->Activate();
}

/******************************************************************************
 ReceiveErrorMessage (private)

 ******************************************************************************/

void
MDStatsDirector::ReceiveErrorMessage
	(
	istream& input
	)
{
	JString msg;
	input >> msg;

	if (itsMessageDir == NULL)
		{
		itsMessageDir = new JXPGMessageDirector(this);
		assert( itsMessageDir != NULL );
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
	itsFileMenu->SetItemEnable(kGetRecordsCmd, JI2B(itsProcess != NULL));
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
		assert( itsRequestRecordsDialog == NULL );
		itsRequestRecordsDialog = new MDFilterRecordsDialog(this);
		assert( itsRequestRecordsDialog != NULL );
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
		(JXGetHelpManager())->ShowSection(kMDTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDMainHelpName);
		}

	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDCreditsName);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MDStatsDirector::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	(GetWindow())->ReadGeometry(input);

	JString s;
	input >> s;
	itsProgramInput->SetText(s);

	input >> s;
	itsArgsInput->SetText(s);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
MDStatsDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsProgramInput->GetText();
	output << ' ' << itsArgsInput->GetText();
}

/******************************************************************************
 OpenDebugAcceptor

 ******************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

void
MDStatsDirector::OpenDebugAcceptor()
{
	const JError err = JCreateTempFile(&itsSocketName);

	itsAcceptor = new MDLinkAcceptor(this);
	assert( itsAcceptor != NULL );

	JRemoveFile(itsSocketName);
	ACE_UNIX_Addr addr(itsSocketName);
	if (itsAcceptor->open(addr) == -1)
		{
		cerr << "error trying to create socket: " << jerrno() << endl;
		exit(1);
		}
}

/******************************************************************************
 DeleteDebugLink (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
MDStatsDirector::DeleteDebugLink()
{
	delete itsLink;
	itsLink = NULL;
}

void
MDStatsDirector::DeleteDebugAcceptor()
{
	if (itsAcceptor != NULL)
		{
		delete itsAcceptor;
		itsAcceptor = NULL;
		}
}

#define JTemplateName ACE_Acceptor
#define JTemplateType DebugLink, ACE_LSOCK_ACCEPTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType
