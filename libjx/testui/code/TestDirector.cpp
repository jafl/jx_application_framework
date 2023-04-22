/******************************************************************************
 TestDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestDirector.h"
#include "TestWidget.h"
#include "TestPGTask.h"
#include "AnimateHelpMenuTask.h"
#include "AnimateWindowIconTask.h"
#include "TestSaveFileDialog.h"
#include "TestInputFieldsDialog.h"
#include "TestButtonsDialog.h"
#include "TestPopupChoiceDialog.h"
#include "TestSliderDirector.h"
#include "TestPartitionDirector.h"
#include "TestTabDirector.h"
#include "TestStrTableDirector.h"
#include "TestFloatTableDirector.h"
#include "TestTextEditDocument.h"
#include "TestLinkedDocument.h"
#include "TestDNDTextDirector.h"
#include "TestImageDirector.h"
#include "TestFileListDirector.h"
#include "TestIdleTask.h"
#include "testjxGlobals.h"

#include "SmileyBitmaps.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jx/JXDisplayMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXEPSPrinter.h>
#include <jx-af/jx/JXImageMask.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXTipOfTheDayDialog.h>
#include <jx-af/jx/JXStandAlonePG.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXChoosePathDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/jXActionDefs.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JStopWatch.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JConstBitmap.h>
#include <jx-af/jcore/JBroadcastSnooper.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <unistd.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kWindowGeomFileName = "testjx_window_geom";

// About menu

static const JUtf8Byte* kAboutMenuStr =
	"    About            %h a"
	"  | from string db   %i" kJXHelpSpecificAction		// "Help %h h %k Ctrl-H"
	"  | Tip of the Day   %h t"
	"%l| Print PostScript %h p"
	"  | Print EPS...     %h e %k Ctrl-E"
	"%l| Quit             %h q %k Ctrl-Q"
	"%l";	// test that JXTextMenu ignores this

enum
{
	kAboutCmd = 1, kHelpCmd, kTipCmd,
	kPrintPSMenuCmd, kPrintEPSCmd,
	kQuitCmd
};

// PostScript printing menu

static const JUtf8Byte* kPrintPSMenuStr =
	"    Page setup..."
	"  | Print...      %h p %k Ctrl-P";

enum
{
	kPSPageSetupCmd = 1, kPrintPSCmd
};

// Test menu

static const JUtf8Byte* kTestMenuStr =
	"    New main window"
	"%l| User notification"
	"  | File chooser"
	"  | Progress display"
	"%l| Input fields  %k Ctrl-I"
	"  | Buttons       %k Ctrl-B"
	"  | Popup menus"
	"  | Sliders"
	"%l| Partitions"
	"  | Tab group"
	"  | String table"
	"  | Number table"
	"  | Text editor                       %k Ctrl-O"
	"  | Linked documents                  %k Super-O"
	"  | Drag-And-Drop (text)              %k Hyper-O"
	"  | Image viewer                      %k Ctrl-I"
	"  | File List"
	"%l| Send email"
	"  | Show file content"
	"  | Show web page"
	"%l| Enable Smileys menu %b"
	"%l| Zombie process"
	"%l| Place window at (0,0)             %k Ctrl-1"
	"  | Place window at (30,30)           %k Ctrl-2"
	"  | Place window at (100,100)         %k Ctrl-3"
	"  | Move window by (+10,+10)          %k Ctrl-4"
	"  | Raise all windows"
	"  | Window config                     %k Ctrl-0"
	"%l| Idle task mutex"
	"%l| Force broken pipe (does not dump core)"
	"  | Generate X error (dumps core)"
	"  | Lock up for 10 seconds (test MDI)"
	"%l| Timing for font substitution";

enum
{
	kNewTestDirectorCmd = 1,
	kTestUserNotifyMenuCmd, kTestChooseSaveFileMenuCmd, kTestPGMenuCmd,
	kTestInputCmd, kTestButtonsCmd, kTestPopupChoiceCmd, kTestSliderCmd,
	kTestPartitionsCmd, kTestTabGroupCmd,
	kTestStrTableCmd, kTestNumTableCmd,
	kTestTextEditorCmd, kTestLinkedDocCmd, kTestDNDTextCmd,
	kTestImageViewCmd, kTestFileListCmd,
	kSendEmailCmd, kShowFileContentCmd, kShowWebPageCmd,
	kTestDisabledMenuCmd,
	kTestZombieProcessCmd,
	kTestPlaceWindow0Cmd, kTestPlaceWindow30Cmd, kTestPlaceWindow100Cmd,
	kTestMoveWindowCmd, kRaiseAllWindowsCmd, kPrintWMConfigCmd,
	kTestIdleTaskMutexCmd,
	kTestBrokenPipeCmd, kTestUncaughtXErrorCmd, kLockUpToTestMDICmd,
	kTimeFontSubCmd
};

// UserNotification menu

static const JUtf8Byte* kUserNotificationMenuStr =
	"Message|Warning|Error";

enum
{
	kTestMessageCmd = 1, kTestWarningCmd, kTestErrorCmd
};

// ChooseSaveFile menu

static const JUtf8Byte* kChooseSaveFileMenuStr =
	"    Choose file..."
	"  | Choose file... (custom)"
	"  | Save file..."
	"  | Save file (custom)..."
	"%l| Choose readable directory..."
	"  | Choose writable directory...";

enum
{
	kChooseFileCmd = 1, kChooseFileCustomCmd,
	kSaveFileCmd, kSaveFileCustomCmd,
	kChooseRPathCmd, kChooseRWPathCmd
};

// ProgressDisplay menu

static const JUtf8Byte* kProgressDisplayMenuStr =
	"    Fixed length"
	"  | Variable length"
	"%l| Fixed length (background)"
	"  | Variable length (background)";

enum
{
	kFixLenFGCmd = 1, kVarLenFGCmd, kFixLenBGCmd, kVarLenBGCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

TestDirector::TestDirector
	(
	JXDirector*		supervisor,
	const bool	isMaster,
	const bool	startIconic,
	const bool	bufferTestWidget,
	const bool	testWidgetIsImage,
	const bool	snoopWindow
	)
	:
	JXWindowDirector(supervisor),
	itsIsMasterFlag(isMaster),
	itsPSPrinter(nullptr),
	itsEPSPrinter(nullptr)
{
	BuildWindow(isMaster, bufferTestWidget, testWidgetIsImage);

	JXWindow* window = GetWindow();
	if (snoopWindow)
	{
		itsWindowSnooper = jnew JBroadcastSnooper(window);
		assert( itsWindowSnooper != nullptr );
	}
	else
	{
		itsWindowSnooper = nullptr;
	}

	if (isMaster && JFileExists(JString(kWindowGeomFileName, JString::kNoCopy)))
	{
		std::ifstream input(kWindowGeomFileName);
		window->ReadGeometry(input);
	}

	if (startIconic)
	{
		window->Iconify();
	}

	// GetDisplay() only works after SetWindow()

	itsPSPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != nullptr );

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestDirector::~TestDirector()
{
	std::ofstream output(kWindowGeomFileName);
	GetWindow()->WriteGeometry(output);

	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
	jdelete itsAnimHelpTask;
	jdelete itsAnimIconTask;

	jdelete itsWindowSnooper;
}

/******************************************************************************
 OpenTextFile

 ******************************************************************************/

