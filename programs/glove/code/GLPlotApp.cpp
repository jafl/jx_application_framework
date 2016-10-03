/******************************************************************************
 GLPlotApp.cpp

	GLPlotApp class

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GLPlotApp.h"
#include "GXDataDocument.h"
#include "GLGlobals.h"
#include "GLPrefsMgr.h"
#include "GLAboutDialog.h"
#include "GLStringData.h"

#include <JXDocumentManager.h>

#include <JDirInfo.h>
#include <JPtrArray.h>
#include <JPoint.h>
#include <jFileUtil.h>
#include <stdlib.h>
#include <jAssert.h>

const JCharacter* kCursorSubPath = "/cursormodule/";
const JCharacter* kDataSubPath = "/datamodule/";
const JCharacter* kImportSubPath = "/importmodule/";
const JCharacter* kExportSubPath = "/exportmodule/";
const JCharacter* kFitSubPath = "/fitmodule/";

static const JCharacter* kGloveFileSignature = "*** Glove File Format ***";

const JCharacter* kAppSignature = "glove";

/******************************************************************************
 Constructor

 ******************************************************************************/

GLPlotApp::GLPlotApp
	(
	int* 		argc, 
	char* 		argv[],
	JBoolean* 	displayAbout, 
	JString* 	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kGLDefaultStringData)
{
// Assumption - person has home dir, or no fileimpprogs

	itsModulePath = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsModulePath != NULL);

	JString homeDir;
	
	if (!JGetHomeDirectory(&homeDir))
		{
		homeDir == "/";		
		}
	
	JAppendDirSeparator(&homeDir);

	JString dmhome = homeDir + ".glove";
	JString* str = new JString(dmhome);
	assert(str != NULL);
	itsModulePath->Append(str);
	str = new JString("/usr/local/lib/glove");
	itsModulePath->Append(str);
	

	
	*displayAbout = InitGLGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = GetPrefsMgr()->GetGloveVersionStr();
		if (*prevVersStr == JGetString("VERSION"))
			{
			prevVersStr->Clear();
			}
		else
			{
			*displayAbout = kJTrue;
			}
		}
	else
		{
		prevVersStr->Clear();
		}

//	itsFileImpProgs = new JPtrArray<JString>();


	JXCreateDefaultDocumentManager(kJTrue);
	
/*
	JString fihome = homeDir + ".glove/fileimport/";
	foundFileImpDir = kJFalse;
	if (JDirInfo::Create(fihome, &itsFileImpDir))
		{
		foundFileImpDir = kJTrue;
		UpdateFileImpProgs();
		}
*/
	itsDirNumber = 1;
	
	
	itsDataModules = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsCursorModules = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsImportModules = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsExportModules = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsFitModules = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	itsDataPathIndex = new JArray<JIndex>;
	itsCursorPathIndex = new JArray<JIndex>;
	itsImportPathIndex = new JArray<JIndex>;
	itsExportPathIndex = new JArray<JIndex>;
	itsFitPathIndex = new JArray<JIndex>;

	ReloadDataModules();
	ReloadExportModules();
	ReloadCursorModules();
	ReloadFitModules();
	ReloadImportModules();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLPlotApp::~GLPlotApp()
{
/*
	itsFileImpProgs->DeleteAll();
	delete itsFileImpProgs;
	
	if (foundFileImpDir)
		{
		delete itsFileImpDir;
		}
*/
	itsModulePath->DeleteAll();
	delete itsModulePath;

	itsDataModules->DeleteAll();
	delete itsDataModules;
	itsCursorModules->DeleteAll();
	delete itsCursorModules;
	itsImportModules->DeleteAll();
	delete itsImportModules;
	itsExportModules->DeleteAll();
	delete itsExportModules;
	itsFitModules->DeleteAll();
	delete itsFitModules;
	delete itsDataPathIndex;
	delete itsCursorPathIndex;
	delete itsImportPathIndex;
	delete itsExportPathIndex;
	delete itsFitPathIndex;
	DeleteGLGlobals();
}

/******************************************************************************
 ReloadImportModules

 ******************************************************************************/

