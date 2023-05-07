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
class JXFunctionTask;
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

private:

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JString						itsSocketName;
	LinkAcceptor*				itsAcceptor;
	JMemoryManager::DebugLink*	itsLink;
	JProcess*					itsProcess;
	JXFunctionTask*				itsPingTask;
	JXPGMessageDirector*		itsMessageDir;
	JString						itsExitStatsFile;

	SizeHistogram*			itsAllocatedHisto;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

// begin statsLayout

	JXFileInput*  itsProgramInput;
	JXTextButton* itsChooseProgramButton;
	JXInputField* itsArgsInput;
	JXTextButton* itsRunProgramButton;
	JXStaticText* itsAllocatedBlocksDisplay;
	JXStaticText* itsAllocatedBytesDisplay;
	JXStaticText* itsDeallocatedBlocksDisplay;

// end statsLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();

	void	OpenDebugAcceptor();
	void	DeleteDebugAcceptor();
	void	CloseLink(const bool deleteProcess);
	void	SendRequest(std::ostringstream& data);
	void	HandleResponse();

	void	ChooseProgram();
	void	RunProgram();
	void	FinishProgram();

	void	RequestRunningStats();
	void	ReceiveRunningStats(std::istream& input);
	void	ReceiveExitStats(std::istream& input);
	void	ReadExitStats();
	void	RequestRecords(const JMemoryManager::RecordFilter& filter);
	void	ReceiveRecords(std::istream& input, const JString& windowTitle);
	void	ReceiveErrorMessage(std::istream& input);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);
};

#endif
