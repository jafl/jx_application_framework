/******************************************************************************
 thxGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "thxGlobals.h"
#include "THXMDIServer.h"
#include "THXPrefsManager.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXPTPrinter.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <jAssert.h>

static THXApp*				theApplication     = nullptr;	// owns itself
static THXMDIServer*		theMDIServer       = nullptr;	// owned by JX
static THXPrefsManager*		thePrefsManager    = nullptr;
static JXPTPrinter*			theTapePrinter     = nullptr;
static JXPSPrinter*			thePSGraphPrinter  = nullptr;
static JX2DPlotEPSPrinter*	theEPSGraphPrinter = nullptr;

static const JCharacter* kWMClassInstance = "Leibnitz";
static const JCharacter* kExprWindowClass = "Leibnitz_Expression";
static const JCharacter* kPlotWindowClass = "Leibnitz_Plot";
static const JCharacter* kConvWindowClass = "Leibnitz_Base_Conversion";
static const JCharacter* kVarWindowClass  = "Leibnitz_Constants";

// string ID's

static const JCharacter* kDescriptionID = "Description::thxGlobals";

/******************************************************************************
 THXCreateGlobals

 ******************************************************************************/

void
THXCreateGlobals
	(
	THXApp* app
	)
{
	theApplication = app;

	JBoolean isNew;
	thePrefsManager	= jnew THXPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theMDIServer = jnew THXMDIServer;
	assert( theMDIServer != nullptr );

	theTapePrinter = jnew JXPTPrinter;
	assert( theTapePrinter != nullptr );

	JXDisplay* display = app->GetDisplay(1);

	thePSGraphPrinter = jnew JXPSPrinter(display);
	assert( thePSGraphPrinter != nullptr );

	theEPSGraphPrinter = jnew JX2DPlotEPSPrinter(display);
	assert( theEPSGraphPrinter != nullptr );
}

/******************************************************************************
 THXDeleteGlobals

 ******************************************************************************/

void
THXDeleteGlobals()
{
	jdelete theTapePrinter;
	theTapePrinter = nullptr;

	jdelete thePSGraphPrinter;
	thePSGraphPrinter = nullptr;

	jdelete theEPSGraphPrinter;
	theEPSGraphPrinter = nullptr;

	theApplication = nullptr;
	theMDIServer   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 THXGetApplication

 ******************************************************************************/

THXApp*
THXGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 THXGetPrefsManager

 ******************************************************************************/

THXPrefsManager*
THXGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 THXGetMDIServer

 ******************************************************************************/

THXMDIServer*
THXGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 THXGetPSGraphPrinter

 ******************************************************************************/

JXPSPrinter*
THXGetPSGraphPrinter()
{
	assert( thePSGraphPrinter != nullptr );
	return thePSGraphPrinter;
}

/******************************************************************************
 THXGetEPSGraphPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
THXGetEPSGraphPrinter()
{
	assert( theEPSGraphPrinter != nullptr );
	return theEPSGraphPrinter;
}

/******************************************************************************
 THXGetTapePrinter

 ******************************************************************************/

JXPTPrinter*
THXGetTapePrinter()
{
	assert( theTapePrinter != nullptr );
	return theTapePrinter;
}

/******************************************************************************
 THXGetWMClassInstance

 ******************************************************************************/

const JCharacter*
THXGetWMClassInstance()
{
	return kWMClassInstance;
}

/******************************************************************************
 THXGetExprWindowClass

 ******************************************************************************/

const JCharacter*
THXGetExprWindowClass()
{
	return kExprWindowClass;
}

/******************************************************************************
 THXGetPlotWindowClass

 ******************************************************************************/

const JCharacter*
THXGetPlotWindowClass()
{
	return kPlotWindowClass;
}

/******************************************************************************
 THXGetBaseConvWindowClass

 ******************************************************************************/

const JCharacter*
THXGetBaseConvWindowClass()
{
	return kConvWindowClass;
}

/******************************************************************************
 THXGetVarWindowClass

 ******************************************************************************/

const JCharacter*
THXGetVarWindowClass()
{
	return kVarWindowClass;
}

/******************************************************************************
 THXGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
THXGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 THXGetVersionStr

 ******************************************************************************/

JString
THXGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}
