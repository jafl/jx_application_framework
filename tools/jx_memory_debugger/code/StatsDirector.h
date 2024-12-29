/******************************************************************************
 StatsDirector.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_StatsDirector
#define _H_StatsDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JMemoryManager.h>

class JProcess;
class JXTextMenu;
class JXTextButton;
class JXToolBar;
class JXStaticText;
class JXInputField;
class JXFileInput;
class JXIdleTask;
class JXPGMessageDirector;
class LinkAcceptor;
class SizeHistogram;

class StatsDirector : public JXWindowDirector, public JPrefObject
{
public:

	StatsDirector(JXDirector* supervisor);

	~StatsDirector() override;

	void	SetLink(JMemoryManager::DebugLink* link);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsDataMenu;
	JXTextMenu*	itsPrefsMenu;

	JString						itsSocketName;
	LinkAcceptor*				itsAcceptor;
	JMemoryManager::DebugLink*	itsLink;
	JProcess*					itsProcess;
	JXIdleTask*					itsPingTask;
	JXPGMessageDirector*		itsMessageDir;

	JMemoryManager::RecordFilter	itsDataFilter;

// begin JXLayout

	JXToolBar*     itsToolBar;
	JXTextButton*  itsChooseProgramButton;
	JXTextButton*  itsRunProgramButton;
	JXStaticText*  itsAllocatedBlocksDisplay;
	JXStaticText*  itsAllocatedBytesDisplay;
	JXStaticText*  itsDeallocatedBlocksDisplay;
	SizeHistogram* itsAllocatedHisto;
	JXFileInput*   itsProgramInput;
	JXInputField*  itsArgsInput;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();

	void	OpenDebugAcceptor();
	void	DeleteDebugAcceptor();
	void	CloseLink();
	void	SendRequest(std::ostringstream& data);
	void	HandleResponse();

	void	ChooseProgram();
	void	RunProgram();
	void	FinishProgram();

	void	RequestRunningStats();
	void	ReceiveRunningStats(std::istream& input);
	void	ReceiveExitStats(std::istream& input);
	void	ReadExitStats();
	void	SendRequest(const JIndex cmd);
	void	ReceiveRecords(std::istream& input, const JString& windowTitle,
						   const bool showIfEmpty);
	void	ReceiveErrorMessage(std::istream& input);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateDataMenu();
	void	HandleDataMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
};

#endif