void 
GLPlotApp::ReloadImportModules()
{
	itsImportModules->DeleteAll();
	itsImportPathIndex->RemoveAll();
	JDirInfo* info;
	
	const JSize count = itsModulePath->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString path = *(itsModulePath->NthElement(i)) + kImportSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = new JString(entry.GetName());
					itsImportModules->Append(str);
					itsImportPathIndex->AppendElement(i);
					}
				}					
			delete info;
			}
		}
}

/******************************************************************************
 GetImportModulePath

 ******************************************************************************/

JBoolean
GLPlotApp::GetImportModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsImportModules->IndexValid(index));
	
	JIndex mIndex = itsImportPathIndex->GetElement(index);
	*path = *(itsModulePath->NthElement(mIndex)) + kImportSubPath +
				*(itsImportModules->NthElement(index));
	return kJTrue;
/*	
	if (foundFileImpDir)
		{
		assert( itsFileImpProgs->IndexValid(index) );
		
		JString str = itsFileImpDir->GetCWD();
		JAppendDirSeparator(&str);
		str += *(itsFileImpProgs->NthElement(index));
		*path = str;
		return kJTrue;
		}
		
	return kJFalse;
*/		
}

/******************************************************************************
 GetImportModules

 ******************************************************************************/

JPtrArray<JString>* 
GLPlotApp::GetImportModules()
{
//	if (foundFileImpDir)
//		{
//		itsFileImpDir->UpdateList();
//		}
	return itsImportModules;
}

/******************************************************************************
 Receive


void 
GLPlotApp::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsFileImpDir && 
		(message.Is(JDirInfo::kContentsUpdated) ||
		message.Is(JDirInfo::kPermissionsChanged) ))
		{
		UpdateFileImpProgs();
		}
		
}
 ******************************************************************************/


/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void 
GLPlotApp::DirectorClosed
	(
	JXDirector* theDirector
	)
{

}

/******************************************************************************
 UpdateFileImpProgs

 ******************************************************************************/
/*
void 
GLPlotApp::UpdateFileImpProgs()
{
	for (JSize i = 1; i <= itsFileImpDir->GetEntryCount(); i++)
		{
		const JDirEntry& entry = itsFileImpDir->GetEntry(i);
		if ( entry.IsExecutable() && !(entry.IsDirectory()) )
			{
			JString* str = new JString(entry.GetName());
			itsFileImpProgs->Append(str);
			}
		}
}
*/

/******************************************************************************
 NewFile

 ******************************************************************************/

void 
GLPlotApp::NewFile()
{
	JString str = "Untitled " + JString(itsDirNumber);
	itsDirNumber++;
	GXDataDocument* tableDir = new GXDataDocument(this, str, kJFalse);
	assert( tableDir != NULL);
	tableDir->Activate();
}

/******************************************************************************
 OpenFile

 ******************************************************************************/

void 
GLPlotApp::OpenFile
	(
	const JCharacter* fileName
	)
{
	JXFileDocument* doc;
	if (JXGetDocumentManager()->FileDocumentIsOpen(fileName, &doc))
		{
		doc->Activate();
		}
	else if (!JFileExists(fileName))
		{
		JString msg = fileName;
		msg += " does not exist.";
		(JGetUserNotification())->ReportError(msg);
		}
	else if (!JFileReadable(fileName))
		{
		JString msg = fileName;
		msg += " is not readable.";
		(JGetUserNotification())->ReportError(msg);
		}
	else
		{
		GXDataDocument* tableDir = new GXDataDocument(this, fileName, kJTrue);
		assert( tableDir != NULL);
		tableDir->Activate();
		}
}

/******************************************************************************
 ReloadDataModules

 ******************************************************************************/

void 
GLPlotApp::ReloadDataModules()
{
	itsDataModules->DeleteAll();
	itsDataPathIndex->RemoveAll();
	JDirInfo* info;
	
	const JSize count = itsModulePath->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString path = *(itsModulePath->NthElement(i)) + kDataSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = new JString(entry.GetName());
					itsDataModules->Append(str);
					itsDataPathIndex->AppendElement(i);
					}
				}					
			delete info;
			}
		}
}

/******************************************************************************
 GetDataModulePath

 ******************************************************************************/

