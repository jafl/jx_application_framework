/******************************************************************************
 jGlobals.h

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jGlobals
#define _H_jGlobals

// we include these for convenience

#include <JAssertBase.h>
#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JCreateProgressDisplay.h>
#include <JStringManager.h>

class JGetCurrentFontManager;
class JFontManager;
class JGetCurrentColormap;
class JColormap;
class JWebBrowser;

void JInitCore(JAssertBase* ah = NULL, const JUtf8Byte* appSignature = NULL,
			   const JUtf8Byte** defaultStringData = NULL, JUserNotification* un = NULL,
			   JChooseSaveFile* csf = NULL, JCreateProgressDisplay* cpg = NULL,
			   JGetCurrentFontManager* gcfm = NULL,
			   JGetCurrentColormap* gcc = NULL,
			   const JString& defaultFontName = JString::empty,
			   const JSize defaultFontSize = 0,
			   const JSize defaultRowColHeaderFontSize = 0,
			   const JString& monospaceFontName = JString::empty,
			   const JSize defaultMonoFontSize = 0);
void JSetWebBrowser(JWebBrowser* webBrowser);

void JDeleteGlobals();

JBoolean				JGetAssertHandler(JAssertBase** ah);
JWebBrowser*			JGetWebBrowser();

JUserNotification*		JGetUserNotification();
JChooseSaveFile*		JGetChooseSaveFile();
JCreateProgressDisplay*	JGetCreatePG();

const JFontManager*		JGetCurrFontManager();
JColormap*				JGetCurrColormap();

JStringManager*			JGetStringManager();
const JString&			JGetString(const JUtf8Byte* id);
JString					JGetString(const JUtf8Byte* id, const JUtf8Byte* map[], const JSize size);

const JString&			JGetDefaultFontName();
JSize					JGetDefaultFontSize();
JSize					JGetDefaultRowColHeaderFontSize();

const JString&			JGetMonospaceFontName();
JSize					JGetDefaultMonoFontSize();

JBoolean	JGetDataDirectories(const JString& signature, const JString& dirName,
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
