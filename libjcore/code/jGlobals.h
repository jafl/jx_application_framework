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
			   const JUtf8Byte* defaultFontName = NULL,
			   const JUtf8Byte* greekFontName = NULL,
			   const JUtf8Byte* monospaceFontName = NULL);
void JSetWebBrowser(JWebBrowser* webBrowser);

void JDeleteGlobals();

JBoolean				JGetAssertHandler(JAssertBase** ah);
JStringManager*			JGetStringManager();
JWebBrowser*			JGetWebBrowser();

JUserNotification*		JGetUserNotification();
JChooseSaveFile*		JGetChooseSaveFile();
JCreateProgressDisplay*	JGetCreatePG();

const JFontManager*		JGetCurrFontManager();
JColormap*				JGetCurrColormap();

const JUtf8Byte*		JGetDefaultFontName();
const JUtf8Byte*		JGetGreekFontName();
const JUtf8Byte*		JGetMonospaceFontName();

JBoolean	JGetJDataDirectories(const JUtf8Byte* dirName,
								 JString* sysDir, JString* userDir);
JBoolean	JGetDataDirectories(const JUtf8Byte* signature, const JUtf8Byte* dirName,
								JString* sysDir, JString* userDir);

/******************************************************************************
 Convenience functions

 ******************************************************************************/

inline JProgressDisplay*
JNewPG()
{
	return (JGetCreatePG())->New();
}

inline const JString&
JGetString
	(
	const JUtf8Byte* id
	)
{
	return (JGetStringManager())->Get(id);
}

inline JString
JGetString
	(
	const JUtf8Byte*	id,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	return (JGetStringManager())->Get(id, map, size);
}

#endif
