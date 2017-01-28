/******************************************************************************
 JXApplication.h

	Interface for the JXApplication class

	Copyright (C) 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXApplication
#define _H_JXApplication

#include <JXDirector.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason
#include <JPtrArray-JString.h>
#include <JXDisplay.h>			// for Windows template compile
#include <JXIdleTask.h>			// for Windows template compile
#include <JXUrgentTask.h>		// for Windows template compile
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXWindow;

class JXApplication : public JXDirector
{
public:

	JXApplication(int* argc, char* argv[], const JCharacter* appSignature,
				  const JCharacter** defaultStringData);

	virtual ~JXApplication();

	const JString&	GetSignature() const;

	void			Run();
	virtual void	Suspend();
	virtual void	Resume();
	virtual void	Quit();
	JBoolean		IsQuitting() const;

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
	JBoolean	GetDisplayIndex(JXDisplay* display, JIndex* index) const;

	const JPtrArray<JXDisplay>*	GetDisplayList() const;

	JBoolean	OpenDisplay(const JCharacter* displayName, JIndex* displayIndex);
	JBoolean	DisplayExists(const Display* xDisplay);
	JBoolean	FindDisplay(const Display* xDisplay, JXDisplay** display);

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

	JBoolean	HasBlockingWindow() const;
	JBoolean	HadBlockingWindow() const;

	// required by JXInitGlobals()

	static int	JXIOErrorHandler(Display* xDisplay);

	// called by JXAssert

	static void	Abort(const JXDocumentManager::SafetySaveReason reason,
					  const JBoolean dumpCore);

	// called by JXMDIServer

	static void	StripBaseOptions(JPtrArray<JString>* argList);

protected:

	void				UpdateCurrentTime();
	virtual JBoolean	HandleCustomEvent();
	virtual JBoolean	HandleCustomEventWhileBlocking();

	virtual JBoolean	Close();	// use Quit() instead

	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

private:

	typedef JPtrArray< JPtrArray<JXIdleTask> >	IdleTaskStack;

private:

	JPtrArray<JXDisplay>*	itsDisplayList;
	JIndex					itsCurrDisplayIndex;
	JBoolean				itsIgnoreDisplayDeletedFlag;
	JBoolean				itsIgnoreTaskDeletedFlag;

	Time					itsCurrentTime;			// in milliseconds
	JPtrArray<JXIdleTask>*	itsIdleTasks;
	Time					itsMaxSleepTime;		// in milliseconds
	Time					itsLastIdleTime;		// in milliseconds
	Time					itsLastIdleTaskTime;	// in milliseconds
	JSize					itsWaitForChildCounter;
	IdleTaskStack*			itsIdleTaskStack;

	JPtrArray<JXUrgentTask>*	itsUrgentTasks;
	JPtrArray<JXUrgentTask>*	itsRunningUrgentTasks;	// usually NULL; not owned
	JBoolean					itsHasBlockingWindowFlag;
	JBoolean					itsHadBlockingWindowFlag;

	JString		itsSignature;
	JString		itsRestartCmd;		// for session managers
	JBoolean	itsRequestQuitFlag;

private:

	void	HandleOneEvent();
	void	PerformTasks(const JBoolean hadEvents, const JBoolean allowSleep);
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

	void		PrepareForBlockingWindow();
	void		BlockingWindowFinished();
	JBoolean	HandleOneEventForWindow(JXWindow* window,
										const JBoolean allowSleep = kJTrue);

	friend class JXCSFDialogBase;
	friend class JXChooseSaveFile;
	friend class JXUserNotification;
	friend class JXStandAlonePG;
	friend class JXDNDManager;
	friend class JXWindow;

	// not allowed

	JXApplication(const JXApplication& source);
	const JXApplication& operator=(const JXApplication& source);
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
	return itsDisplayList->NthElement(itsCurrDisplayIndex);
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
	return itsDisplayList->NthElement(index);
}

/******************************************************************************
 GetDisplayIndex

 ******************************************************************************/

inline JBoolean
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

	Returns kJTrue if the given display hasn't been deleted.

 ******************************************************************************/

inline JBoolean
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

inline JBoolean
JXApplication::HasBlockingWindow()
	const
{
	return itsHasBlockingWindowFlag;
}

inline JBoolean
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

inline JBoolean
JXApplication::IsQuitting()
	const
{
	return itsRequestQuitFlag;
}

#endif
