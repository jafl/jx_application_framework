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

static const JUtf8Byte* kWMClassInstance = "Leibnitz";
static const JUtf8Byte* kExprWindowClass = "Leibnitz_Expression";
static const JUtf8Byte* kPlotWindowClass = "Leibnitz_Plot";
static const JUtf8Byte* kConvWindowClass = "Leibnitz_Base_Conversion";
static const JUtf8Byte* kVarWindowClass  = "Leibnitz_Constants";

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

	bool isNew;
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

const JUtf8Byte*
THXGetWMClassInstance()
{
	return kWMClassInstance;
}

/******************************************************************************
 THXGetExprWindowClass

 ******************************************************************************/

const JUtf8Byte*
THXGetExprWindowClass()
{
	return kExprWindowClass;
}

/******************************************************************************
 THXGetPlotWindowClass

 ******************************************************************************/

const JUtf8Byte*
THXGetPlotWindowClass()
{
	return kPlotWindowClass;
}

/******************************************************************************
 THXGetBaseConvWindowClass

 ******************************************************************************/

const JUtf8Byte*
THXGetBaseConvWindowClass()
{
	return kConvWindowClass;
}

/******************************************************************************
 THXGetVarWindowClass

 ******************************************************************************/

const JUtf8Byte*
THXGetVarWindowClass()
{
	return kVarWindowClass;
}

/******************************************************************************
 THXGetVersionNumberStr

 ******************************************************************************/

const JString&
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
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::thxGlobals", map, sizeof(map));
}