void
TestDirector::OpenTextFile
	(
	const JString& fileName
	)
{
	if (JFileReadable(fileName))
	{
		TestTextEditDocument* doc = jnew TestTextEditDocument(this, fileName, false);
		assert( doc != nullptr );
		doc->Activate();
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		JGetUserNotification()->ReportError(
			JGetString("FileNotReadable::TestDirector", map, sizeof(map)));
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestDirector::BuildWindow
	(
	const bool isMaster,
	const bool bufferTestWidget,
	const bool testWidgetIsImage
	)
{
	JXDisplay* display = JXGetApplication()->GetCurrentDisplay();

// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestDirector"));
	window->SetWMClass("testjx", "TestDirector");

	window->SetMinSize(150,150);
	if (isMaster)
	{
		window->SetCloseAction(JXWindow::kQuitApp);
	}
	else
	{
		window->SetCloseAction(JXWindow::kCloseDisplay);
	}

	itsAnimIconTask = jnew AnimateWindowIconTask(GetWindow());
	assert( itsAnimIconTask != nullptr );
	ListenTo(window, std::function([this](const JXWindow::Iconified&)
	{
		itsAnimIconTask->Start();
	}));
	ListenTo(window, std::function([this](const JXWindow::Deiconified&)
	{
		itsAnimIconTask->Stop();
	}));

	// menus

	JXImage* aboutTitleImage =
		jnew JXImage(display, kSmileyBitmap[ kHappySmileyIndex ], JColorManager::GetRedColor());
	assert( aboutTitleImage != nullptr );
	itsAboutMenu = menuBar->AppendTextMenu(aboutTitleImage, true);
	itsAboutMenu->SetShortcuts(JGetString("AboutMenuShortcut::TestDirector"));
	itsAboutMenu->SetMenuItems(kAboutMenuStr, "TestDirector");
	itsAboutMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsAboutMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdateAboutMenu, this),
		std::bind(&TestDirector::HandleAboutMenu, this, std::placeholders::_1));

	itsAnimHelpTask = jnew AnimateHelpMenuTask(itsAboutMenu, kHelpCmd);
	assert( itsAnimHelpTask != nullptr );

	itsPrintPSMenu = jnew JXTextMenu(itsAboutMenu, kPrintPSMenuCmd, menuBar);
	assert( itsPrintPSMenu != nullptr );
	itsPrintPSMenu->SetMenuItems(kPrintPSMenuStr);
	itsPrintPSMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrintPSMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdatePrintPSMenu, this),
		std::bind(&TestDirector::HandlePrintPSMenu, this, std::placeholders::_1));

	itsTestMenu = menuBar->AppendTextMenu(JGetString("TestMenuTitle::TestDirector"));
	itsTestMenu->SetMenuItems(kTestMenuStr);
	itsTestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTestMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdateTestMenu, this),
		std::bind(&TestDirector::HandleTestMenu, this, std::placeholders::_1));

	itsUNMenu = jnew JXTextMenu(itsTestMenu, kTestUserNotifyMenuCmd, menuBar);
	assert( itsUNMenu != nullptr );
	itsUNMenu->SetMenuItems(kUserNotificationMenuStr);
	itsUNMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsUNMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdateUNMenu, this),
		std::bind(&TestDirector::HandleUNMenu, this, std::placeholders::_1));

	itsCSFMenu = jnew JXTextMenu(itsTestMenu, kTestChooseSaveFileMenuCmd, menuBar);
	assert( itsCSFMenu != nullptr );
	itsCSFMenu->SetMenuItems(kChooseSaveFileMenuStr);
	itsCSFMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsCSFMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdateCSFMenu, this),
		std::bind(&TestDirector::HandleCSFMenu, this, std::placeholders::_1));

	itsPGMenu = jnew JXTextMenu(itsTestMenu, kTestPGMenuCmd, menuBar);
	assert( itsPGMenu != nullptr );
	itsPGMenu->SetMenuItems(kProgressDisplayMenuStr);
	itsPGMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPGMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdatePGMenu, this),
		std::bind(&TestDirector::HandlePGMenu, this, std::placeholders::_1));

	if (isMaster)
	{
		itsDisplayMenu =
			jnew JXDisplayMenu(JGetString("DisplayMenuTitle::TestDirector"), menuBar,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  0,0, 10,10);
		assert( itsDisplayMenu != nullptr );
		menuBar->AppendMenu(itsDisplayMenu);
	}
	else
	{
		itsDisplayMenu = nullptr;
	}

	itsWidget =
		jnew TestWidget(isMaster, testWidgetIsImage,
					   menuBar, scrollbarSet,
					   scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic,
					   0,0, 10,10);
	assert( itsWidget != nullptr );
	itsWidget->FitToEnclosure(true, true);
	itsWidget->SetSingleFocusWidget();

	BuildIconMenus(window, menuBar);

	// do this -after- constructing JXScrollableWidget

	GetWindow()->BufferDrawing(bufferTestWidget);
}

