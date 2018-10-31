/******************************************************************************
 GLPlotApp.cpp

	GLPlotApp class

	Copyright (C) 1997 by Glenn W. Bach.

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

static const JUtf8Byte* kAppSignature = "glove";

static const JString kCursorSubPath("/cursormodule/", kJFalse);
static const JString kDataSubPath("/datamodule/", kJFalse);
static const JString kImportSubPath("/importmodule/", kJFalse);
static const JString kExportSubPath("/exportmodule/", kJFalse);
static const JString kFitSubPath("/fitmodule/", kJFalse);

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

	itsModulePath = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsModulePath != nullptr);

	JString homeDir;
	
	if (!JGetHomeDirectory(&homeDir))
		{
		homeDir = JGetRootDirectory();
		}
	
	JAppendDirSeparator(&homeDir);

	JString dmhome = homeDir + ".glove";
	JString* str = jnew JString(dmhome);
	assert(str != nullptr);
	itsModulePath->Append(str);
	str = jnew JString("/usr/local/lib/glove");
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

//	itsFileImpProgs = jnew JPtrArray<JString>();


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
	
	
	itsDataModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsCursorModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsImportModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsExportModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsFitModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	itsDataPathIndex = jnew JArray<JIndex>;
	itsCursorPathIndex = jnew JArray<JIndex>;
	itsImportPathIndex = jnew JArray<JIndex>;
	itsExportPathIndex = jnew JArray<JIndex>;
	itsFitPathIndex = jnew JArray<JIndex>;

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
	jdelete itsFileImpProgs;
	
	if (foundFileImpDir)
		{
		jdelete itsFileImpDir;
		}
*/
	itsModulePath->DeleteAll();
	jdelete itsModulePath;

	itsDataModules->DeleteAll();
	jdelete itsDataModules;
	itsCursorModules->DeleteAll();
	jdelete itsCursorModules;
	itsImportModules->DeleteAll();
	jdelete itsImportModules;
	itsExportModules->DeleteAll();
	jdelete itsExportModules;
	itsFitModules->DeleteAll();
	jdelete itsFitModules;
	jdelete itsDataPathIndex;
	jdelete itsCursorPathIndex;
	jdelete itsImportPathIndex;
	jdelete itsExportPathIndex;
	jdelete itsFitPathIndex;
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
		JString path = *(itsModulePath->GetElement(i)) + kImportSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = jnew JString(entry.GetName());
					itsImportModules->Append(str);
					itsImportPathIndex->AppendElement(i);
					}
				}					
			jdelete info;
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
	*path = *(itsModulePath->GetElement(mIndex)) + kImportSubPath +
				*(itsImportModules->GetElement(index));
	return kJTrue;
/*	
	if (foundFileImpDir)
		{
		assert( itsFileImpProgs->IndexValid(index) );
		
		JString str = itsFileImpDir->GetCWD();
		JAppendDirSeparator(&str);
		str += *(itsFileImpProgs->GetElement(index));
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
			JString* str = jnew JString(entry.GetName());
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
	JString str = "Untitled " + JString((JUInt64) itsDirNumber);
	itsDirNumber++;
	GXDataDocument* tableDir = jnew GXDataDocument(this, str, kJFalse);
	assert( tableDir != nullptr);
	tableDir->Activate();
}

/******************************************************************************
 OpenFile

 ******************************************************************************/

void 
GLPlotApp::OpenFile
	(
	const JString& fileName
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
		GXDataDocument* tableDir = jnew GXDataDocument(this, fileName, kJTrue);
		assert( tableDir != nullptr);
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
		JString path = *(itsModulePath->GetElement(i)) + kDataSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = jnew JString(entry.GetName());
					itsDataModules->Append(str);
					itsDataPathIndex->AppendElement(i);
					}
				}					
			jdelete info;
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
	*path = *(itsModulePath->GetElement(mIndex)) + kDataSubPath + 
				*(itsDataModules->GetElement(index));
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
		JString path = *(itsModulePath->GetElement(i)) + kCursorSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = jnew JString(entry.GetName());
					itsCursorModules->Append(str);
					itsCursorPathIndex->AppendElement(i);
					}
				}					
			jdelete info;
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
	*path = *(itsModulePath->GetElement(mIndex)) + kCursorSubPath +
				*(itsCursorModules->GetElement(index));
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
		JString path = *(itsModulePath->GetElement(i)) + kExportSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = jnew JString(entry.GetName());
					itsExportModules->Append(str);
					itsExportPathIndex->AppendElement(i);
					}
				}					
			jdelete info;
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
	*path = *(itsModulePath->GetElement(mIndex)) + kExportSubPath +
				*(itsExportModules->GetElement(index));
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
		JString path = *(itsModulePath->GetElement(i)) + kFitSubPath;
		if (JDirInfo::Create(path, &info))
			{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
				{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
					{
					JString* str = jnew JString(entry.GetName());
					itsFitModules->Append(str);
					itsFitPathIndex->AppendElement(i);
					}
				}					
			jdelete info;
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
	*path = *(itsModulePath->GetElement(mIndex)) + kFitSubPath +
				*(itsFitModules->GetElement(index));
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

	prevVersStr can be nullptr.

 ******************************************************************************/

void
GLPlotApp::DisplayAbout
	(
	const JString& prevVersStr
	)
{
	GLAboutDialog* dlog = jnew GLAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
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
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
GLPlotApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static public)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
GLPlotApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kGLDefaultStringData);
}
