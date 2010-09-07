/******************************************************************************
 TestDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
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
#include "testjxHelpText.h"
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
#include <JXColormap.h>
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

static const JCharacter* kWindowGeomFileName = "testjx_window_geom";

// About menu

static const JCharacter* kAboutMenuShortcuts = "#?";
static const JCharacter* kAboutMenuStr =
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

static const JCharacter* kPrintPSMenuStr =
	"    Page setup..."
	"  | Print...      %h p %k Ctrl-P";

enum
{
	kPSPageSetupCmd = 1, kPrintPSCmd
};

// Test menu

static const JCharacter* kTestMenuTitleStr = "Test %h #t";
static const JCharacter* kTestMenuStr =
	"    New main window"
	"%l| User notification"
	"  | File chooser"
	"  | Progress display"
	"%l| Input fields"
	"  | Buttons"
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

static const JCharacter* kUserNotificationMenuStr =
	"Message|Warning|Error";

enum
{
	kTestMessageCmd = 1, kTestWarningCmd, kTestErrorCmd
};

// ChooseSaveFile menu

static const JCharacter* kChooseSaveFileMenuStr =
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

static const JCharacter* kProgressDisplayMenuStr =
	"    Fixed length"
	"  | Variable length"
	"%l| Fixed length (background)"
	"  | Variable length (background)";

enum
{
	kFixLenFGCmd = 1, kVarLenFGCmd, kFixLenBGCmd, kVarLenBGCmd
};

// Display menu

static const JCharacter* kDisplayMenuName = "Display";

// Icon menu

static const JCharacter* kIconMenuTitleStr = "Smileys";

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
	itsPSPrinter    = NULL;
	itsEPSPrinter   = NULL;

	itsCSF = new TestChooseSaveFile;
	assert( itsCSF != NULL );

	BuildWindow(isMaster, bufferTestWidget, testWidgetIsImage);

	JXWindow* window = GetWindow();
	if (snoopWindow)
		{
		itsWindowSnooper = new JBroadcastSnooper(window);
		assert( itsWindowSnooper != NULL );
		}
	else
		{
		itsWindowSnooper = NULL;
		}

	if (isMaster && JFileExists(kWindowGeomFileName))
		{
		ifstream input(kWindowGeomFileName);
		window->ReadGeometry(input);
		}

	if (startIconic)
		{
		window->Iconify();
		}

	// GetDisplay() only works after SetWindow()

	itsPSPrinter = new JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != NULL );
	ListenTo(itsPSPrinter);

	itsEPSPrinter = new JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != NULL );
	ListenTo(itsEPSPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestDirector::~TestDirector()
{
	ofstream output(kWindowGeomFileName);
	(GetWindow())->WriteGeometry(output);

	delete itsPSPrinter;
	delete itsEPSPrinter;
	delete itsCSF;
	delete itsAnimHelpTask;
	delete itsAnimIconTask;
	delete itsWindowSnooper;
}

/******************************************************************************
 OpenTextFile

 ******************************************************************************/

