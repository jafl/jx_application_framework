/******************************************************************************
 GLDataDocument.h

	Interface for the GLDataDocument class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLDataDocument
#define _H_GLDataDocument

#include <JXFileDocument.h>
#include <JString.h>
#include <JPtrArray.h>

class GLRaggedFloatTableData;
class GLRaggedFloatTable;
class JXTextMenu;
class JXPSPrinter;
class JXDialogDirector;
class GLChooseFileImportDialog;
class GLPlotDir;
class JXScrollbarSet;
class GLGetDelimiterDialog;

class GLDataDocument : public JXFileDocument
{
public:

	GLDataDocument(JXDirector* supervisor, const JString& fileName,
					const JBoolean onDisk);

	virtual ~GLDataDocument();

	void CreateNewPlot(	const JIndex type,
						const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
						const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
						const JBoolean linked, const JString& label );
						
	void AddToPlot( const JIndex plotIndex, const JIndex type,
					const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
					const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
					const JBoolean linked, const JString& label );
	
	void 					GetPlotNames(JPtrArray<JString>& names);
	GLRaggedFloatTableData*	GetData();
	JString					GetInternalModuleName(const JIndex index) const;
	JSize					GetInternalModuleCount();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	DirectorClosed(JXDirector* theDirector) override;
	virtual void	WriteTextFile(std::ostream& output, const JBoolean safetySave) const override;
	virtual void	DiscardChanges() override;
			
private:

	JXPSPrinter*				itsPrinter;
	GLRaggedFloatTableData*		itsData;
	GLRaggedFloatTable*			itsTable;			// owned by its enclosure
	JXTextMenu*					itsFileMenu;		// owned by the menu bar
	JXTextMenu*					itsExportMenu;
	JXTextMenu*					itsHelpMenu;
	
	GLChooseFileImportDialog*	itsFileImportDialog;
	GLGetDelimiterDialog*		itsDelimiterDialog;
	JString						itsCurrentFileName;

	JPtrArray<GLPlotDir>* itsPlotWindows;
	JSize				itsPlotNumber;
	JBoolean			itsListenToData;

	JXScrollbarSet*		itsScrollbarSet;

// begin JXLayout


// end JXLayout

private:

	void		BuildWindow();
	void		LoadFile(const JString& fileName);

	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex item);

	void		UpdateExportMenu();
	void		HandleExportMenu(const JIndex item);

	void		HandleHelpMenu(const JIndex item);

	JBoolean	LoadNativeFile(std::istream& is);
	void		LoadImportFile();
	void		LoadDelimitedFile();
	void		LoadInternalFile(const JIndex index);
	
	void		ChooseFileFilter();
	void		ReadPlotData(std::istream& is, const JFloat gloveVersion);

	// not allowed

	GLDataDocument(const GLDataDocument& source);
	const GLDataDocument& operator=(const GLDataDocument& source);
};

/******************************************************************************
 GetData

 ******************************************************************************/

inline GLRaggedFloatTableData*	
GLDataDocument::GetData()
{
	return itsData;
}

#endif
