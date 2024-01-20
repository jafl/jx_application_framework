/******************************************************************************
 JXApplication.h

	Interface for the JXApplication class

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXApplication
#define _H_JXApplication

#include "JXDirector.h"
#include "JXDocumentManager.h"	// for SafetySaveReason
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
	friend class JXWindow;

public:

	enum FiberPriority
	{
		kEventLoopPriority,
		kEventHandlerPriority,
		kIdleTaskPriority,
		kUrgentTaskPriority
	};

public:

	JXApplication(int* argc, char* argv[],
				  const JUtf8Byte* appSignature, const JUtf8Byte* wmName,
				  const JUtf8Byte** defaultStringData);

	~JXApplication() override;

	const JString&	GetSignature() const;
	const JString&	GetWMName() const;

	void			Run();
	void			Suspend() override;
	void			Resume() override;
	virtual void	Quit();
	bool			IsQuitting() const;

	void	DisplayBusyCursor();
	void	DisplayInactiveCursor();

	void	HideAllWindows();

	Time	GetCurrentTime() const;

	void	InstallIdleTask(JXIdleTask* task);
	void	RemoveIdleTask(JXIdleTask* task);

	void	InstallUrgentTask(JXUrgentTask* task);

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

	static void	StartFiber(const std::function<void()>& f,
						   const FiberPriority priority = kIdleTaskPriority);
	static bool	IsWorkerFiber();

	// for use by main()

	static void	RemoveCmdLineOption(int* argc, char* argv[],
									const unsigned long offset,
									const unsigned long removeCount);

	// called by JXDisplay

	void	DisplayOpened(JXDisplay* display);
	void	DisplayClosed(JXDisplay* display);

	// called by JXWindow

	const JString&	GetRestartCommand() const;

	// required by JXInitGlobals()

	[[noreturn]] static int	JXIOErrorHandler(Display* xDisplay);

	// called by JXAssert

	[[noreturn]] static void	Abort(const JXDocumentManager::SafetySaveReason reason,
									  const bool dumpCore);

	// called by JXMDIServer

	static void	StripBaseOptions(JPtrArray<JString>* argList);

protected:

	void	UpdateCurrentTime();

	bool	Close() override;	// use Quit() instead

	void			ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;
	virtual void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

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

	std::recursive_mutex*	itsTaskMutex;

	JString	itsSignature;
	JString	itsWMName;
	JString	itsRestartCmd;		// for session managers
	bool	itsRequestQuitFlag;
	bool	itsIsQuittingFlag;

private:

	void	HandleOneEvent();
	void	StartTasks(const bool hadEvents);
	void	StartIdleTasks();

	static void	ParseBaseOptions(int* argc, char* argv[], JString* displayName);
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
	return itsDisplayList->GetItem(itsCurrDisplayIndex);
}

/******************************************************************************
 GetDisplayCount

 ******************************************************************************/

inline JSize
JXApplication::GetDisplayCount()
	const
{
	return itsDisplayList->GetItemCount();
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
	return itsDisplayList->GetItem(index);
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
 GetSignature

 ******************************************************************************/

inline const JString&
JXApplication::GetSignature()
	const
{
	return itsSignature;
}

/******************************************************************************
 GetWMName

 ******************************************************************************/

inline const JString&
JXApplication::GetWMName()
	const
{
	return itsWMName;
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
	return itsIsQuittingFlag;
}

#endif
