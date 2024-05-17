/******************************************************************************
 jGlobals.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_jGlobals
#define _H_jGlobals

// we include these for convenience

#include "JAssertBase.h"
#include "JUserNotification.h"
#include "JCreateProgressDisplay.h"
#include "JStringManager.h"
#include <functional>
#include <mutex>

class JFontManager;
class JWebBrowser;

void JInitCore(JAssertBase* ah = nullptr, const JUtf8Byte* appSignature = nullptr,
			   const JUtf8Byte** defaultStringData = nullptr,
			   JUserNotification* un = nullptr, JCreateProgressDisplay* cpg = nullptr);
void JSetWebBrowser(JWebBrowser* webBrowser);

void JDeleteGlobals();

bool					JGetAssertHandler(JAssertBase** ah);
JWebBrowser*			JGetWebBrowser();

JUserNotification*		JGetUserNotification();
JCreateProgressDisplay*	JGetCreatePG();

JStringManager*			JGetStringManager();
const JString&			JGetString(const JUtf8Byte* id);
JString					JGetString(const JUtf8Byte* id, const JUtf8Byte* map[], const JSize size);

bool	JGetDataDirectories(const JUtf8Byte* signature, const JUtf8Byte* dirName,
							JString* sysDir, JString* userDir);

void	JSetTaskScheduler(const std::function<void(const std::function<void()>&)>& sched);
void	JScheduleTask(const std::function<void()>& f);

bool	JIsInteractiveThread();
void	JSetThreadIsInteractive(const bool interactive = true);

std::recursive_mutex&	JGetTemporaryDirectoryChangeMutex();

/******************************************************************************
 Convenience functions

 ******************************************************************************/

inline JProgressDisplay*
JNewPG()
{
	return JGetCreatePG()->New();
}

#endif
