/******************************************************************************
 GNBGlobals.cc

	Access to testjx global objects and factories.

	Written by Glenn W. Bach.

 ******************************************************************************/

#include "GNBApp.h"
#include "GNBGlobals.h"
#include "GNBHelpText.h"
#include "GNBNoteDir.h"
#include "GNBPrefsMgr.h"
#include "GNBMDIServer.h"
#include "GNBPTPrinter.h"
#include "GNBTreeDir.h"

#include <JXDocumentManager.h>
#include <JXPTPrinter.h>
#include <JXWindow.h>

#include <JUserNotification.h>
#include <JDirEntry.h>
#include <JRegex.h>
#include <JTreeList.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jTime.h>

#include <unistd.h>
#include <stdlib.h>
#include <jAssert.h>

static GNBApp*				theApplication 		= NULL;		// owns itself
static GNBNoteDir*			itsPrinterDir		= NULL;
static GNBPrefsMgr*			itsPrefsMgr			= NULL;
static GNBMDIServer*		itsMDIServer		= NULL;
static GNBPTPrinter*		itsAltPTPrinter		= NULL;
static GNBTreeDir*			itsTrashDir			= NULL;

static JXDocumentManager*	itsDocMgr			= NULL;
static JXPTPrinter*			itsPrinter			= NULL;

static JTreeList*			itsPrinterList		= NULL;

static const JCharacter* kGNBWMInstance          	= "notebook";
static const JCharacter* kNotebookWindowClass      	= "Buzzer_Notebook";
static const JCharacter* kTrashWindowClass         	= "Buzzer_Trash";
static const JCharacter* kEditorWindowClass        	= "Buzzer_Editor";

/******************************************************************************
 GNBCreateGlobals

	server can be NULL

 ******************************************************************************/

JBoolean
GNBCreateGlobals
	(
	GNBApp*		app
	)
{
	theApplication = app;
	
	JString oldPrefsFile, newPrefsFile;
	if (JGetHomeDirectory(&oldPrefsFile))
		{
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, ".nps_buzzer.pref");
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	JBoolean isNew;
	itsPrefsMgr = new GNBPrefsMgr(&isNew);
	assert(itsPrefsMgr != NULL);

	JXInitHelp(kTOCHelpName, kHelpSectionCount, kHelpSectionName);

	itsDocMgr = 
		new JXDocumentManager(kJFalse);
	assert(itsDocMgr != NULL);

	itsPrinter = new JXPTPrinter();
	assert(itsPrinter != NULL);

	itsPrefsMgr->ReadPrinterSetup(itsPrinter);

	itsAltPTPrinter	= new GNBPTPrinter(itsPrefsMgr, kGPTAltPrinterSetupID);
	assert(itsAltPTPrinter != NULL);

	itsMDIServer = new GNBMDIServer(app);
	assert(itsMDIServer != NULL);

	GNBTreeDir::CreateTrash(app, &itsTrashDir);
	itsTrashDir->GetWindow()->SetCloseAction(JXWindow::kDeactivateDirector);
	JXGetDocumentManager()->DocumentMustStayOpen(itsTrashDir, kJTrue);
	
	return isNew;	
}

/******************************************************************************
 GNBDeleteGlobals

 ******************************************************************************/

void
GNBDeleteGlobals()
{	
	itsPrefsMgr->WritePrinterSetup(itsPrinter);
	delete itsPrinter;
	delete itsAltPTPrinter;
	delete itsPrefsMgr;
//	delete itsDocMgr;
	theApplication = NULL;
}

/******************************************************************************
 GNBGetApplication

 ******************************************************************************/

GNBApp*
GNBGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 GGetPrefsMgr

 ******************************************************************************/

GNBPrefsMgr*
GNBGetPrefsMgr()
{
	assert(itsPrefsMgr != NULL);
	return itsPrefsMgr;
}

/******************************************************************************
 GNBSetTrashDir

 ******************************************************************************/

void
GNBSetTrashDir
	(
	GNBTreeDir* dir
	)
{
	assert(itsTrashDir == NULL);
	itsTrashDir = dir;
}

/******************************************************************************
 GNBGetAltPrinter

 ******************************************************************************/

GNBPTPrinter*
GNBGetAltPTPrinter()
{
	assert(itsAltPTPrinter != NULL);
	return itsAltPTPrinter;
}

/******************************************************************************
 GNBGetTrashDir

 ******************************************************************************/

GNBTreeDir*
GNBGetTrashDir()
{
	assert(itsTrashDir != NULL);
	return itsTrashDir;
}

/******************************************************************************
 GNBGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
GNBGetPTPrinter()
{
	return itsPrinter;
}

/******************************************************************************
 GNBGetCurrentPrintDir

 ******************************************************************************/

GNBNoteDir*			
GNBGetCurrentPrintDir()
{
	assert(itsPrinterDir != NULL);
	return itsPrinterDir;
}

/******************************************************************************
 GNBSetCurrentPrintDir

 ******************************************************************************/

void
GNBSetCurrentPrintDir
	(
	GNBNoteDir* dir
	)
{
	itsPrinterDir	= dir;
}

/******************************************************************************
 GNBGetCurrentPrintList

 ******************************************************************************/

JTreeList*			
GNBGetCurrentPrintList()
{
	assert(itsPrinterList != NULL);
	return itsPrinterList;
}

/******************************************************************************
 GNBSetCurrentPrintList

 ******************************************************************************/

void
GNBSetCurrentPrintList
	(
	JTreeList* list
	)
{
	itsPrinterList	= list;
}

/******************************************************************************
 GNBGetWMClassInstance

 ******************************************************************************/

const JCharacter*
GNBGetWMClassInstance()
{
	return kGNBWMInstance;
}

/******************************************************************************
 GNBGetTableWindowClass

 ******************************************************************************/

const JCharacter*
GNBGetNotebookWindowClass()
{
	return kNotebookWindowClass;
}

/******************************************************************************
 GNBGetViewWindowClass

 ******************************************************************************/

const JCharacter*
GNBGetTrashWindowClass()
{
	return kTrashWindowClass;
}

/******************************************************************************
 GNBGetEditorWindowClass

 ******************************************************************************/

const JCharacter*
GNBGetEditorWindowClass()
{
	return kEditorWindowClass;
}

/******************************************************************************
 GNBGetAltPrinter

 ******************************************************************************/

GNBMDIServer*
GNBGetMDIServer()
{
	assert(itsMDIServer != NULL);
	return itsMDIServer;
}
