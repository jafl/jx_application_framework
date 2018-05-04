/******************************************************************************
 jGlobals.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_jGlobals
#define _H_jGlobals

// we include these for convenience

#include <JAssertBase.h>
#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JCreateProgressDisplay.h>
#include <JStringManager.h>

class JFontManager;
class JColorManager;
class JWebBrowser;

void JInitCore(JAssertBase* ah = NULL, const JUtf8Byte* appSignature = NULL,
			   const JUtf8Byte** defaultStringData = NULL, JUserNotification* un = NULL,
			   JChooseSaveFile* csf = NULL, JCreateProgressDisplay* cpg = NULL,
			   const JString& defaultFontName = JString::empty,
			   const JSize defaultFontSize = 0,
			   const JSize defaultRowColHeaderFontSize = 0,
			   const JString& defaultMonospaceFontName = JString::empty,
			   const JSize defaultMonospaceFontSize = 0);
void JSetWebBrowser(JWebBrowser* webBrowser);

void JDeleteGlobals();

JBoolean				JGetAssertHandler(JAssertBase** ah);
JWebBrowser*			JGetWebBrowser();

JUserNotification*		JGetUserNotification();
JChooseSaveFile*		JGetChooseSaveFile();
JCreateProgressDisplay*	JGetCreatePG();

JStringManager*			JGetStringManager();
const JString&			JGetString(const JUtf8Byte* id);
JString					JGetString(const JUtf8Byte* id, const JUtf8Byte* map[], const JSize size);

const JString&			JGetDefaultFontName();
JSize					JGetDefaultFontSize();
JSize					JGetDefaultRowColHeaderFontSize();

const JString&			JGetDefaultMonospaceFontName();
JSize					JGetDefaultMonospaceFontSize();

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