/******************************************************************************
 BuildIconMenus

	Creates a pile of menus to exercise JXImageMenu.  The menu that appears
	on the menu bar is told to own the four icons so they will get deleted
	when the window is deleted.  All the submenus are told -not- to own the
	icons so they won't get deleted more than once.

 ******************************************************************************/

void
TestDirector::BuildIconMenus
	(
	JXWindow*	window,
	JXMenuBar*	menuBar
	)
{
	// create icons

	JXDisplay* display = window->GetDisplay();

	const JColorID kSmileyColor[] =
	{
		JColorManager::GetWhiteColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetBlueColor(),
		JColorManager::GetBlackColor()
	};

	JXImage* image[kSmileyBitmapCount];
	for (JUnsignedOffset i=0; i<kSmileyBitmapCount; i++)
	{
		image[i] = jnew JXImage(display, kSmileyBitmap[i], kSmileyColor[i]);
		assert( image[i] != nullptr );
	}

	// create 1x6 menu in menu bar -- this owns the icons

	itsSmileyMenu =
		jnew JXImageMenu(JGetString("IconMenuTitle::TestDirector"), 6, menuBar,
						 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSmileyMenu != nullptr );
	itsSmileyMenu->SetUpdateAction(JXMenu::kDisableNone);
	menuBar->AppendMenu(itsSmileyMenu);

	for (auto* i : image)
	{
		itsSmileyMenu->AppendItem(i, true);
	}

	// create 2x2 submenu of radio buttons

	itsIconMenu = jnew JXImageMenu(2, itsSmileyMenu, 2, menuBar);
	assert( itsIconMenu != nullptr );
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);

	for (auto* i : image)
	{
		itsIconMenu->AppendItem(i, false, JXMenu::kRadioType);
	}

	itsIconMenuItem = 1;

	itsIconMenu->AttachHandlers(this,
		std::bind(&TestDirector::UpdateIconMenu, this),
		std::bind(&TestDirector::HandleIconMenu, this, std::placeholders::_1));

	// create 3x5 submenu that has a few unused cells

	JXImageMenu* submenu = jnew JXImageMenu(5, itsSmileyMenu, 4, menuBar);
	assert( submenu != nullptr );
	submenu->SetUpdateAction(JXMenu::kDisableNone);

	for (JIndex j=1; j<=3; j++)
	{
		for (auto* i : image)
		{
			submenu->AppendItem(i, false);
		}
	}
}

