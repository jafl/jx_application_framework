/******************************************************************************
 <pre>Globals.h

   Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<pre>Globals
#define _H_<pre>Globals

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jXGlobals.h>

// we include these for convenience

#include "<PRE>App.h"
#include "<PRE>PrefsManager.h"
#include <JXDocumentManager.h>

class <PRE>MDIServer;

<PRE>App*			<PRE>GetApplication();
<PRE>PrefsManager*	<PRE>GetPrefsManager();
<PRE>MDIServer*		<PRE>GetMDIServer();

const JCharacter*	<PRE>GetVersionNumberStr();
JString				<PRE>GetVersionStr();

	// called by <PRE>App

JBoolean	<PRE>CreateGlobals(<PRE>App* app);
void		<PRE>DeleteGlobals();
void		<PRE>CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
