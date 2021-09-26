/******************************************************************************
 <pre>Globals.h

   Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#ifndef _H_<pre>Globals
#define _H_<pre>Globals

#include <jx-af/jx/jXGlobals.h>

// we include these for convenience

#include "<PRE>App.h"
#include "<PRE>PrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>

class <PRE>MDIServer;

<PRE>App*			<PRE>GetApplication();
<PRE>PrefsManager*	<PRE>GetPrefsManager();
<PRE>MDIServer*		<PRE>GetMDIServer();

const JString&		<PRE>GetVersionNumberStr();
JString				<PRE>GetVersionStr();

	// called by <PRE>App

bool	<PRE>CreateGlobals(<PRE>App* app);
void	<PRE>DeleteGlobals();
void	<PRE>CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	<PRE>GetWMClassInstance();
const JUtf8Byte*	<PRE>GetMainWindowClass();

#endif