/******************************************************************************
 UpdateAboutMenu (private)

 ******************************************************************************/

void
TestDirector::UpdateAboutMenu()
{
	itsAnimHelpTask->Reset();
	itsAnimHelpTask->Start();		// removes itself
}

/******************************************************************************
 HandleAboutMenu (private)

 ******************************************************************************/

void
TestDirector::HandleAboutMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		TestjxGetApplication()->DisplayAbout(GetDisplay());
	}
	else if (index == kHelpCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
	else if (index == kTipCmd)
	{
		auto* dlog = jnew JXTipOfTheDayDialog(true, false);
		assert( dlog != nullptr );
		if (dlog->DoDialog())
		{
			std::cout << "Should show at startup: " << JBoolToString(dlog->ShowAtStartup()) << std::endl;
		}
	}

	else if (index == kPrintEPSCmd)
	{
		if (itsEPSPrinter->ConfirmUserPrintSetup())
		{
			itsWidget->Print(*itsEPSPrinter);
		}
	}

	else if (index == kQuitCmd && itsIsMasterFlag)
	{
		JXGetApplication()->Quit();
	}
	else if (index == kQuitCmd)
	{
		GetDisplay()->Close();
	}
}

/******************************************************************************
 UpdatePrintPSMenu (private)

 ******************************************************************************/

void
TestDirector::UpdatePrintPSMenu()
{
}

/******************************************************************************
 HandlePrintPSMenu (private)

 ******************************************************************************/

void
TestDirector::HandlePrintPSMenu
	(
	const JIndex index
	)
{
	if (index == kPSPageSetupCmd)
	{
		itsPSPrinter->EditUserPageSetup();
	}
	else if (index == kPrintPSCmd)
	{
		if (itsPSPrinter->ConfirmUserPrintSetup())
		{
			itsWidget->Print(*itsPSPrinter);
		}
	}
}

/******************************************************************************
 UpdateTestMenu (private)

 ******************************************************************************/

void
TestDirector::UpdateTestMenu()
{
	if (itsSmileyMenu->IsActive())
	{
		itsTestMenu->CheckItem(kTestDisabledMenuCmd);
	}
}

/******************************************************************************
 HandleTestMenu (private)

 ******************************************************************************/

