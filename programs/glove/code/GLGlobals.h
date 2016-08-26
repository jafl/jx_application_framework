/******************************************************************************
 GLGlobals.h

	Copyright © 1997-2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLGlobals
#define _H_GLGlobals

#include <jXGlobals.h>

class GLFitManager;
class GLPrefsMgr;
class GLPlotApp;
class JXPTPrinter;
class GLMDIServer;

GLFitManager*	GetFitManager();
GLPrefsMgr*		GetPrefsMgr();
GLPlotApp*		GLGetApplication();
JXPTPrinter*	GLGetPTPrinter();
GLMDIServer*	GLGetMDIServer();

JBoolean		InitGLGlobals(GLPlotApp* app);
void			DeleteGLGlobals();

#endif
