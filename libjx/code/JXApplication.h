/******************************************************************************
 JXApplication.h

	Interface for the JXApplication class

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXApplication
#define _H_JXApplication

#include "jx-af/jx/JXDirector.h"
#include "jx-af/jx/JXDocumentManager.h"	// need definition of SafetySaveReason
#include <jx-af/jcore/JPtrArray-JString.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <mutex>

class JXWindow;
class JXDisplay;
class JXIdleTask;
class JXUrgentTask;

class JXApplication : public JXDirector
{
public:

	JXApplication(int* argc, char* argv[], const JUtf8Byte* appSignature,
				  const JUtf8Byte** defaultStringData);

	virtual ~JXApplication();

	const JString&	GetSignature() const;

	void			Run();
	void	Suspend() override;
	void	Resume() override;
	virtual void	Quit();
	bool			IsQuitting() const;

	void	DisplayBusyCursor();
	void	DisplayInactiveCursor();

	void	HideAllWindows();

	Time	GetCurrentTime() const;

	void	InstallIdleTask(JXIdleTask* newTask);
	void	RemoveIdleTask(JXIdleTask* task);

	void	InstallUrgentTask(JXUrgentTask* newTask);
	void	RemoveUrgentTask(JXUrgentTask* task);

	JXDisplay*	GetCurrentDisplay() const;
	void		SetCurrentDisplay(const JIndex index);
	void		SetCurrentDisplay(JXDisplay* display);

	JSize		GetDisplayCount() const;
	JXDisplay*	GetDisplay(const JIndex index) const;
	bool		GetDisplayIndex(JXDisplay* display, JIndex* index) const;

	const JPtrArray<JXDisplay>*	GetDisplayList() const;

	bool	OpenDisplay(const JString& displayName, JIndex* displayIndex);
	bool	DisplayExists(const Display* xDisplay);
	bool	FindDisplay(const Display* xDisplay, JXDisplay** display);

	// for use by networking clean-up code

	static void	CheckACEReactor();

	// for use by main()

	static void	RemoveCmdLineOption(int* argc, char* argv[],
									const unsigned long offset,
									const unsigned long removeCount);

	// called by JXDisplay

	void	DisplayOpened(JXDisplay* display);
	void	DisplayClosed(JXDisplay* display);

	// called by JXWindow

	const JString&	GetRestartCommand() const;

	// for use by special windows that block until dismissed

	bool	HasBlockingWindow() const;
	bool	HadBlockingWindow() const;

	// required by JXInitGlobals()

	static int	JXIOErrorHandler(Display* xDisplay);

	// called by JXAssert

	static void	Abort(const JXDocumentManager::SafetySaveReason reason,
					  const bool dumpCore);

	// called by JXMDIServer

	static void	StripBaseOptions(JPtrArray<JString>* argList);

protected:

	void			UpdateCurrentTime();
	virtual bool	HandleCustomEvent();
	virtual bool	HandleCustomEventWhileBlocking();

	bool	Close() override;	// use Quit() instead

	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	typedef JPtrArray< JPtrArray<JXIdleTask> >	IdleTaskStack;

private:

	JPtrArray<JXDisplay>*	itsDisplayList;
	JIndex					itsCurrDisplayIndex;
	bool					itsIgnoreDisplayDeletedFlag;
	bool					itsIgnoreTaskDeletedFlag;

	Time					itsCurrentTime;			// in milliseconds
	JPtrArray<JXIdleTask>*	itsIdleTasks;
	Time					itsMaxSleepTime;		// in milliseconds
	Time					itsLastIdleTime;		// in milliseconds
	Time					itsLastIdleTaskTime;	// in milliseconds
	JSize					itsWaitForChildCounter;
	IdleTaskStack*			itsIdleTaskStack;

	JPtrArray<JXUrgentTask>*	itsUrgentTasks;
	JPtrArray<JXUrgentTask>*	itsRunningUrgentTasks;	// usually nullptr; not owned
	bool						itsHasBlockingWindowFlag;
	bool						itsHadBlockingWindowFlag;

	std::recursive_mutex*	itsTaskMutex;

	JString	itsSignature;
	JString	itsRestartCmd;		// for session managers
	bool	itsRequestQuitFlag;

private:

	void	HandleOneEvent();
	void	PerformTasks(const bool hadEvents, const bool allowSleep);
	void	PerformIdleTasks();
	void	PerformUrgentTasks();

	void	PushIdleTaskStack();
	void	PopIdleTaskStack();
	void	PopAllIdleTaskStack();

	static void	ParseBaseOptions(int* argc, char* argv[], JString* displayName);

	static Bool	GetNextWindowEvent(Display* display, XEvent* event, char* arg);
	static Bool	GetNextBkgdEvent(Display* display, XEvent* event, char* arg);
	static Bool	DiscardNextEvent(Display* display, XEvent* event, char* arg);

	// for use by special windows that block until dismissed

	void	PrepareForBlockingWindow();
	void	BlockingWindowFinished();
	bool	HandleOneEventForWindow(JXWindow* window,
									const bool allowSleep = true);

	friend class JXCSFDialogBase;
	friend class JXChooseSaveFile;
	friend class JXUserNotification;
	friend class JXStandAlonePG;
	friend class JXDNDManager;
	friend class JXWindow;
};


/******************************************************************************
 GetCurrentTime

 ******************************************************************************/