void
TestDirector::HandleTestMenu
	(
	const JIndex index
	)
{
	if (index == kNewTestDirectorCmd)	// mainly for placing on other X displays
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestDirector* dir = jnew TestDirector(TestjxGetApplication(), false);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestInputCmd)
	{
		TestInputFieldsDialog* dir = jnew TestInputFieldsDialog();
		assert( dir != nullptr );
		dir->DoDialog();
	}
	else if (index == kTestButtonsCmd)
	{
		TestButtonsDialog* dir = jnew TestButtonsDialog();
		assert( dir != nullptr );
		dir->DoDialog();
	}
	else if (index == kTestPopupChoiceCmd)
	{
		TestPopupChoiceDialog* dir = jnew TestPopupChoiceDialog();
		assert( dir != nullptr );
		dir->DoDialog();
	}
	else if (index == kTestSliderCmd)
	{
		TestSliderDirector* dir = jnew TestSliderDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestPartitionsCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestPartitionDirector* dir = jnew TestPartitionDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}
	else if (index == kTestTabGroupCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestTabDirector* dir = jnew TestTabDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestStrTableCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestStrTableDirector* dir = jnew TestStrTableDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}
	else if (index == kTestNumTableCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestFloatTableDirector* dir = jnew TestFloatTableDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestTextEditorCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestTextEditDocument* doc = jnew TestTextEditDocument(this);
		assert( doc != nullptr );
		doc->Activate();
	}

	else if (index == kTestLinkedDocCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestLinkedDocument* doc = jnew TestLinkedDocument(this);
		assert( doc != nullptr );
		doc->Activate();
	}

	else if (index == kTestDNDTextCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestDNDTextDirector* dir = jnew TestDNDTextDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestImageViewCmd)
	{
		TestImageDirector* dir = jnew TestImageDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kTestFileListCmd)
	{
		TestFileListDirector* dir = jnew TestFileListDirector(this);
		assert( dir != nullptr );
		dir->Activate();
	}

	else if (index == kSendEmailCmd)
	{
		JXGetWebBrowser()->ShowURL(JString("mailto:me@example.com", JString::kNoCopy));
	}
	else if (index == kShowFileContentCmd)
	{
		JXGetWebBrowser()->ShowURL(JString("file:/etc/hosts", JString::kNoCopy));
	}
	else if (index == kShowWebPageCmd)
	{
		JXGetWebBrowser()->ShowURL(JString("http://example.com", JString::kNoCopy));
	}

	else if (index == kTestDisabledMenuCmd)
	{
		itsSmileyMenu->SetActive(!itsSmileyMenu->IsActive());
	}

	else if (index == kTestZombieProcessCmd)
	{
		pid_t pid;
		std::cout << std::endl;
		const JError err = JExecute(JString("ls", JString::kNoCopy), &pid);
		std::cout << std::endl;
		if (err.OK())
		{
			JGetUserNotification()->DisplayMessage(
				JGetString("ZombieProcessNotification::TestDirector"));
		}
		else
		{
			err.ReportIfError();
		}
	}

	else if (index == kTestPlaceWindow0Cmd)
	{
		JXWindow* window = GetWindow();
		window->Place(0,0);

		std::cout << std::endl;
		std::cout << "Window is now at " << window->GetDesktopLocation() << std::endl;
	}
	else if (index == kTestPlaceWindow30Cmd)
	{
		JXWindow* window = GetWindow();
		window->Place(30,30);

		std::cout << std::endl;
		std::cout << "Window is now at " << window->GetDesktopLocation() << std::endl;
	}
	else if (index == kTestPlaceWindow100Cmd)
	{
		JXWindow* window = GetWindow();
		window->Place(100,100);

		std::cout << std::endl;
		std::cout << "Window is now at " << window->GetDesktopLocation() << std::endl;
	}
	else if (index == kTestMoveWindowCmd)
	{
		JXWindow* window = GetWindow();
		window->Move(10,10);

		std::cout << std::endl;
		std::cout << "Window is now at " << window->GetDesktopLocation() << std::endl;
	}
	else if (index == kRaiseAllWindowsCmd)
	{
		GetDisplay()->RaiseAllWindows();
	}
	else if (index == kPrintWMConfigCmd)
	{
		GetWindow()->PrintWindowConfig();
	}

	else if (index == kTestIdleTaskMutexCmd)
	{
		(jnew TestIdleTask())->Start();
	}

	else if (index == kTestBrokenPipeCmd)
	{
		// This is clearly a ludicrous action, but it does test the
		// JCore signal handling system.

		int fd[2];
		const JError err = JCreatePipe(fd);
		if (err.OK())
		{
			close(fd[0]);

			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wunused-result"
			write(fd[1], "arf", 3);		// write to pipe with no readers => SIGPIPE
			#pragma GCC diagnostic pop
		}
		else
		{
			err.ReportIfError();
		}
	}

	else if (index == kTestUncaughtXErrorCmd)
	{
		// This is clearly a ludicrous action, but it does test the
		// JX error handling system.

		XDestroyWindow(*(GetDisplay()), None);
	}

	else if (index == kLockUpToTestMDICmd)
	{
		JWait(10.0);
	}

	else if (index == kTimeFontSubCmd)
	{
		TestFontSubstitutionTiming();
	}
}

