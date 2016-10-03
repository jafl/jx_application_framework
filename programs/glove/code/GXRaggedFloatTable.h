/******************************************************************************
 GXRaggedFloatTable.h

	Interface for the GXRaggedFloatTable class

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXRaggedFloatTable
#define _H_GXRaggedFloatTable

#include <JXEditTable.h>
#include <JAuxTableData.h>
#include <JFontStyle.h>

class JString;
class GRaggedFloatTableData;
class JXFloatInput;
class JTableSelection;
class JXMenuBar;
class JXTextMenu;
class GXCreatePlotDialog;
class GXCreateVectorPlotDialog;
class GXSimpleTransformDialog;
class GXFullTransformDialog;
class GXColByIncDialog;
class GXColByRangeDialog;
class GXDataDocument;
class GXTransformFunctionDialog;
class GVarList;
class JXToolBar;
class JXTextButton;
class JUndo;
class GLUndoBase;
class DataModule;
class JPSPrinter;

class GXRaggedFloatTable : public JXEditTable
{
public:

	friend class GLUndoBase;
	friend class DataModule;

public:

	enum SelectionType
	{
		kRowsSelected = 1,
		kColsSelected,
		kElementsSelected,
		kNoneSelected
	};

public:

	GXRaggedFloatTable(GXDataDocument* dir,
					JXTextButton* okButton,
					GRaggedFloatTableData* data, const int precision,
					JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GXRaggedFloatTable();

	void SelectRow(const JIndex row);
	void AddRowToSelection(const JIndex row);
	void ExtendSelectionToRow(const JIndex row);
	void SelectCol(const JIndex col);
	void AddColToSelection(const JIndex col);
	void ExtendSelectionToCol(const JIndex col);

	void WriteData(ostream& os);
	void ReadData(istream& is, const JFloat gloveVersion);

	GRaggedFloatTableData*				GetFloatData() const;
	GXRaggedFloatTable::SelectionType 	GetSelectionType();

	void 		GetSelectionArea(JIndex* rows, JIndex* cols, JIndex* startRow, JIndex* startCol);
	JBoolean 	WriteDataCols(ostream& os, const int cols);
	void 		ExportDataMatrix(ostream& os);
	void 		ExportData(ostream& os);

	void		PrintRealTable(JPSPrinter& p);

	// needed by undo

	void			Undo();
	void			Redo();
	JBoolean		HasUndo() const;
	JBoolean		HasRedo() const;

	void			LoadDefaultToolButtons(JXToolBar* toolbar);

protected:

	virtual void Receive(JBroadcaster* sender, const Message& message);
	virtual void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectRangeDrag
	};

	// needed by undo

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

private:

	GXDataDocument*				itsTableDir;
	GRaggedFloatTableData*		itsFloatData;			// we don't own this

	JXFloatInput*				itsFloatInputField;		// used during editing

	JXTextMenu*					itsEditMenu;
	JXTextMenu*					itsDataMenu;
	JXTextMenu*					itsModuleMenu;

	GXCreatePlotDialog*			itsCreatePlotDialog;
	GXCreateVectorPlotDialog*	itsCreateVectorPlotDialog;
	GXColByRangeDialog*			itsColByRangeDialog;
	GXColByIncDialog*			itsColByIncDialog;
	GXTransformFunctionDialog*	itsTransDialog;

	Atom						itsGloveTextXAtom;

	GVarList*					itsVarList;
	JXTextButton*				itsOKButton;
	DragType					itsDragType;

	// used for undo

	JPtrArray<JUndo>*			itsUndoList;
	JIndex						itsFirstRedoIndex;	// range [1:count+1]
	UndoState					itsUndoState;

private:

	void SelectCell(const JPoint cell);
	void ExtendSelection(const JPoint cell);

	void HandleEditMenu(const JIndex Index);
	void UpdateEditMenu();
	void HandleDataMenu(const JIndex index);
	void UpdateDataMenu();

	void HandleCutCmd();
	void HandleCopyCmd();
	void HandlePasteCmd();
	void HandleSpecialPasteCmd();
	void HandleInsertion(const JBoolean undo = kJTrue);
	void HandleDuplication();
	void HandleDeletion();

	void ChoosePlotColumns(const JIndex type);
	void PlotData(const JIndex type);

	void GetNewColByRange();
	void CreateNewColByRange();

	void GetNewColByInc();
	void CreateNewColByInc();

	void EvaluateTransformFunction();
	void ChooseNewTransformFunction();

	void HandleModuleMenu(const JIndex index);
	void UpdateModuleMenu();

	// needed by undo

	JBoolean 	GetCurrentRedo(JUndo** undo) const;
	JBoolean	GetCurrentUndo(JUndo** undo) const;
	void 		NewUndo(JUndo* undo);

	// not allowed

	GXRaggedFloatTable(const GXRaggedFloatTable& source);
	const GXRaggedFloatTable& operator=(const GXRaggedFloatTable& source);
};


/******************************************************************************
 GetFloatData (protected)

 ******************************************************************************/

inline GRaggedFloatTableData*
GXRaggedFloatTable::GetFloatData()
	const
{
	return itsFloatData;
}

#endif
