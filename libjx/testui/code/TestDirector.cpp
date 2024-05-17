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

	itsPSPrinter  = jnew JXPSPrinter(GetDisplay());
	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
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

#include "TestDirector-About.h"
#include "TestDirector-PrintPostscript.h"
#include "TestDirector-Test.h"
#include "TestDirector-UserNotification.h"
#include "TestDirector-ChooseSaveFile.h"
#include "TestDirector-ProgressDisplay.h"

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

	auto* window = jnew JXWindow(this, 400,330, JGetString("WindowTitle::TestDirector::JXLayout"));
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestDirector");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

	itsWidget =
		jnew TestWidget(isMaster, testWidgetIsImage, menuBar, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 400,300);

// end JXLayout

	if (isMaster)
	{
		window->SetCloseAction(JXWindow::kQuitApp);
	}
	else
	{
		window->SetCloseAction(JXWindow::kCloseDisplay);
	}

	itsAnimIconTask = jnew AnimateWindowIconTask(GetWindow());
	ListenTo(window, std::function([this](const JXWindow::Iconified&)
	{
		itsAnimIconTask->Start();
	}));
	ListenTo(window, std::function([this](const JXWindow::Deiconified&)
	{
		itsAnimIconTask->Stop();
	}));

	itsWidget->SetSingleFocusWidget();

	// menus

	if (isMaster)
	{
		itsDisplayMenu =
			jnew JXDisplayMenu(JGetString("DisplayMenuTitle::TestDirector"), menuBar,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  0,0, 10,10);
		menuBar->PrependMenu(itsDisplayMenu);
	}
	else
	{
		itsDisplayMenu = nullptr;
	}

	itsTestMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::TestDirector_Test"));
	itsTestMenu->SetMenuItems(kTestMenuStr);
	itsTestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTestMenu->AttachHandlers(this,
		&TestDirector::UpdateTestMenu,
		&TestDirector::HandleTestMenu);
	ConfigureTestMenu(itsTestMenu);

	itsUNMenu = jnew JXTextMenu(itsTestMenu, kTestUserNotifyMenuCmd, menuBar);
	itsUNMenu->SetMenuItems(kUserNotificationMenuStr);
	itsUNMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsUNMenu->AttachHandlers(this,
		&TestDirector::UpdateUNMenu,
		&TestDirector::HandleUNMenu);
	ConfigureUserNotificationMenu(itsUNMenu);

	itsCSFMenu = jnew JXTextMenu(itsTestMenu, kTestChooseSaveFileMenuCmd, menuBar);
	itsCSFMenu->SetMenuItems(kChooseSaveFileMenuStr);
	itsCSFMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsCSFMenu->AttachHandlers(this,
		&TestDirector::UpdateCSFMenu,
		&TestDirector::HandleCSFMenu);
	ConfigureChooseSaveFileMenu(itsCSFMenu);

	itsPGMenu = jnew JXTextMenu(itsTestMenu, kTestPGMenuCmd, menuBar);
	itsPGMenu->SetMenuItems(kProgressDisplayMenuStr);
	itsPGMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPGMenu->AttachHandlers(this,
		&TestDirector::UpdatePGMenu,
		&TestDirector::HandlePGMenu);
	ConfigureProgressDisplayMenu(itsPGMenu);

	JXImage* aboutTitleImage =
		jnew JXImage(display, kSmileyBitmap[ kHappySmileyIndex ], JColorManager::GetRedColor());
	itsAboutMenu = menuBar->PrependTextMenu(aboutTitleImage, true);
	itsAboutMenu->SetMenuItems(kAboutMenuStr);
	itsAboutMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsAboutMenu->AttachHandlers(this,
		&TestDirector::UpdateAboutMenu,
		&TestDirector::HandleAboutMenu);
	ConfigureAboutMenu(itsAboutMenu);

	itsAnimHelpTask = jnew AnimateHelpMenuTask(itsAboutMenu, kHelpCmd);

	itsPrintPSMenu = jnew JXTextMenu(itsAboutMenu, kPrintPSMenuCmd, menuBar);
	itsPrintPSMenu->SetMenuItems(kPrintPostscriptMenuStr);
	itsPrintPSMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrintPSMenu->AttachHandlers(this,
		&TestDirector::UpdatePrintPSMenu,
		&TestDirector::HandlePrintPSMenu);
	ConfigurePrintPostscriptMenu(itsPrintPSMenu);

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
	}

	// create 1x6 menu in menu bar -- this owns the icons

	itsSmileyMenu =
		jnew JXImageMenu(JGetString("IconMenuTitle::TestDirector"), 6, menuBar,
						 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	itsSmileyMenu->SetUpdateAction(JXMenu::kDisableNone);
	menuBar->AppendMenu(itsSmileyMenu);

	for (auto* i : image)
	{
		itsSmileyMenu->AppendItem(i, true);
	}

	// create 2x2 submenu of radio buttons

	itsIconMenu = jnew JXImageMenu(2, itsSmileyMenu, 2, menuBar);
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);

	for (auto* i : image)
	{
		itsIconMenu->AppendItem(i, false, JXMenu::kRadioType);
	}

	itsIconMenuItem = 1;

	itsIconMenu->AttachHandlers(this,
		&TestDirector::UpdateIconMenu,
		&TestDirector::HandleIconMenu);

	// create 3x5 submenu that has a few unused cells

	JXImageMenu* submenu = jnew JXImageMenu(5, itsSmileyMenu, 4, menuBar);
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
		dir->Activate();
	}

	else if (index == kTestInputCmd)
	{
		TestInputFieldsDialog* dir = jnew TestInputFieldsDialog();
		dir->DoDialog();
	}
	else if (index == kTestButtonsCmd)
	{
		TestButtonsDialog* dir = jnew TestButtonsDialog();
		dir->DoDialog();
	}
	else if (index == kTestPopupChoiceCmd)
	{
		TestPopupChoiceDialog* dir = jnew TestPopupChoiceDialog();
		dir->DoDialog();
	}
	else if (index == kTestSliderCmd)
	{
		TestSliderDirector* dir = jnew TestSliderDirector(this);
		dir->Activate();
	}

	else if (index == kTestPartitionsCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestPartitionDirector* dir = jnew TestPartitionDirector(this);
		dir->Activate();
	}
	else if (index == kTestTabGroupCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestTabDirector* dir = jnew TestTabDirector(this);
		dir->Activate();
	}

	else if (index == kTestStrTableCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestStrTableDirector* dir = jnew TestStrTableDirector(this);
		dir->Activate();
	}
	else if (index == kTestNumTableCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestFloatTableDirector* dir = jnew TestFloatTableDirector(this);
		dir->Activate();
	}

	else if (index == kTestTextEditorCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestTextEditDocument* doc = jnew TestTextEditDocument(this);
		doc->Activate();
	}

	else if (index == kTestLinkedDocCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestLinkedDocument* doc = jnew TestLinkedDocument(this);
		doc->Activate();
	}

	else if (index == kTestDNDTextCmd)
	{
		if (itsDisplayMenu != nullptr)
		{
			itsDisplayMenu->SelectCurrentDisplay();
		}
		TestDNDTextDirector* dir = jnew TestDNDTextDirector(this);
		dir->Activate();
	}

	else if (index == kTestImageViewCmd)
	{
		TestImageDirector* dir = jnew TestImageDirector(this);
		dir->Activate();
	}

	else if (index == kTestFileListCmd)
	{
		TestFileListDirector* dir = jnew TestFileListDirector(this);
		dir->Activate();
	}

	else if (index == kSendEmailCmd)
	{
		JXGetWebBrowser()->ShowURL("mailto:me@example.com");
	}
	else if (index == kShowFileContentCmd)
	{
		JXGetWebBrowser()->ShowURL("file:/etc/hosts");
	}
	else if (index == kShowWebPageCmd)
	{
		JXGetWebBrowser()->ShowURL("http://example.com");
	}

	else if (index == kTestDisabledMenuCmd)
	{
		itsSmileyMenu->SetActive(!itsSmileyMenu->IsActive());
	}

	else if (index == kTestZombieProcessCmd)
	{
		pid_t pid;
		std::cout << std::endl;
		const JError err = JExecute("ls", &pid);
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
		auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, "./about_owner_docs", JString::empty, JGetString("CustomChooseFileInstructions::TestDirector"));
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
	dynamic_cast<JXStandAlonePG&>(*pg).RaiseWhenUpdate();

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
