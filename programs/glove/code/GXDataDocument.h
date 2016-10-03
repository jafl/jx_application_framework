/******************************************************************************
 GXDataDocument.h

	Interface for the GXDataDocument class

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXDataDocument
#define _H_GXDataDocument

#include <JXFileDocument.h>
#include <JString.h>
#include <JPtrArray.h>

class GRaggedFloatTableData;
class GXRaggedFloatTable;
class JXTextMenu;
class JXPSPrinter;
class JXDialogDirector;
class GXChooseFileImportDialog;
class PlotDir;
class JXScrollbarSet;
class GXGetDelimiterDialog;

class GXDataDocument : public JXFileDocument
{
public:

	GXDataDocument(JXDirector* supervisor, const JCharacter* fileName,
					const JBoolean onDisk);

	virtual ~GXDataDocument();

	void CreateNewPlot(	const JIndex type,
						const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
						const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
						const JBoolean linked, const JCharacter* label );
						
	void AddToPlot( const JIndex plotIndex, const JIndex type,
					const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
					const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
					const JBoolean linked, const JCharacter* label );
	
	void 					GetPlotNames(JPtrArray<JString>& names);
	GRaggedFloatTableData*	GetData();
	const JCharacter*		GetInternalModuleName(const JIndex index);
	JSize					GetInternalModuleCount();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	DirectorClosed(JXDirector* theDirector);
	virtual void	WriteTextFile(ostream& output, const JBoolean safetySave) const;
	virtual void	DiscardChanges();
			
private:

	JXPSPrinter*				itsPrinter;
	GRaggedFloatTableData*		itsData;
	GXRaggedFloatTable*			itsTable;			// owned by its enclosure
	JXTextMenu*					itsFileMenu;		// owned by the menu bar
	JXTextMenu*					itsExportMenu;
	JXTextMenu*					itsHelpMenu;
	
	GXChooseFileImportDialog*	itsFileImportDialog;
	GXGetDelimiterDialog*		itsDelimiterDialog;
	JString						itsCurrentFileName;

	JPtrArray<PlotDir>* itsPlotWindows;
	JSize				itsPlotNumber;
	JBoolean			itsListenToData;

	JXScrollbarSet*		itsScrollbarSet;

// begin JXLayout


// end JXLayout

private:

	void		BuildWindow();
	void		LoadFile(const JCharacter* fileName);

	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex item);

	void		UpdateExportMenu();
	void		HandleExportMenu(const JIndex item);

	void		HandleHelpMenu(const JIndex item);

	JBoolean	LoadNativeFile(istream& is);
	void		LoadImportFile();
	void		LoadDelimitedFile();
	void		LoadInternalFile(const JIndex index);
	
	void		ChooseFileFilter();
	void		ReadPlotData(istream& is, const JFloat gloveVersion);

	// not allowed

	GXDataDocument(const GXDataDocument& source);
	const GXDataDocument& operator=(const GXDataDocument& source);
};

/******************************************************************************
 GetData

 ******************************************************************************/

inline GRaggedFloatTableData*	
GXDataDocument::GetData()
{
	return itsData;
}

#endif