/******************************************************************************
 TestFontSubstitutionTiming (private)

 ******************************************************************************/

void
TestDirector::TestFontSubstitutionTiming()
{
	const JSize size = 1024*1024;
	JKLRand r;

	JFontManager* fontMgr = GetDisplay()->GetFontManager();

	JString s;
	s.SetBlockSize(size);
	for (JIndex i=1; i<=size; i++)
	{
		s.Append(JUtf8Character(r.UniformLong(32, 126)));
	}

	const JFont f = JFontManager::GetDefaultFont();

	JStopWatch w;
	w.StartTimer();

	bool hasGlyphs = f.HasGlyphsForString(fontMgr, s);

	w.StopTimer();
	std::cout << "check ascii glyphs: " << JBoolToString(hasGlyphs) << ' ' << w.PrintTimeInterval() << std::endl;
{
	JStringIterator iter(s);
	JUtf8Character c;

	w.StartTimer();

	while (iter.Next(&c))
	{
		JFont f1 = f;
		f1.SubstituteToDisplayGlyph(fontMgr, c);
	}
}
	w.StopTimer();
	std::cout << "substitute ascii glyphs: " << w.PrintTimeInterval() << std::endl;

	s.SetBlockSize(4*size);
	s.Clear();
	for (JIndex i=1; i<=size; i++)
	{
		s.Append(JUtf8Character::Utf32ToUtf8(r.UniformLong(46000, 55000)));
	}

	w.StartTimer();

	hasGlyphs = f.HasGlyphsForString(fontMgr, s);

	w.StopTimer();
	std::cout << "check korean glyphs: " << JBoolToString(hasGlyphs) << ' ' << w.PrintTimeInterval() << std::endl;
{
	JStringIterator iter(s);
	JUtf8Character c;

	w.StartTimer();

	while (iter.Next(&c))
	{
		JFont f1 = f;
		f1.SubstituteToDisplayGlyph(fontMgr, c);
	}
}
	w.StopTimer();
	std::cout << "substitute korean glyphs: " << w.PrintTimeInterval() << std::endl;
}

/******************************************************************************
 UpdateUNMenu (private)

 ******************************************************************************/

void
TestDirector::UpdateUNMenu()
{
}

/******************************************************************************
 HandleUNMenu (private)

 ******************************************************************************/

void
TestDirector::HandleUNMenu
	(
	const JIndex index
	)
{
	if (index == kTestMessageCmd)
	{
		JGetUserNotification()->DisplayMessage(JGetString("TestMessage::TestDirector"));
	}
	else if (index == kTestWarningCmd)
	{
		std::cout << JGetUserNotification()->AskUserYes(JGetString("WarningMessage::TestDirector")) << std::endl;
	}
	else if (index == kTestErrorCmd)
	{
		JGetUserNotification()->ReportError(JGetString("ErrorMessage::TestDirector"));
	}
}

/******************************************************************************
 UpdateCSFMenu (private)

 ******************************************************************************/

void
TestDirector::UpdateCSFMenu()
{
}

/******************************************************************************
 HandleCSFMenu (private)

 ******************************************************************************/