inline Time
JXApplication::GetCurrentTime()
	const
{
	return itsCurrentTime;
}

/******************************************************************************
 GetCurrentDisplay

 ******************************************************************************/

inline JXDisplay*
JXApplication::GetCurrentDisplay()
	const
{
	return itsDisplayList->GetElement(itsCurrDisplayIndex);
}

/******************************************************************************
 GetDisplayCount

 ******************************************************************************/

inline JSize
JXApplication::GetDisplayCount()
	const
{
	return itsDisplayList->GetElementCount();
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXApplication::GetDisplay
	(
	const JIndex index
	)
	const
{
	return itsDisplayList->GetElement(index);
}

/******************************************************************************
 GetDisplayIndex

 ******************************************************************************/

inline bool
JXApplication::GetDisplayIndex
	(
	JXDisplay*	display,
	JIndex*		index
	)
	const
{
	return itsDisplayList->Find(display, index);
}

/******************************************************************************
 GetDisplayList

 ******************************************************************************/

inline const JPtrArray<JXDisplay>*
JXApplication::GetDisplayList()
	const
{
	return itsDisplayList;
}

/******************************************************************************
 DisplayExists

	Returns true if the given display hasn't been deleted.

 ******************************************************************************/

inline bool
JXApplication::DisplayExists
	(
	const Display* xDisplay
	)
{
	JXDisplay* d;
	return FindDisplay(xDisplay, &d);
}

/******************************************************************************
 Blocking window

 ******************************************************************************/

inline bool
JXApplication::HasBlockingWindow()
	const
{
	return itsHasBlockingWindowFlag;
}

inline bool
JXApplication::HadBlockingWindow()
	const
{
	return itsHadBlockingWindowFlag;
}

// private

inline void
JXApplication::PrepareForBlockingWindow()
{
	PushIdleTaskStack();
}

inline void
JXApplication::BlockingWindowFinished()
{
	PopIdleTaskStack();
}

/******************************************************************************
 GetSignature

 ******************************************************************************/

inline const JString&
JXApplication::GetSignature()
	const
{
	return itsSignature;
}

/******************************************************************************
 GetRestartCommand

	Required for X session managers.  It is assumed that programs restore
	their state when run with no arguments.

 ******************************************************************************/

inline const JString&
JXApplication::GetRestartCommand()
	const
{
	return itsRestartCmd;
}

/******************************************************************************
 IsQuitting

 ******************************************************************************/

inline bool
JXApplication::IsQuitting()
	const
{
	return itsRequestQuitFlag;
}

#endif
