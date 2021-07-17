/******************************************************************************
 GLRaggedFloatTable.h

	Interface for the GLRaggedFloatTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLRaggedFloatTable
#define _H_GLRaggedFloatTable

#include <JXEditTable.h>
#include <JAuxTableData.h>
#include <JFontStyle.h>

class JString;
class GLRaggedFloatTableData;
class JXFloatInput;
class JTableSelection;
class JXMenuBar;
class JXTextMenu;
class GLCreatePlotDialog;
class GLCreateVectorPlotDialog;
class GXSimpleTransformDialog;
class GXFullTransformDialog;
class GLColByIncDialog;
class GLColByRangeDialog;
class GLDataDocument;
class GLTransformFunctionDialog;
class GLVarList;
class JXToolBar;
class JXTextButton;
class JUndo;
class GLUndoBase;
class GLDataModule;
class JPSPrinter;

class GLRaggedFloatTable : public JXEditTable
{
public:

	friend class GLUndoBase;
	friend class GLDataModule;

public:

	enum SelectionType
	{
		kRowsSelected = 1,
		kColsSelected,
		kElementsSelected,
		kNoneSelected
	};

public:

	GLRaggedFloatTable(GLDataDocument* dir,
					JXTextButton* okButton,
					GLRaggedFloatTableData* data, const int precision,
					JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GLRaggedFloatTable();

	void SelectRow(const JIndex row);
	void AddRowToSelection(const JIndex row);
	void ExtendSelectionToRow(const JIndex row);
	void SelectCol(const JIndex col);
	void AddColToSelection(const JIndex col);
	void ExtendSelectionToCol(const JIndex col);

	void WriteData(std::ostream& os);
	void ReadData(std::istream& is, const JFloat gloveVersion);

	GLRaggedFloatTableData*				GetFloatData() const;
	GLRaggedFloatTable::SelectionType 	GetSelectionType();

	void 		GetSelectionArea(JIndex* rows, JIndex* cols, JIndex* startRow, JIndex* startCol);
	bool 	WriteDataCols(std::ostream& os, const int cols);
	void 		ExportDataMatrix(std::ostream& os);
	void 		ExportData(std::ostream& os);

	void		PrintRealTable(JPSPrinter& p);

	// needed by undo

	void			Undo();
	void			Redo();
	bool		HasUndo() const;
	bool		HasRedo() const;

	void			LoadDefaultToolButtons(JXToolBar* toolbar);

protected:

	virtual void Receive(JBroadcaster* sender, const Message& message) override;
	virtual void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void HandleKeyPress(const JUtf8Character& c, const int keySym,
								const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

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

	GLDataDocument*				itsTableDir;
	GLRaggedFloatTableData*		itsFloatData;			// we don't own this

	JXFloatInput*				itsFloatInputField;		// used during editing

	JXTextMenu*					itsEditMenu;
	JXTextMenu*					itsDataMenu;
	JXTextMenu*					itsModuleMenu;

	GLCreatePlotDialog*			itsCreatePlotDialog;
	GLCreateVectorPlotDialog*	itsCreateVectorPlotDialog;
	GLColByRangeDialog*			itsColByRangeDialog;
	GLColByIncDialog*			itsColByIncDialog;
	GLTransformFunctionDialog*	itsTransDialog;

	Atom						itsGloveTextXAtom;

	GLVarList*					itsTransformVarList;
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
	void HandleInsertion(const bool undo = true);
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

	bool 	GetCurrentRedo(JUndo** undo) const;
	bool	GetCurrentUndo(JUndo** undo) const;
	void 		NewUndo(JUndo* undo);

	// not allowed

	GLRaggedFloatTable(const GLRaggedFloatTable& source);
	const GLRaggedFloatTable& operator=(const GLRaggedFloatTable& source);
};


/******************************************************************************
 GetFloatData (protected)

 ******************************************************************************/

inline GLRaggedFloatTableData*
GLRaggedFloatTable::GetFloatData()
	const
{
	return itsFloatData;
}

#endif