JBoolean
GLPlotApp::GetDataModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsDataModules->IndexValid(index));
	
	JIndex mIndex = itsDataPathIndex->GetElement(index);
	*path = *(itsModulePath->NthElement(mIndex)) + kDataSubPath + 
				*(itsDataModules->NthElement(index));
	return kJTrue;
		
}

/******************************************************************************
 GetDataModules

 ******************************************************************************/

JPtrArray<JString>* 
GLPlotApp::GetDataModules()
{
	return itsDataModules;
}

/******************************************************************************
 ReloadCursorModules

 ******************************************************************************/

void 
GLPlotApp::ReloadCursorModules()
{
	itsCursorModules->DeleteAll();
	itsCursorPathIndex->RemoveAll();
	JDirInfo* info;
	
	const JSize count = itsModulePath->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString path = *(itsModulePath->NthElement(i)) + kCursorSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = new JString(entry.GetName());
					itsCursorModules->Append(str);
					itsCursorPathIndex->AppendElement(i);
					}
				}					
			delete info;
			}
		}
}

/******************************************************************************
 GetCursorModulePath

 ******************************************************************************/

JBoolean
GLPlotApp::GetCursorModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsCursorModules->IndexValid(index));
	
	JIndex mIndex = itsCursorPathIndex->GetElement(index);
	*path = *(itsModulePath->NthElement(mIndex)) + kCursorSubPath +
				*(itsCursorModules->NthElement(index));
	return kJTrue;
		
}

/******************************************************************************
 GetCursorModules

 ******************************************************************************/

JPtrArray<JString>* 
GLPlotApp::GetCursorModules()
{
	return itsCursorModules;
}

/******************************************************************************
 ReloadExportModules

 ******************************************************************************/

void 
GLPlotApp::ReloadExportModules()
{
	itsExportModules->DeleteAll();
	itsExportPathIndex->RemoveAll();
	JDirInfo* info;
	
	const JSize count = itsModulePath->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString path = *(itsModulePath->NthElement(i)) + kExportSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = new JString(entry.GetName());
					itsExportModules->Append(str);
					itsExportPathIndex->AppendElement(i);
					}
				}					
			delete info;
			}
		}
}

/******************************************************************************
 GetExportModulePath

 ******************************************************************************/

JBoolean
GLPlotApp::GetExportModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsExportModules->IndexValid(index));
	
	JIndex mIndex = itsExportPathIndex->GetElement(index);
	*path = *(itsModulePath->NthElement(mIndex)) + kExportSubPath +
				*(itsExportModules->NthElement(index));
	return kJTrue;
		
}

/******************************************************************************
 GetExportModules

 ******************************************************************************/

JPtrArray<JString>* 
GLPlotApp::GetExportModules()
{
	return itsExportModules;
}

/******************************************************************************
 ReloadFitModules

 ******************************************************************************/

void 
GLPlotApp::ReloadFitModules()
{
	itsFitModules->DeleteAll();
	itsFitPathIndex->RemoveAll();
	JDirInfo* info;
	
	const JSize count = itsModulePath->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString path = *(itsModulePath->NthElement(i)) + kFitSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = new JString(entry.GetName());
					itsFitModules->Append(str);
					itsFitPathIndex->AppendElement(i);
					}
				}					
			delete info;
			}
		}
}

/******************************************************************************
 GetFitModulePath

 ******************************************************************************/

JBoolean
GLPlotApp::GetFitModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsFitModules->IndexValid(index));
	
	JIndex mIndex = itsFitPathIndex->GetElement(index);
	*path = *(itsModulePath->NthElement(mIndex)) + kFitSubPath +
				*(itsFitModules->NthElement(index));
	return kJTrue;
		
}

/******************************************************************************
 GetFitModules

 ******************************************************************************/

JPtrArray<JString>* 
GLPlotApp::GetFitModules()
{
	return itsFitModules;
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be NULL.

 ******************************************************************************/

void
GLPlotApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	GLAboutDialog* dlog = new GLAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 GetModulePath (public)

 ******************************************************************************/

const JPtrArray<JString>&
GLPlotApp::GetModulePath()
	const
{
	return *itsModulePath;
}

/******************************************************************************
 InitStrings (static public)

 ******************************************************************************/

void
GLPlotApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kGLDefaultStringData);
}
