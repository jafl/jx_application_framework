/******************************************************************************
 TestDirector.h

	Interface for the TestDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestDirector
#define _H_TestDirector

#include <jx-af/jx/JXWindowDirector.h>

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

	TestDirector(JXDirector* supervisor, const bool isMaster,
				 const bool startIconic = false,
				 const bool bufferTestWidget = true,
				 const bool testWidgetIsImage = false,
				 const bool snoopWindow = false);

	virtual ~TestDirector();

	void	OpenTextFile(const JString& fileName);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool			itsIsMasterFlag;	// true if we were the first one created
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
	JXDisplayMenu*	itsDisplayMenu;		// can be nullptr
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

	void	BuildWindow(const bool isMaster, const bool bufferTestWidget,
						const bool testWidgetIsImage);
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

	void	FGProcess(const bool fixedLength);
	void	BeginBGProcess(const bool fixedLength);

	void	TestFontSubstitutionTiming();

	void	UpdateIconMenu();
	void	HandleIconMenu(const JIndex item);
};

#endif