void
TestDirector::HandleCSFMenu
	(
	const JIndex index
	)
{
	bool ok = false;
	JString resultStr;
	if (index == kChooseFileCmd)
	{
		auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, JString::empty, JString::empty, JGetString("ChooseFileInstructions::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetFullName();
		}
	}
	else if (index == kChooseFileCustomCmd)
	{
		auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, JString("./about_owner_docs", JString::kNoCopy), JString::empty, JGetString("CustomChooseFileInstructions::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetFullName();
		}
	}
	else if (index == kSaveFileCmd)
	{
		auto* dlog = JXSaveFileDialog::Create(JGetString("SaveFilePrompt::TestDirector"), JGetString("DefaultFileName::TestDirector"), JString::empty, JGetString("SaveFileInstructions::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetFullName();
		}
	}
	else if (index == kSaveFileCustomCmd)
	{
		auto* dlog = TestSaveFileDialog::Create(TestSaveFileDialog::kPNGFormat, JGetString("SaveFilePrompt::TestDirector"), JGetString("DefaultFileName::TestDirector"), JString::empty, JGetString("CustomSaveFileInstructions::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetFullName();

			TestSaveFileDialog::SaveFormat format = dlog->GetSaveFormat();
			const JUtf8Byte* formatStr =
				(format == TestSaveFileDialog::kGIFFormat  ? "GIF"  :
				(format == TestSaveFileDialog::kPNGFormat  ? "PNG"  :
				(format == TestSaveFileDialog::kJPEGFormat ? "JPEG" : "unknown")));

			const JUtf8Byte* map[] =
			{
				"format", formatStr
			};
			resultStr += JGetString("CustomSaveFileResult::TestDirector", map, sizeof(map));
		}
	}
	else if (index == kChooseRPathCmd)
	{
		auto* dlog = JXChoosePathDialog::Create(JXChoosePathDialog::kAcceptReadable, JString::empty, JString::empty, JGetString("ChooseReadableDirectory::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetPath();
		}
	}
	else if (index == kChooseRWPathCmd)
	{
		auto* dlog = JXChoosePathDialog::Create(JXChoosePathDialog::kRequireWritable, JString::empty, JString::empty, JGetString("ChooseWritableDirectory::TestDirector"));
		ok = dlog->DoDialog();
		if (ok)
		{
			resultStr = dlog->GetPath();
		}
	}

	if (ok)
	{
		const JUtf8Byte* map[] =
		{
			"result", resultStr.GetBytes()
		};
		JGetUserNotification()->DisplayMessage(JGetString("CSFResult::TestDirector", map, sizeof(map)));
	}
}

/******************************************************************************
 UpdatePGMenu (private)

 ******************************************************************************/

void
TestDirector::UpdatePGMenu()
{
}

/******************************************************************************
 HandlePGMenu (private)

 ******************************************************************************/

void
TestDirector::HandlePGMenu
	(
	const JIndex index
	)
{
	if (index == kFixLenFGCmd)
	{
		FGProcess(true);
	}
	else if (index == kVarLenFGCmd)
	{
		FGProcess(false);
	}
	else if (index == kFixLenBGCmd)
	{
		BeginBGProcess(true);
	}
	else if (index == kVarLenBGCmd)
	{
		BeginBGProcess(false);
	}
}

/******************************************************************************
 FGProcess (private)

 ******************************************************************************/

void
TestDirector::FGProcess
	(
	const bool fixedLength
	)
{
	JProgressDisplay* pg = JNewPG();
	dynamic_cast<JXStandAlonePG*>(pg)->RaiseWhenUpdate();

	const JSize stepCount = 100;
	if (fixedLength)
	{
		pg->FixedLengthProcessBeginning(
			stepCount, JGetString("ProgressMessage::TestDirector"), true, true);
	}
	else
	{
		pg->VariableLengthProcessBeginning(
			JGetString("ProgressMessage::TestDirector"), true, true);
	}

	for (JIndex i=1; i<=stepCount; i++)
	{
		JWait(0.5);		// simulate massive, greedy number crunching

		// update the display

		JString progressStr(i, 0);
		progressStr.Prepend("iteration ");
		if (!pg->IncrementProgress(progressStr))
		{
			break;
		}
	}

	pg->ProcessFinished();
}

/******************************************************************************
 BeginBGProcess (private)

 ******************************************************************************/

void
TestDirector::BeginBGProcess
	(
	const bool fixedLength
	)
{
	TestPGTask* task = jnew TestPGTask(fixedLength);
	assert( task != nullptr );
	task->Start();
}

/******************************************************************************
 UpdateIconMenu (private)

 ******************************************************************************/

void
TestDirector::UpdateIconMenu()
{
	itsIconMenu->CheckItem(itsIconMenuItem);
}

/******************************************************************************
 HandleIconMenu (private)

 ******************************************************************************/

void
TestDirector::HandleIconMenu
	(
	const JIndex index
	)
{
	itsIconMenuItem = index;
}
