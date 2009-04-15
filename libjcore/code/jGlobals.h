/******************************************************************************
 jGlobals.h

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jGlobals
#define _H_jGlobals

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

// we include these for convenience

#include <JAssertBase.h>
#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JCreateProgressDisplay.h>
#include <JStringManager.h>
#include <JString.h>

class JGetCurrentFontManager;
class JFontManager;
class JGetCurrentColormap;
class JColormap;

void JInitCore(JAssertBase* ah = NULL, const JCharacter* appSignature = NULL,
			   const JCharacter** defaultStringData = NULL, JUserNotification* un = NULL,
			   JChooseSaveFile* csf = NULL, JCreateProgressDisplay* cpg = NULL,
			   JGetCurrentFontManager* gcfm = NULL,
			   JGetCurrentColormap* gcc = NULL,
			   const JCharacter* defaultFontName = NULL,
			   const JCharacter* greekFontName = NULL,
			   const JCharacter* monospaceFontName = NULL);

void JDeleteGlobals();

JBoolean				JGetAssertHandler(JAssertBase** ah);
JStringManager*			JGetStringManager();

JUserNotification*		JGetUserNotification();
JChooseSaveFile*		JGetChooseSaveFile();
JCreateProgressDisplay*	JGetCreatePG();

const JFontManager*		JGetCurrFontManager();
JColormap*				JGetCurrColormap();

const JCharacter*		JGetDefaultFontName();
const JCharacter*		JGetGreekFontName();
const JCharacter*		JGetMonospaceFontName();

JBoolean	JGetJDataDirectories(const JCharacter* dirName,
								 JString* sysDir, JString* userDir);
JBoolean	JGetDataDirectories(const JCharacter* signature, const JCharacter* dirName,
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
	const JCharacter* id
	)
{
	return (JGetStringManager())->Get(id);
}

inline JString
JGetString
	(
	const JCharacter*	id,
	const JCharacter*	map[],
	const JSize			size
	)
{
	return (JGetStringManager())->Get(id, map, size);
}

#endif
