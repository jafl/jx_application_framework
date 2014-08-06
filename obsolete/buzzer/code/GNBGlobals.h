/******************************************************************************
 GNBGlobals.h

 *****************************************************************************/

#ifndef _H_GNBGlobals
#define _H_GNBGlobals

// we include these for convenience

#include <JString.h>

#include <jXGlobals.h>
#include <jTypes.h>

class GNBApp;
class GNBNoteDir;
class GNBPrefsMgr;
class GNBPTPrinter;
class GNBTreeDir;
class GNBMDIServer;

class JXPTPrinter;

class JTreeList;

GNBApp*				GNBGetApplication();
GNBPrefsMgr*		GNBGetPrefsMgr();
GNBMDIServer*		GNBGetMDIServer();
GNBTreeDir*			GNBGetTrashDir();
GNBPTPrinter*		GNBGetAltPTPrinter();

JXPTPrinter*		GNBGetPTPrinter();

GNBNoteDir*			GNBGetCurrentPrintDir();
void				GNBSetCurrentPrintDir(GNBNoteDir* dir);

JTreeList*			GNBGetCurrentPrintList();
void				GNBSetCurrentPrintList(JTreeList* list);

	// called by TestApp

JBoolean 	GNBCreateGlobals(GNBApp* app);
void 		GNBDeleteGlobals();
void		GNBSetTrashDir(GNBTreeDir* dir);

const JCharacter*	GNBGetWMClassInstance();
const JCharacter*	GNBGetNotebookWindowClass();
const JCharacter*	GNBGetTrashWindowClass();
const JCharacter*	GNBGetEditorWindowClass();

#endif
