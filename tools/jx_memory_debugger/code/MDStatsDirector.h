/******************************************************************************
 MDStatsDirector.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDStatsDirector
#define _H_MDStatsDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JMemoryManager.h>

class JProcess;
class JXTextMenu;
class JXTextButton;
class JXToolBar;
class JXStaticText;
class JXInputField;
class JXFileInput;
class JXTimerTask;
class JXPGMessageDirector;
class MDLinkAcceptor;
class MDSizeHistogram;
class MDFilterRecordsDialog;

class MDStatsDirector : public JXWindowDirector, public JPrefObject
{
public:

	MDStatsDirector(JXDirector* supervisor);

	virtual	~MDStatsDirector();

	void	SetLink(JMemoryManager::DebugLink* link);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JString						itsSocketName;
	MDLinkAcceptor*				itsAcceptor;
	JMemoryManager::DebugLink*	itsLink;
	JProcess*					itsProcess;
	JXTimerTask*				itsPingTask;
	JXPGMessageDirector*		itsMessageDir;
	JString						itsExitStatsFile;

	MDSizeHistogram*		itsAllocatedHisto;
	MDFilterRecordsDialog*	itsRequestRecordsDialog;

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
	void	CloseLink(const JBoolean deleteProcess);
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
	void	ReceiveRecords(std::istream& input, const JCharacter* windowTitle);
	void	ReceiveErrorMessage(std::istream& input);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	// not allowed

	MDStatsDirector(const MDStatsDirector& source);
	const MDStatsDirector& operator=(const MDStatsDirector& source);
};

#endif
