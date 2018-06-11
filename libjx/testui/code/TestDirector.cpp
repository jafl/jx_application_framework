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
#include "TestChooseSaveFile.h"
#include "testjxGlobals.h"

#include "SmileyBitmaps.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXImageMenu.h>
#include <JXDisplayMenu.h>
#include <JXScrollbarSet.h>
#include <JXPSPrinter.h>
#include <JXEPSPrinter.h>
#include <JXImageMask.h>
#include <JXColorManager.h>
#include <JXHelpManager.h>
#include <JXTipOfTheDayDialog.h>
#include <JXStandAlonePG.h>
#include <jXActionDefs.h>

#include <JConstBitmap.h>
#include <JBroadcastSnooper.h>
#include <jProcessUtil.h>
#include <jTime.h>
#include <jFileUtil.h>
#include <jSysUtil.h>
#include <unistd.h>
#include <stdio.h>
#include <jAssert.h>

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
	"%l| Enable Smileys menu %b"
	"%l| Zombie process"
	"%l| Place window at (0,0)             %k Ctrl-1"
	"  | Place window at (30,30)           %k Ctrl-2"
	"  | Place window at (100,100)         %k Ctrl-3"
	"  | Move window by (+10,+10)          %k Ctrl-4"
	"  | Raise all windows"
	"  | Window config                     %k Ctrl-0"
	"%l| Force broken pipe (does not dump core)"
	"  | Generate X error (dumps core)"
	"  | Lock up for 10 seconds (test MDI)";

