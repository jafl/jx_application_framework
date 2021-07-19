/******************************************************************************
 GLGlobals.h

	Copyright (C) 1997-2000 by Glenn Bach. 

 ******************************************************************************/

#include "GLGlobals.h"
#include "GLFitManager.h"
#include "GLPrefsMgr.h"
#include "GLMDIServer.h"

#include <JXPTPrinter.h>
#include <JXTextMenu.h>

#include <JPtrArray-JString.h>
#include <jDirUtil.h>
#include <cstdarg>
#include <jAssert.h>

static GLFitManager*	itsFitManager	= nullptr;
static GLPrefsMgr*		itsPrefsMgr 	= nullptr;
static GLPlotApp*		itsApplication 	= nullptr;
static JXPTPrinter*		itsPrinter		= nullptr;
static GLMDIServer*		itsMDIServer	= nullptr;

/******************************************************************************
 InitGLGlobals

 ******************************************************************************/

bool 
InitGLGlobals
	(
	GLPlotApp* app
	)
{
	itsApplication = app;

	bool isNew;
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
GLGetPrefsMgr()
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
GLGetFitManager()
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

/******************************************************************************
 GLBuildColumnMenus

 ******************************************************************************/

void
GLBuildColumnMenus
	(
	const JUtf8Byte*	key,
	const JSize			count,
	...
	)
{
	std::va_list args;
	va_start(args, count);

	for (JUInt64 i=1; i<=count; i++)
		{
		JString str(i);
		const JUtf8Byte* map[] =
			{
			"i", str.GetBytes()
			};
		str = JGetString(key, map, sizeof("map"));

		std::va_list args2;
		va_copy(args2, args);

		while (true)
			{
			JXTextMenu* menu = va_arg(args2, JXTextMenu*);
			if (menu == nullptr)
				{
				break;
				}
			menu->AppendItem(str);
			}

		va_end(args2);
		}

	va_end(args);
}
