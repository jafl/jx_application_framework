/******************************************************************************
 GLGlobals.h

	Copyright (C) 1997-2000 by Glenn Bach. 

 ******************************************************************************/

#include "GLGlobals.h"
#include "GLFitManager.h"
#include "GLPrefsMgr.h"
#include "GLMDIServer.h"

#include <JXPTPrinter.h>

#include <JPtrArray-JString.h>
#include <jDirUtil.h>
#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

static GLFitManager*	itsFitManager	= nullptr;
static GLPrefsMgr*		itsPrefsMgr 	= nullptr;
static GLPlotApp*		itsApplication 	= nullptr;
static JXPTPrinter*		itsPrinter		= nullptr;
static GLMDIServer*		itsMDIServer	= nullptr;

/******************************************************************************
 InitGLGlobals


 ******************************************************************************/

JBoolean 
InitGLGlobals
	(
	GLPlotApp* app
	)
{
	itsApplication = app;

	JBoolean isNew;
	itsPrefsMgr = jnew GLPrefsMgr(&isNew);
	assert(itsPrefsMgr != nullptr);
	
	JXInitHelp();

	itsPrinter = jnew JXPTPrinter();
	assert(itsPrinter != nullptr);

	itsPrefsMgr->ReadPrinterSetup(itsPrinter);

	itsFitManager = jnew GLFitManager();
	assert(itsFitManager != nullptr);

	itsMDIServer	= jnew GLMDIServer(app);
	assert(itsMDIServer != nullptr);

	return isNew;
}

/******************************************************************************
 DeleteGLGlobals


 ******************************************************************************/

void 
DeleteGLGlobals()
{
	itsPrefsMgr->WritePrinterSetup(itsPrinter);
	jdelete itsPrinter;
	jdelete itsFitManager;
	jdelete itsPrefsMgr;
}

/******************************************************************************
 GetPrefsMgr

 ******************************************************************************/

GLPrefsMgr*
GetPrefsMgr()
{
	assert(itsPrefsMgr != nullptr);
	return itsPrefsMgr;
}

/******************************************************************************
 GLGetApplication

 ******************************************************************************/

GLPlotApp*
GLGetApplication()
{
	assert(itsApplication != nullptr);
	return itsApplication;
}

/******************************************************************************
 GNBGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
GLGetPTPrinter()
{
	return itsPrinter;
}

/******************************************************************************
 GetFitManager

 ******************************************************************************/

GLFitManager*
GetFitManager()
{
	assert(itsFitManager != nullptr);
	return itsFitManager;
}

/******************************************************************************
 GLGetMDIServer

 ******************************************************************************/

GLMDIServer*	
GLGetMDIServer()
{
	assert(itsMDIServer != nullptr);
	return itsMDIServer;
}