enum
{
	kNewTestDirectorCmd = 1,
	kTestUserNotifyMenuCmd, kTestChooseSaveFileMenuCmd, kTestPGMenuCmd,
	kTestInputCmd, kTestButtonsCmd, kTestPopupChoiceCmd, kTestSliderCmd,
	kTestPartitionsCmd, kTestTabGroupCmd,
	kTestStrTableCmd, kTestNumTableCmd,
	kTestTextEditorCmd, kTestLinkedDocCmd, kTestDNDTextCmd,
	kTestImageViewCmd,
	kTestDisabledMenuCmd,
	kTestZombieProcessCmd,
	kTestPlaceWindow0Cmd, kTestPlaceWindow30Cmd, kTestPlaceWindow100Cmd,
	kTestMoveWindowCmd, kRaiseAllWindowsCmd, kPrintWMConfigCmd,
	kTestBrokenPipe, kTestUncaughtXError, kLockUpToTestMDICmd
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
	const JBoolean	isMaster,
	const JBoolean	startIconic,
	const JBoolean	bufferTestWidget,
	const JBoolean	testWidgetIsImage,
	const JBoolean	snoopWindow
	)
	:
	JXWindowDirector(supervisor)
{
	itsIsMasterFlag = isMaster;
	itsPSPrinter    = nullptr;
	itsEPSPrinter   = nullptr;

	itsCSF = jnew TestChooseSaveFile;
	assert( itsCSF != nullptr );

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

	if (isMaster && JFileExists(JString(kWindowGeomFileName, kJFalse)))
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
	ListenTo(itsPSPrinter);

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );
	ListenTo(itsEPSPrinter);
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
	jdelete itsCSF;
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
		TestTextEditDocument* doc = jnew TestTextEditDocument(this, fileName);
		assert( doc != nullptr );
		doc->Activate();
		}
	else
		{
		const JUtf8Byte* map[] =
			{
			"name", fileName.GetBytes()
			};
		(JGetUserNotification())->ReportError(
			JGetString("FileNotReadable::TestDirector", map, sizeof(map)));
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestDirector::BuildWindow
	(
	const JBoolean isMaster,
	const JBoolean bufferTestWidget,
	const JBoolean testWidgetIsImage
	)
{
	JXDisplay* display = (JXGetApplication())->GetCurrentDisplay();

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	JXScrollbarSet* scrollbarSet =
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
	ListenTo(window);		// for icon animation

	// menus

	JXImage* aboutTitleImage =
		jnew JXImage(display, kSmileyBitmap[ kHappySmileyIndex ], JColorManager::GetRedColor());
	assert( aboutTitleImage != nullptr );
	itsAboutMenu = menuBar->AppendTextMenu(aboutTitleImage, kJTrue);
	itsAboutMenu->SetShortcuts(JGetString("AboutMenuShortcut::TestDirector"));
	itsAboutMenu->SetMenuItems(kAboutMenuStr, "TestDirector");
	itsAboutMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsAboutMenu);

	itsAnimHelpTask = jnew AnimateHelpMenuTask(itsAboutMenu, kHelpCmd);
	assert( itsAnimHelpTask != nullptr );

	itsPrintPSMenu = jnew JXTextMenu(itsAboutMenu, kPrintPSMenuCmd, menuBar);
	assert( itsPrintPSMenu != nullptr );
	itsPrintPSMenu->SetMenuItems(kPrintPSMenuStr);
	itsPrintPSMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrintPSMenu);

	itsTestMenu = menuBar->AppendTextMenu(JGetString("TestMenuTitle::TestDirector"));
	itsTestMenu->SetMenuItems(kTestMenuStr);
	itsTestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTestMenu);

	itsUNMenu = jnew JXTextMenu(itsTestMenu, kTestUserNotifyMenuCmd, menuBar);
	assert( itsUNMenu != nullptr );
	itsUNMenu->SetMenuItems(kUserNotificationMenuStr);
	itsUNMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsUNMenu);

	itsCSFMenu = jnew JXTextMenu(itsTestMenu, kTestChooseSaveFileMenuCmd, menuBar);
	assert( itsCSFMenu != nullptr );
	itsCSFMenu->SetMenuItems(kChooseSaveFileMenuStr);
	itsCSFMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCSFMenu);

	itsPGMenu = jnew JXTextMenu(itsTestMenu, kTestPGMenuCmd, menuBar);
	assert( itsPGMenu != nullptr );
	itsPGMenu->SetMenuItems(kProgressDisplayMenuStr);
	itsPGMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPGMenu);

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
	itsWidget->FitToEnclosure(kJTrue, kJTrue);
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
JIndex i;

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
	for (i=0; i<kSmileyBitmapCount; i++)
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

	for (i=0; i<kSmileyBitmapCount; i++)
		{
		itsSmileyMenu->AppendItem(image[i], kJTrue);
		}

	// create 2x2 submenu of radio buttons

	itsIconMenu = jnew JXImageMenu(2, itsSmileyMenu, 2, menuBar);
	assert( itsIconMenu != nullptr );
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);

	for (i=0; i<kSmileyBitmapCount; i++)
		{
		itsIconMenu->AppendItem(image[i], kJFalse, JXMenu::kRadioType);
		}

	itsIconMenuItem = 1;
	ListenTo(itsIconMenu);

	// create 3x5 submenu that has a few unused cells

	JXImageMenu* submenu = jnew JXImageMenu(5, itsSmileyMenu, 4, menuBar);
	assert( submenu != nullptr );
	submenu->SetUpdateAction(JXMenu::kDisableNone);

	for (JIndex j=1; j<=3; j++)
		{
		for (i=0; i<kSmileyBitmapCount; i++)
			{
			submenu->AppendItem(image[i], kJFalse);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TestDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXWindow* window = GetWindow();		// ensure that it isn't const

	if (sender == itsAboutMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateAboutMenu();
		}
	else if (sender == itsAboutMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleAboutMenu(selection->GetIndex());
		}

	else if (sender == itsPrintPSMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrintPSMenu();
		}
	else if (sender == itsPrintPSMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrintPSMenu(selection->GetIndex());
		}

	else if (sender == itsTestMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTestMenu();
		}
	else if (sender == itsTestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleTestMenu(selection->GetIndex());
		}

	else if (sender == itsUNMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateUNMenu();
		}
	else if (sender == itsUNMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleUNMenu(selection->GetIndex());
		}

	else if (sender == itsCSFMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCSFMenu();
		}
	else if (sender == itsCSFMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleCSFMenu(selection->GetIndex());
		}

	else if (sender == itsPGMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePGMenu();
		}
	else if (sender == itsPGMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePGMenu(selection->GetIndex());
		}

	else if (sender == itsIconMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateIconMenu();
		}
	else if (sender == itsIconMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleIconMenu(selection->GetIndex());
		}

	else if (sender == window && message.Is(JXWindow::kIconified))
		{
		itsAnimIconTask->Start();
		}
	else if (sender == window && message.Is(JXWindow::kDeiconified))
		{
		itsAnimIconTask->Stop();
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsWidget->Print(*itsPSPrinter);
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsWidget->Print(*itsEPSPrinter);
			}
		}

	else if (message.Is(JXTipOfTheDayDialog::kShowAtStartup))
		{
		const JXTipOfTheDayDialog::ShowAtStartup* info =
			dynamic_cast<const JXTipOfTheDayDialog::ShowAtStartup*>(&message);
		assert( info != nullptr );
		std::cout << "Should show at startup: " << info->ShouldShowAtStartup() << std::endl;
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
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
		(TestjxGetApplication())->DisplayAbout(GetDisplay());
		}
	else if (index == kHelpCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
	else if (index == kTipCmd)
		{
		JXTipOfTheDayDialog* dlog = jnew JXTipOfTheDayDialog(kJTrue, kJFalse);
		assert( dlog != nullptr );
		dlog->BeginDialog();
		ListenTo(dlog);
		}

	else if (index == kPrintEPSCmd)
		{
		itsEPSPrinter->BeginUserPrintSetup();
		}

	else if (index == kQuitCmd && itsIsMasterFlag)
		{
		(JXGetApplication())->Quit();
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
		itsPSPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		itsPSPrinter->BeginUserPrintSetup();
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
		TestDirector* dir = jnew TestDirector(TestjxGetApplication(), kJFalse);
		assert( dir != nullptr );
		dir->Activate();
		}

	else if (index == kTestInputCmd)
		{
		TestInputFieldsDialog* dir = jnew TestInputFieldsDialog(this);
		assert( dir != nullptr );
		dir->BeginDialog();
		}
	else if (index == kTestButtonsCmd)
		{
		TestButtonsDialog* dir = jnew TestButtonsDialog(this);
		assert( dir != nullptr );
		dir->BeginDialog();
		}
	else if (index == kTestPopupChoiceCmd)
		{
		TestPopupChoiceDialog* dir = jnew TestPopupChoiceDialog(this);
		assert( dir != nullptr );
		dir->BeginDialog();
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

	else if (index == kTestDisabledMenuCmd)
		{
		itsSmileyMenu->SetActive(!itsSmileyMenu->IsActive());
		}

	else if (index == kTestZombieProcessCmd)
		{
		pid_t pid;
		std::cout << std::endl;
		const JError err = JExecute(JString("ls", kJFalse), &pid);
		std::cout << std::endl;
		if (err.OK())
			{
			(JGetUserNotification())->DisplayMessage(
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

	else if (index == kTestBrokenPipe)
		{
		// This is clearly a ludicrous action, but it does test the
		// JCore signal handling system.

		int fd[2];
		const JError err = JCreatePipe(fd);
		if (err.OK())
			{
			close(fd[0]);
			write(fd[1], "arf", 3);		// write to pipe with no readers => SIGPIPE
			}
		else
			{
			err.ReportIfError();
			}
		}

	else if (index == kTestUncaughtXError)
		{
		// This is clearly a ludicrous action, but it does test the
		// JX error handling system.

		XDestroyWindow(*(GetDisplay()), None);
		}

	else if (index == kLockUpToTestMDICmd)
		{
		JWait(10.0);
		}
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
		(JGetUserNotification())->DisplayMessage(JGetString("TestMessage::TestDirector"));
		}
	else if (index == kTestWarningCmd)
		{
		(JGetUserNotification())->AskUserYes(JGetString("WarningMessage::TestDirector"));
		}
	else if (index == kTestErrorCmd)
		{
		(JGetUserNotification())->ReportError(JGetString("ErrorMessage::TestDirector"));
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
	JChooseSaveFile* csf = JGetChooseSaveFile();

	JBoolean ok = kJFalse;
	JString resultStr;
	if (index == kChooseFileCmd)
		{
		ok = csf->ChooseFile(JGetString("ChooseFilePrompt::TestDirector"),
			JGetString("ChooseFileInstructions::TestDirector"),
			&resultStr);
		}
	else if (index == kChooseFileCustomCmd)
		{
		csf = itsCSF;
		ok  = csf->ChooseFile(JGetString("ChooseFilePrompt::TestDirector"),
			JGetString("CustomChooseFileInstructions::TestDirector"),
			&resultStr);
		}
	else if (index == kSaveFileCmd)
		{
		ok = csf->SaveFile(JGetString("SaveFilePrompt::TestDirector"),
			JGetString("SaveFileInstructions::TestDirector"),
			JGetString("DefaultFileName::TestDirector"), &resultStr);
		}
	else if (index == kSaveFileCustomCmd)
		{
		csf = itsCSF;
		ok  = csf->SaveFile(JGetString("SaveFilePrompt::TestDirector"),
			JGetString("CustomSaveFileInstructions::TestDirector"),
			JGetString("DefaultFileName::TestDirector"), &resultStr);

		if (ok)
			{
			TestChooseSaveFile::SaveFormat format = itsCSF->GetSaveFormat();
			const JUtf8Byte* formatStr =
				(format == TestChooseSaveFile::kGIFFormat  ? "GIF"  :
				(format == TestChooseSaveFile::kPNGFormat  ? "PNG"  :
				(format == TestChooseSaveFile::kJPEGFormat ? "JPEG" : "unknown")));

			const JUtf8Byte* map[] =
				{
				"format", formatStr
				};
			resultStr += JGetString("CustomSaveFileResult::TestDirector", map, sizeof(map));
			}
		}
	else if (index == kChooseRPathCmd)
		{
		ok = csf->ChooseRPath(JString::empty,
			JGetString("ChooseReadableDirectory::TestDirector"),
			JString::empty, &resultStr);
		}
	else if (index == kChooseRWPathCmd)
		{
		ok = csf->ChooseRWPath(JString::empty,
			JGetString("ChooseWritableDirectory::TestDirector"),
			JString::empty, &resultStr);
		}

	if (ok)
		{
		const JUtf8Byte* map[] =
			{
			"result", resultStr.GetBytes()
			};
		(JGetUserNotification())->DisplayMessage(JGetString("CSFResult::TestDirector", map, sizeof(map)));
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
		FGProcess(kJTrue);
		}
	else if (index == kVarLenFGCmd)
		{
		FGProcess(kJFalse);
		}
	else if (index == kFixLenBGCmd)
		{
		BeginBGProcess(kJTrue);
		}
	else if (index == kVarLenBGCmd)
		{
		BeginBGProcess(kJFalse);
		}
}

/******************************************************************************
 FGProcess (private)

 ******************************************************************************/

void
TestDirector::FGProcess
	(
	const JBoolean fixedLength
	)
{
	JProgressDisplay* pg = JNewPG();
	dynamic_cast<JXStandAlonePG*>(pg)->RaiseWhenUpdate();

	const JSize stepCount = 100;
	if (fixedLength)
		{
		pg->FixedLengthProcessBeginning(
			stepCount, JGetString("ProgressMessage::TestDirector"), kJTrue, kJFalse);
		}
	else
		{
		pg->VariableLengthProcessBeginning(
			JGetString("ProgressMessage::TestDirector"), kJTrue, kJFalse);
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
	const JBoolean fixedLength
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
