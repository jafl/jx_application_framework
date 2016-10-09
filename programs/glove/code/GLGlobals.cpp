/******************************************************************************
 GLGlobals.h

	Copyright (C) 1997-2000 by Glenn Bach. 

 ******************************************************************************/

#include "GLGlobals.h"
#include "GLFitManager.h"
#include "GLPrefsMgr.h"
#include "GLMDIServer.h"
#include "GLHelpText.h"

#include <JXPTPrinter.h>

#include <JString.h>
#include <JPtrArray.h>

#include <jDirUtil.h>
#include <jFileUtil.h>

#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

GLFitManager*			itsFitManager	= NULL;
GLPrefsMgr*				itsPrefsMgr 	= NULL;
GLPlotApp*				itsApplication 	= NULL;
static JXPTPrinter*		itsPrinter		= NULL;
GLMDIServer*			itsMDIServer	= NULL;

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
	assert(itsPrefsMgr != NULL);
	
	JXInitHelp(kGLTOCHelpName, kHelpSectionCount, kHelpSectionName);

	itsPrinter = jnew JXPTPrinter();
	assert(itsPrinter != NULL);

	itsPrefsMgr->ReadPrinterSetup(itsPrinter);

	itsFitManager = jnew GLFitManager();
	assert(itsFitManager != NULL);

	itsMDIServer	= jnew GLMDIServer(app);
	assert(itsMDIServer != NULL);

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
	assert(itsPrefsMgr != NULL);
	return itsPrefsMgr;
}

/******************************************************************************
 GLGetApplication

 ******************************************************************************/

GLPlotApp*
GLGetApplication()
{
	assert(itsApplication != NULL);
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
	assert(itsFitManager != NULL);
	return itsFitManager;
}

/******************************************************************************
 GLGetMDIServer

 ******************************************************************************/

GLMDIServer*	
GLGetMDIServer()
{
	assert(itsMDIServer != NULL);
	return itsMDIServer;
}
