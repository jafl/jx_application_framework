/******************************************************************************
 wizGlobals.cpp

	Access to global objects and factories.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "wizGlobals.h"
#include "Wiz2War.h"
#include "wizHelpText.h"
#include <jAssert.h>

static WizApp*	theApplication       = NULL;	// owns itself
static Wiz2War*	theConnectionManager = NULL;

static const JCharacter* kWMClassInstance = "WizWar_Client";
static const JCharacter* kMapWindowClass  = "WizWar_Map";
static const JCharacter* kChatWindowClass = "WizWar_Chat";

// string ID's

static const JCharacter* kDescriptionID = "Description::wizGlobals";

/******************************************************************************
 WizCreateGlobals

 ******************************************************************************/

void
WizCreateGlobals
	(
	WizApp* app
	)
{
	theApplication = app;

	JXInitHelp(kWizTOCHelpName, kWizHelpSectionCount, kWizHelpSectionName);

	theConnectionManager = new Wiz2War;
	assert( theConnectionManager != NULL );
}

/******************************************************************************
 WizDeleteGlobals

 ******************************************************************************/

void
WizDeleteGlobals()
{
	delete theConnectionManager;
	theConnectionManager = NULL;

	theApplication = NULL;
}

/******************************************************************************
 WizGetApplication

 ******************************************************************************/

WizApp*
WizGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 WizGetConnectionManager

 ******************************************************************************/

Wiz2War*
WizGetConnectionManager()
{
	assert( theConnectionManager != NULL );
	return theConnectionManager;
}

/******************************************************************************
 WizGetWMClassInstance

 ******************************************************************************/

const JCharacter*
WizGetWMClassInstance()
{
	return kWMClassInstance;
}

/******************************************************************************
 WizGetMapWindowClass

 ******************************************************************************/

const JCharacter*
WizGetMapWindowClass()
{
	return kMapWindowClass;
}

/******************************************************************************
 WizGetChatWindowClass

 ******************************************************************************/

const JCharacter*
WizGetChatWindowClass()
{
	return kChatWindowClass;
}

/******************************************************************************
 WizGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
WizGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 WizGetVersionStr

 ******************************************************************************/

JString
WizGetVersionStr()
{
	const JString protVers = JString(kWWCurrentProtocolVersion, JString::kBase10);
	const JCharacter* map[] =
		{
		"version",          JGetString("VERSION"),
		"protocol_version", protVers,
		"copyright",        JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
