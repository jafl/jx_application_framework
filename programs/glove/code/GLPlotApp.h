/******************************************************************************
 GLPlotApp.h

	Interface for the GLPlotApp class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLPlotApp
#define _H_GLPlotApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class GLPlotApp : public JXApplication
{
public:

	GLPlotApp(int* argc, char* argv[], JBoolean* displayAbout, JString* prevVersStr);
	
	virtual ~GLPlotApp();

	JBoolean				GetImportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetImportModules();
	void					ReloadImportModules();
	
	JBoolean 				GetExportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetExportModules();
	void					ReloadExportModules();
	
	JBoolean 				GetDataModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetDataModules();
	void					ReloadDataModules();
	
	JBoolean 				GetFitModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetFitModules();
	void					ReloadFitModules();
	
	JBoolean 				GetCursorModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetCursorModules();
	void					ReloadCursorModules();

	const JPtrArray<JString>&	GetModulePath() const;

	void NewFile();
	void OpenFile(const JCharacter* filename);
	void DisplayAbout(const JCharacter* prevVersStr = NULL);

	static void					InitStrings();

protected:

	virtual void	DirectorClosed(JXDirector* theDirector);

private:
	
	JIndex 				itsDirNumber;

	JPtrArray<JString>* itsModulePath;

	JPtrArray<JString>*	itsDataModules;
	JPtrArray<JString>*	itsCursorModules;
	JPtrArray<JString>*	itsExportModules;
	JPtrArray<JString>*	itsImportModules;
	JPtrArray<JString>*	itsFitModules;

	JArray<JIndex>*		itsDataPathIndex;
	JArray<JIndex>*		itsCursorPathIndex;
	JArray<JIndex>*		itsExportPathIndex;
	JArray<JIndex>*		itsImportPathIndex;
	JArray<JIndex>*		itsFitPathIndex;

};

#endif


