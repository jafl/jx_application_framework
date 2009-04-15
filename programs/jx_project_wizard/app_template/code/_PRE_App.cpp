/******************************************************************************
 <PRE>App.cc

	<Description>

	BASE CLASS = public JXApplication

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>App.h"
#include "<PRE>AboutDialog.h"
#include "<pre>StringData.h"
#include "<pre>Globals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "<Binary>";

/******************************************************************************
 Constructor

 *****************************************************************************/

<PRE>App::<PRE>App
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
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
			*displayAbout = kJTrue;
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

	prevVersStr can be NULL.

 ******************************************************************************/

void
<PRE>App::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	<PRE>AboutDialog* dlog = new <PRE>AboutDialog(this, prevVersStr);
	assert( dlog != NULL );
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

const JCharacter*
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
	(JGetStringManager())->Register(kAppSignature, k<PRE>DefaultStringData);
}
