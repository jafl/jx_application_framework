/******************************************************************************
 jGlobals.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_jGlobals
#define _H_jGlobals

// we include these for convenience

#include "jx-af/jcore/JAssertBase.h"
#include "jx-af/jcore/JUserNotification.h"
#include "jx-af/jcore/JChooseSaveFile.h"
#include "jx-af/jcore/JCreateProgressDisplay.h"
#include "jx-af/jcore/JStringManager.h"

class JFontManager;
class JWebBrowser;

void JInitCore(JAssertBase* ah = nullptr, const JUtf8Byte* appSignature = nullptr,
			   const JUtf8Byte** defaultStringData = nullptr, JUserNotification* un = nullptr,
			   JChooseSaveFile* csf = nullptr, JCreateProgressDisplay* cpg = nullptr);
void JSetWebBrowser(JWebBrowser* webBrowser);

void JDeleteGlobals();

bool					JGetAssertHandler(JAssertBase** ah);
JWebBrowser*			JGetWebBrowser();

JUserNotification*		JGetUserNotification();
JChooseSaveFile*		JGetChooseSaveFile();
JCreateProgressDisplay*	JGetCreatePG();

JStringManager*			JGetStringManager();
const JString&			JGetString(const JUtf8Byte* id);
JString					JGetString(const JUtf8Byte* id, const JUtf8Byte* map[], const JSize size);

bool	JGetDataDirectories(const JUtf8Byte* signature, const JUtf8Byte* dirName,
							JString* sysDir, JString* userDir);

/******************************************************************************
 Convenience functions

 ******************************************************************************/

inline JProgressDisplay*
JNewPG()
{
	return (JGetCreatePG())->New();
}

#endif