void
TestDirector::OpenTextFile
	(
	const JCharacter* fileName
	)
{
	if (JFileReadable(fileName))
		{
		TestTextEditDocument* doc = new TestTextEditDocument(this, fileName);
		assert( doc != NULL );
		doc->Activate();
		}
	else
		{
		JString msg = "You do not have permission to read \"";
		msg += fileName;
		msg += "\".";
		(JGetUserNotification())->ReportError(msg);
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
	JXDisplay* display   = (JXGetApplication())->GetCurrentDisplay();
	JXColormap* colormap = display->GetColormap();

// begin JXLayout

    JXWindow* window = new JXWindow(this, 400,330, "");
    assert( window != NULL );

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
    assert( menuBar != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
    assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Test Director");
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

	itsAnimIconTask = new AnimateWindowIconTask(GetWindow());
	assert( itsAnimIconTask != NULL );
	ListenTo(window);		// for icon animation

	// menus

	JXImage* aboutTitleImage =
		new JXImage(display, kSmileyBitmap[ kHappySmileyIndex ], colormap->GetRedColor());
	assert( aboutTitleImage != NULL );
	itsAboutMenu = menuBar->AppendTextMenu(aboutTitleImage, kJTrue);
	itsAboutMenu->SetShortcuts(kAboutMenuShortcuts);
	itsAboutMenu->SetMenuItems(kAboutMenuStr, "TestDirector");
	itsAboutMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsAboutMenu);

	itsAnimHelpTask = new AnimateHelpMenuTask(itsAboutMenu, kHelpCmd);
	assert( itsAnimHelpTask != NULL );

	itsPrintPSMenu = new JXTextMenu(itsAboutMenu, kPrintPSMenuCmd, menuBar);
	assert( itsPrintPSMenu != NULL );
	itsPrintPSMenu->SetMenuItems(kPrintPSMenuStr);
	itsPrintPSMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrintPSMenu);

	itsTestMenu = menuBar->AppendTextMenu(kTestMenuTitleStr);
	itsTestMenu->SetMenuItems(kTestMenuStr);
	itsTestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTestMenu);

	itsUNMenu = new JXTextMenu(itsTestMenu, kTestUserNotifyMenuCmd, menuBar);
	assert( itsUNMenu != NULL );
	itsUNMenu->SetMenuItems(kUserNotificationMenuStr);
	itsUNMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsUNMenu);

	itsCSFMenu = new JXTextMenu(itsTestMenu, kTestChooseSaveFileMenuCmd, menuBar);
	assert( itsCSFMenu != NULL );
	itsCSFMenu->SetMenuItems(kChooseSaveFileMenuStr);
	itsCSFMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsCSFMenu);

	itsPGMenu = new JXTextMenu(itsTestMenu, kTestPGMenuCmd, menuBar);
	assert( itsPGMenu != NULL );
	itsPGMenu->SetMenuItems(kProgressDisplayMenuStr);
	itsPGMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPGMenu);

	if (isMaster)
		{
		itsDisplayMenu =
			new JXDisplayMenu(kDisplayMenuName, menuBar,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  0,0, 10,10);
		assert( itsDisplayMenu != NULL );
		menuBar->AppendMenu(itsDisplayMenu);
		}
	else
		{
		itsDisplayMenu = NULL;
		}

	itsWidget =
		new TestWidget(isMaster, testWidgetIsImage,
					   menuBar, scrollbarSet,
					   scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic,
					   0,0, 10,10);
	assert( itsWidget != NULL );
	itsWidget->FitToEnclosure(kJTrue, kJTrue);
	itsWidget->SetSingleFocusWidget();

	BuildIconMenus(window, menuBar);

	// do this -after- constructing JXScrollableWidget

	(GetWindow())->BufferDrawing(bufferTestWidget);
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

	JXDisplay* display   = window->GetDisplay();
	JXColormap* colormap = window->GetColormap();

	const JColorIndex kSmileyColor[] =
	{
		colormap->GetWhiteColor(),
		colormap->GetRedColor(),
		colormap->GetBlueColor(),
		colormap->GetBlackColor()
	};

	JXImage* image[kSmileyBitmapCount];
	for (i=0; i<kSmileyBitmapCount; i++)
		{
		image[i] = new JXImage(display, kSmileyBitmap[i], kSmileyColor[i]);
		assert( image[i] != NULL );
		}

	// create 1x6 menu in menu bar -- this owns the icons

	itsSmileyMenu =
		new JXImageMenu(kIconMenuTitleStr, 6, menuBar,
						JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSmileyMenu != NULL );
	itsSmileyMenu->SetUpdateAction(JXMenu::kDisableNone);
	menuBar->AppendMenu(itsSmileyMenu);

	for (i=0; i<kSmileyBitmapCount; i++)
		{
		itsSmileyMenu->AppendItem(image[i], kJTrue);
		}

	// create 2x2 submenu of radio buttons

	itsIconMenu = new JXImageMenu(2, itsSmileyMenu, 2, menuBar);
	assert( itsIconMenu != NULL );
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);

	for (i=0; i<kSmileyBitmapCount; i++)
		{
		itsIconMenu->AppendItem(image[i], kJFalse, JXMenu::kRadioType);
		}

	itsIconMenuItem = 1;
	ListenTo(itsIconMenu);

	// create 3x5 submenu that has a few unused cells

	JXImageMenu* submenu = new JXImageMenu(5, itsSmileyMenu, 4, menuBar);
	assert( submenu != NULL );
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
	JXWindow* window = GetWindow();		// insure that it isn't const

	if (sender == itsAboutMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateAboutMenu();
		}
	else if (sender == itsAboutMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleAboutMenu(selection->GetIndex());
		}

	else if (sender == itsPrintPSMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrintPSMenu();
		}
	else if (sender == itsPrintPSMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePrintPSMenu(selection->GetIndex());
		}

	else if (sender == itsTestMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTestMenu();
		}
	else if (sender == itsTestMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleTestMenu(selection->GetIndex());
		}

	else if (sender == itsUNMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateUNMenu();
		}
	else if (sender == itsUNMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleUNMenu(selection->GetIndex());
		}

	else if (sender == itsCSFMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCSFMenu();
		}
	else if (sender == itsCSFMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleCSFMenu(selection->GetIndex());
		}

	else if (sender == itsPGMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePGMenu();
		}
	else if (sender == itsPGMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePGMenu(selection->GetIndex());
		}

	else if (sender == itsIconMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateIconMenu();
		}
	else if (sender == itsIconMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
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
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsWidget->Print(*itsPSPrinter);
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsWidget->Print(*itsEPSPrinter);
			}
		}

	else if (message.Is(JXTipOfTheDayDialog::kShowAtStartup))
		{
		const JXTipOfTheDayDialog::ShowAtStartup* info =
			dynamic_cast(const JXTipOfTheDayDialog::ShowAtStartup*, &message);
		assert( info != NULL );
		cout << "Should show at startup: " << info->ShouldShowAtStartup() << endl;
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
		(JXGetHelpManager())->ShowSection(kMainHelpName);
		}
	else if (index == kTipCmd)
		{
		JXTipOfTheDayDialog* dlog = new JXTipOfTheDayDialog(kJTrue, kJFalse);
		assert( dlog != NULL );
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
		(GetDisplay())->Close();
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
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestDirector* dir = new TestDirector(TestjxGetApplication(), kJFalse);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestInputCmd)
		{
		TestInputFieldsDialog* dir = new TestInputFieldsDialog(this);
		assert( dir != NULL );
		dir->BeginDialog();
		}
	else if (index == kTestButtonsCmd)
		{
		TestButtonsDialog* dir = new TestButtonsDialog(this);
		assert( dir != NULL );
		dir->BeginDialog();
		}
	else if (index == kTestPopupChoiceCmd)
		{
		TestPopupChoiceDialog* dir = new TestPopupChoiceDialog(this);
		assert( dir != NULL );
		dir->BeginDialog();
		}
	else if (index == kTestSliderCmd)
		{
		TestSliderDirector* dir = new TestSliderDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestPartitionsCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestPartitionDirector* dir = new TestPartitionDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kTestTabGroupCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestTabDirector* dir = new TestTabDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestStrTableCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestStrTableDirector* dir = new TestStrTableDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}
	else if (index == kTestNumTableCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestFloatTableDirector* dir = new TestFloatTableDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestTextEditorCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestTextEditDocument* doc = new TestTextEditDocument(this);
		assert( doc != NULL );
		doc->Activate();
		}

	else if (index == kTestLinkedDocCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestLinkedDocument* doc = new TestLinkedDocument(this);
		assert( doc != NULL );
		doc->Activate();
		}

	else if (index == kTestDNDTextCmd)
		{
		if (itsDisplayMenu != NULL)
			{
			itsDisplayMenu->SelectCurrentDisplay();
			}
		TestDNDTextDirector* dir = new TestDNDTextDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestImageViewCmd)
		{
		TestImageDirector* dir = new TestImageDirector(this);
		assert( dir != NULL );
		dir->Activate();
		}

	else if (index == kTestDisabledMenuCmd)
		{
		itsSmileyMenu->SetActive(!itsSmileyMenu->IsActive());
		}

	else if (index == kTestZombieProcessCmd)
		{
		pid_t pid;
		cout << endl;
		const JError err = JExecute("ls", &pid);
		cout << endl;
		if (err.OK())
			{
			(JGetUserNotification())->DisplayMessage(
				"This should not leave a zombie process lying around "
				"because JXApplication is supposed to automatically "
				"clean it up.");
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

		cout << endl;
		cout << "Window is now at " << window->GetDesktopLocation() << endl;
		}
	else if (index == kTestPlaceWindow30Cmd)
		{
		JXWindow* window = GetWindow();
		window->Place(30,30);

		cout << endl;
		cout << "Window is now at " << window->GetDesktopLocation() << endl;
		}
	else if (index == kTestPlaceWindow100Cmd)
		{
		JXWindow* window = GetWindow();
		window->Place(100,100);

		cout << endl;
		cout << "Window is now at " << window->GetDesktopLocation() << endl;
		}
	else if (index == kTestMoveWindowCmd)
		{
		JXWindow* window = GetWindow();
		window->Move(10,10);

		cout << endl;
		cout << "Window is now at " << window->GetDesktopLocation() << endl;
		}
	else if (index == kRaiseAllWindowsCmd)
		{
		(GetDisplay())->RaiseAllWindows();
		}
	else if (index == kPrintWMConfigCmd)
		{
		(GetWindow())->PrintWindowConfig();
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
		(JGetUserNotification())->DisplayMessage("This is a test message.");
		}
	else if (index == kTestWarningCmd)
		{
		(JGetUserNotification())->AskUserYes("Did you like this warning message?");
		}
	else if (index == kTestErrorCmd)
		{
		(JGetUserNotification())->ReportError("This is a fake error.");
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
		ok = csf->ChooseFile("Name of file:",
			"This is a test of the dialog window that allows the user to"
			" choose a file to be opened.\n\n"
			"0123456789012345678901234567890123456789"
			"0123456789012345678901234567890123456789",
			&resultStr);
		}
	else if (index == kChooseFileCustomCmd)
		{
		csf = itsCSF;
		ok  = csf->ChooseFile("Name of file:",
			"This dialog only allows you to choose email files.",
			&resultStr);
		}
	else if (index == kSaveFileCmd)
		{
		ok = csf->SaveFile("Save file as:",
			"This is a test of the dialog window that allows the user to"
			" save a file under a new name.", "Untitled", &resultStr);
		}
	else if (index == kSaveFileCustomCmd)
		{
		csf = itsCSF;
		ok  = csf->SaveFile("Save file as:",
			"This is a test of a custom dialog window that allows the user to"
			" save a file under a new name.", "Untitled", &resultStr);

		if (ok)
			{
			TestChooseSaveFile::SaveFormat format = itsCSF->GetSaveFormat();
			JString msg =
				(format == TestChooseSaveFile::kGIFFormat  ? "GIF"  :
				(format == TestChooseSaveFile::kPNGFormat  ? "PNG"  :
				(format == TestChooseSaveFile::kJPEGFormat ? "JPEG" : "unknown")));
			msg.Prepend("\n(");
			msg.Append(" format)");
			resultStr.Append(msg);
			}
		}
	else if (index == kChooseRPathCmd)
		{
		ok = csf->ChooseRPath("",
			"This is a test of the dialog window that allows the user to"
			" choose a readable directory.", NULL, &resultStr);
		}
	else if (index == kChooseRWPathCmd)
		{
		ok = csf->ChooseRWPath("",
			"This is a test of the dialog window that allows the user to"
			" choose a writable directory.", NULL, &resultStr);
		}

	if (ok)
		{
		const JString msg = "You selected:\n\n" + resultStr;
		(JGetUserNotification())->DisplayMessage(msg);
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
			stepCount, "Crunching a whole lot of very large numbers...", kJTrue, kJFalse);
		}
	else
		{
		pg->VariableLengthProcessBeginning(
			"Crunching a whole lot of very large numbers...", kJTrue, kJFalse);
		}

	for (JIndex i=1; i<=stepCount; i++)
		{
		JWait(0.5);		// simulate massive, greedy number crunching

		// update the display

		JString progressStr = i;
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
	TestPGTask* task = new TestPGTask(fixedLength);
	assert( task != NULL );
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
