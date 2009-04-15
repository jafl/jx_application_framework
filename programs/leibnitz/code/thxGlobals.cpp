/******************************************************************************
 thxGlobals.cpp

	Access to global objects and factories.

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <thxStdInc.h>
#include "thxGlobals.h"
#include "THXMDIServer.h"
#include "thxHelpText.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXPTPrinter.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <jAssert.h>

static THXApp*				theApplication     = NULL;	// owns itself
static THXMDIServer*		theMDIServer       = NULL;	// owned by JX
static JXPTPrinter*			theTapePrinter     = NULL;
static JXPSPrinter*			thePSGraphPrinter  = NULL;
static JX2DPlotEPSPrinter*	theEPSGraphPrinter = NULL;

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

	JXInitHelp(kTHXTOCHelpName, kTHXHelpSectionCount, kTHXHelpSectionName);

	theMDIServer = new THXMDIServer;
	assert( theMDIServer != NULL );

	theTapePrinter = new JXPTPrinter;
	assert( theTapePrinter != NULL );

	JXDisplay* display = app->GetDisplay(1);

	thePSGraphPrinter = new JXPSPrinter(display, display->GetColormap());
	assert( thePSGraphPrinter != NULL );

	theEPSGraphPrinter = new JX2DPlotEPSPrinter(display, display->GetColormap());
	assert( theEPSGraphPrinter != NULL );
}

/******************************************************************************
 THXDeleteGlobals

 ******************************************************************************/

void
THXDeleteGlobals()
{
	delete theTapePrinter;
	theTapePrinter = NULL;

	delete thePSGraphPrinter;
	thePSGraphPrinter = NULL;

	delete theEPSGraphPrinter;
	theEPSGraphPrinter = NULL;

	theApplication = NULL;
	theMDIServer   = NULL;
}

/******************************************************************************
 THXGetApplication

 ******************************************************************************/

THXApp*
THXGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 THXGetMDIServer

 ******************************************************************************/

THXMDIServer*
THXGetMDIServer()
{
	assert( theMDIServer != NULL );
	return theMDIServer;
}

/******************************************************************************
 THXGetPSGraphPrinter

 ******************************************************************************/

JXPSPrinter*
THXGetPSGraphPrinter()
{
	assert( thePSGraphPrinter != NULL );
	return thePSGraphPrinter;
}

/******************************************************************************
 THXGetEPSGraphPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
THXGetEPSGraphPrinter()
{
	assert( theEPSGraphPrinter != NULL );
	return theEPSGraphPrinter;
}

/******************************************************************************
 THXGetTapePrinter

 ******************************************************************************/

JXPTPrinter*
THXGetTapePrinter()
{
	assert( theTapePrinter != NULL );
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
