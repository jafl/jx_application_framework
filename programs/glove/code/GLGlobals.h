/******************************************************************************
 GLGlobals.h

	Copyright (C) 1997-2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLGlobals
#define _H_GLGlobals

#include <jXGlobals.h>

class GLFitManager;
class GLPrefsMgr;
class GLPlotApp;
class JXPTPrinter;
class GLMDIServer;

GLFitManager*	GLGetFitManager();
GLPrefsMgr*		GLGetPrefsMgr();
GLPlotApp*		GLGetApplication();
JXPTPrinter*	GLGetPTPrinter();
GLMDIServer*	GLGetMDIServer();

bool		InitGLGlobals(GLPlotApp* app);
void			DeleteGLGlobals();

void	GLBuildColumnMenus(const JUtf8Byte* key, const JSize count, ...);

#endif
