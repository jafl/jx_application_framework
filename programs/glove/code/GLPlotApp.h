/******************************************************************************
 GLPlotApp.h

	Interface for the GLPlotApp class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLPlotApp
#define _H_GLPlotApp

#include <JXApplication.h>

class GLPlotApp : public JXApplication
{
public:

	GLPlotApp(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);
	
	virtual ~GLPlotApp();

	bool				GetImportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetImportModules();
	void					ReloadImportModules();
	
	bool 				GetExportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetExportModules();
	void					ReloadExportModules();
	
	bool 				GetDataModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetDataModules();
	void					ReloadDataModules();
	
	bool 				GetFitModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetFitModules();
	void					ReloadFitModules();
	
	bool 				GetCursorModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* 	GetCursorModules();
	void					ReloadCursorModules();

	const JPtrArray<JString>&	GetModulePath() const;

	void NewFile();
	void OpenFile(const JString& filename);
	void DisplayAbout(const JString& prevVersStr = JString::empty);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

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


