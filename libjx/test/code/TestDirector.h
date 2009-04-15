/******************************************************************************
 TestDirector.h

	Interface for the TestDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDirector
#define _H_TestDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class TestWidget;
class TestChooseSaveFile;
class AnimateHelpMenuTask;
class AnimateWindowIconTask;
class JBroadcastSnooper;

class JXHelpDirector;
class JXMenuBar;
class JXTextMenu;
class JXImageMenu;
class JXDisplayMenu;
class JXPSPrinter;
class JXEPSPrinter;

class TestDirector : public JXWindowDirector
{
public:

	TestDirector(JXDirector* supervisor, const JBoolean isMaster,
				 const JBoolean startIconic = kJFalse,
				 const JBoolean bufferTestWidget = kJTrue,
				 const JBoolean testWidgetIsImage = kJFalse,
				 const JBoolean useStaticColors = kJFalse,
				 const JBoolean allocDynamicColors = kJTrue,
				 const JBoolean snoopWindow = kJFalse);

	virtual ~TestDirector();

	void	OpenTextFile(const JCharacter* fileName);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean		itsIsMasterFlag;	// kJTrue if we were the first one created
	JXPSPrinter*	itsPSPrinter;
	JXEPSPrinter*	itsEPSPrinter;
	TestWidget*		itsWidget;			// owned by its enclosure

	JIndex	itsIconMenuItem;

	// owned by the menu bar

	JXTextMenu*		itsAboutMenu;
	JXTextMenu*		itsPrintPSMenu;
	JXTextMenu*		itsTestMenu;
	JXTextMenu*		itsUNMenu;
	JXTextMenu*		itsCSFMenu;
	JXTextMenu*		itsPGMenu;
	JXDisplayMenu*	itsDisplayMenu;		// can be NULL
	JXImageMenu*	itsSmileyMenu;
	JXImageMenu*	itsIconMenu;

	// we own these

	TestChooseSaveFile*		itsCSF;
	AnimateHelpMenuTask*	itsAnimHelpTask;
	AnimateWindowIconTask*	itsAnimIconTask;
	JBroadcastSnooper*		itsWindowSnooper;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JBoolean isMaster, const JBoolean bufferTestWidget,
						const JBoolean testWidgetIsImage,
						const JBoolean useStaticColors,
						const JBoolean allocDynamicColors);
	void	BuildIconMenus(JXWindow* window, JXMenuBar* menuBar);

	void	UpdateAboutMenu();
	void	HandleAboutMenu(const JIndex item);

	void	UpdatePrintPSMenu();
	void	HandlePrintPSMenu(const JIndex item);

	void	UpdateTestMenu();
	void	HandleTestMenu(const JIndex item);

	void	UpdateUNMenu();
	void	HandleUNMenu(const JIndex item);

	void	UpdateCSFMenu();
	void	HandleCSFMenu(const JIndex item);

	void	UpdatePGMenu();
	void	HandlePGMenu(const JIndex item);

	void	FGProcess(const JBoolean fixedLength);
	void	BeginBGProcess(const JBoolean fixedLength);

	void	UpdateIconMenu();
	void	HandleIconMenu(const JIndex item);

	// not allowed

	TestDirector(const TestDirector& source);
	const TestDirector& operator=(const TestDirector& source);
};

#endif
