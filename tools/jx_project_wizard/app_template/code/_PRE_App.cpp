/******************************************************************************
 <PRE>App.cpp

	BASE CLASS = public JXApplication

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#include "<PRE>App.h"
#include "<PRE>AboutDialog.h"
#include "<pre>StringData.h"
#include "<pre>Globals.h"
#include <jAssert.h>

static const JUtf8Byte* kAppSignature = "<Binary>";

/******************************************************************************
 Constructor

 *****************************************************************************/

<PRE>App::<PRE>App
	(
	int*		argc,
	char*		argv[],
	bool*		displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, k<PRE>DefaultStringData)
{
	*displayAbout = <PRE>CreateGlobals(this);

	if (!*displayAbout)
	{
		*prevVersStr = (<PRE>GetPrefsManager())->GetPrevVersionStr();
		if (*prevVersStr == <PRE>GetVersionNumberStr())
		{
			prevVersStr->Clear();
		}
		else
		{
			*displayAbout = true;
		}
	}
	else
	{
		prevVersStr->Clear();
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

<PRE>App::~<PRE>App()
{
	<PRE>DeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
<PRE>App::DisplayAbout
	(
	const JString& prevVersStr
	)
{
	<PRE>AboutDialog* dlog = jnew <PRE>AboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
<PRE>App::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
	{
//		JPrefObject::WritePrefs();
	}

	<PRE>CleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
<PRE>App::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
<PRE>App::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, k<PRE>DefaultStringData);
}
