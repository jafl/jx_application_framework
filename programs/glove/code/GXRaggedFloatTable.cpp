/******************************************************************************
 GXRaggedFloatTable.cpp

	Draws a table of numbers stored in a GRaggedFloatTableData object.

	BASE CLASS = JXEditTable

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GXRaggedFloatTable.h"
#include "GRaggedFloatTableData.h"
#include "GXColByIncDialog.h"
#include "GXColByRangeDialog.h"
#include "GXDataDocument.h"
#include "GXTransformFunctionDialog.h"
#include "GVarList.h"
#include "DataModule.h"
#include "GXCreatePlotDialog.h"
#include "GXCreateVectorPlotDialog.h"
#include "GLPlotApp.h"
#include <GXTextSelection.h>
#include "GLUndoElementChange.h"
#include "GLUndoElementAppend.h"
#include "GLUndoElementCut.h"
#include "GLUndoElementsChange.h"
#include "GLUndoElementsInsert.h"
#include "GLUndoElementsCut.h"
#include "GLGlobals.h"

#include "JXToolBar.h"

#include "editcut.xpm"
#include "editcopy.xpm"
#include "editpaste.xpm"
#include "plotdata.xpm"
#include "plotvectordata.xpm"
#include "transform.xpm"

#include <JXApplication.h>
#include <JXColormap.h>
#include <JXDisplay.h>
#include <JXFloatInput.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXSelectionManager.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXWindowDirector.h>

#include <JFunction.h>
#include <JFunctionWithVar.h>
#include <JExprNodeList.h>
#include <JFunctionType.h>
#include <jParseFunction.h>
#include <JPainter.h>
#include <JFontStyle.h>
#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JTableSelection.h>
#include <JTableSelectionIterator.h>
#include <JFontManager.h>
#include <JString.h>
#include <JPSPrinter.h>
#include <JOrderedSetUtil.h>

#include <jXActionDefs.h>
#include <jXKeysym.h>

#include <jStreamUtil.h>
#include <jASCIIConstants.h>
#include <sstream>

#include <jMath.h>
#include <jAssert.h>

// Setup information

const JFileVersion	kCurrentTableVersion = 0;

const JSize kDefaultSize 		= kJDefaultFontSize;

const JCoordinate kDefColWidth  = 100;
const JCoordinate kDefRowWidth 	= 20;
const JCoordinate kHMarginWidth = 2;
const JCoordinate kVMarginWidth = 1;

// Edit menu information

static const JCharacter* kEditMenuTitleStr  = "Edit";
static const JCharacter* kEditMenuStr =
	"    Undo               %k Meta-Z %i " kJXUndoAction
	"  | Redo               %k Meta-Shift-Z %i " kJXRedoAction
	"%l| Cut                %k Meta-X %i " kJXCutAction
	"  | Copy               %k Meta-C %i " kJXCopyAction
	"  | Paste              %k Meta-V %i " kJXPasteAction
	"  | Paste at Selection"
	"%l| Insert             %k Meta-I"
	"  | Duplicate          %k Meta-D"
	"  | Delete             %k Meta-K";

enum
{
	kUndoCmd = 1,
	kRedoCmd,
	kCutCmd,
	kCopyCmd,
	kPasteCmd,
	kSpecialPasteCmd,
	kInsertCmd,
	kDuplicateCmd,
	kDeleteCmd
};

// Data menu information


static const JCharacter* kDataMenuTitleStr  = "Data";
static const JCharacter* kDataMenuStr =
	"    Plot data... %i Plot::GXRaggedFloatTable"
	"  | Plot vector field...  %i PlotVector::GXRaggedFloatTable"
	"%l| Transform... %i Transform::GXRaggedFloatTable"
	"  | Generate column by range..."
	"  | Generate column by increment..."
	"%l| Data module";

enum
{
	kPlotCmd = 1,
	kPlotVectorCmd,
	kTransCmd,
	kGenerateRangeColCmd,
	kGenerateIncrementColCmd,
	kDataModuleCmd
};

enum
{
	kDataPlot = 1,
	kVectorPlot
};

enum
{
	kNewColByRangeCmd = 1,
	kNewColByIncCmd
};

static const JCharacter* kModuleMenuTitleStr  = "Data module";
static const JCharacter* kModuleMenuStr =
	"Reload %l";

enum
{
	kReloadModuleCmd = 1
};

// Selection Targets for Cut/Copy/Paste

static const JCharacter* kTextDataXAtomName 		= "XA_STRING";
static const JCharacter* kGloveTextDataXAtomName 	= "GLOVE_TEXT";

/******************************************************************************
 Constructor

 ******************************************************************************/

GXRaggedFloatTable::GXRaggedFloatTable
	(
	GXDataDocument*			dir,
	JXTextButton* 			okButton,
	GRaggedFloatTableData*	data,
	const int				precision,
	JXMenuBar* 				menuBar,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXEditTable(kDefRowWidth,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h),
	itsOKButton(okButton)
{
	itsTableDir = dir;
	assert( data != NULL );

	ListenTo(itsOKButton);
	itsOKButton->Deactivate();

	itsFloatData = data;

	itsFloatInputField = NULL;

	itsEditMenu = menuBar->AppendTextMenu(kEditMenuTitleStr);
	itsEditMenu->SetMenuItems(kEditMenuStr);
	itsEditMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditMenu);


	JXImage* image = new JXImage(GetDisplay(), JXPM(editcut));
	assert(image != NULL);
	itsEditMenu->SetItemImage(kCutCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(editcopy));
	assert(image != NULL);
	itsEditMenu->SetItemImage(kCopyCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(editpaste));
	assert(image != NULL);
	itsEditMenu->SetItemImage(kPasteCmd, image, kJTrue);

	itsDataMenu = menuBar->AppendTextMenu(kDataMenuTitleStr);
	itsDataMenu->SetMenuItems(kDataMenuStr);
	itsDataMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDataMenu);


	image = new JXImage(GetDisplay(), JXPM(plotdata));
	assert(image != NULL);
	itsDataMenu->SetItemImage(kPlotCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(plotvectordata));
	assert(image != NULL);
	itsDataMenu->SetItemImage(kPlotVectorCmd, image, kJTrue);


	image = new JXImage(GetDisplay(), JXPM(glv_transform));
	assert(image != NULL);
	itsDataMenu->SetItemImage(kTransCmd, image, kJTrue);

	itsModuleMenu = new JXTextMenu(itsDataMenu, kDataModuleCmd, menuBar);
	assert( itsModuleMenu != NULL );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsModuleMenu);

	itsCreatePlotDialog 		= NULL;
	itsCreateVectorPlotDialog	= NULL;
	itsColByRangeDialog 		= NULL;
	itsColByIncDialog 			= NULL;
	itsTransDialog 				= NULL;
	itsVarList 					= NULL;

	itsFirstRedoIndex   		= 1;
	itsUndoState        		= kIdle;

	itsUndoList = new JPtrArray<JUndo>(JPtrArrayT::kDeleteAll);
	assert(itsUndoList != NULL);

	SetTableData(itsFloatData);

	JTableSelection& selection = GetTableSelection();
	while (selection.GetColCount() < GetColCount())
		{
		selection.InsertCols(1, 1);
		}
	while (selection.GetRowCount() < GetRowCount())
		{
		selection.InsertRows(1, 1);
		}

	UpdateModuleMenu();

	itsGloveTextXAtom = GetDisplay()->RegisterXAtom(kGloveTextDataXAtomName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXRaggedFloatTable::~GXRaggedFloatTable()
{
	delete itsEditMenu;
	delete itsDataMenu;

	itsUndoList->DeleteAll();
	delete itsUndoList;
}

/******************************************************************************
 LoadDefaultToolButtons (public)

 ******************************************************************************/

void
GXRaggedFloatTable::LoadDefaultToolButtons
	(
	JXToolBar* toolbar
	)
{
	toolbar->NewGroup();
	toolbar->AppendButton(itsEditMenu, kCutCmd);
	toolbar->AppendButton(itsEditMenu, kCopyCmd);
	toolbar->AppendButton(itsEditMenu, kPasteCmd);
	toolbar->NewGroup();
	toolbar->AppendButton(itsDataMenu, kPlotCmd);
	toolbar->AppendButton(itsDataMenu, kPlotVectorCmd);
	toolbar->NewGroup();
	toolbar->AppendButton(itsDataMenu, kTransCmd);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GXRaggedFloatTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXColormap* colormap = GetColormap();

	if ((GetTableSelection()).IsSelected(cell))
		{
		p.SetFilling(kJTrue);
		p.SetPenColor(colormap->GetDefaultSelectionColor());
		p.Rect(rect);
		p.SetFilling(kJFalse);
		p.SetPenColor(colormap->GetBlackColor());
		}

	JPoint editCell;
	if (!GetEditedCell(&editCell) || cell != editCell)
		{
		p.SetFont(JGetDefaultFontName(), kDefaultSize, JFontStyle());


/* Original code that used the string buffer */
//		if (itsFloatBufferData->GetElement(cell, &str))
//			{
//			JRect r = rect;
//			r.left += kHMarginWidth;
//			p.String(r, str, JPainter::kHAlignRight, JPainter::kVAlignCenter);
//			}
		JFloat value;
		if (itsFloatData->GetElement(cell, &value))
			{
			JRect r = rect;
			r.left += kHMarginWidth;
			JString str(value, 6);
			p.String(r, str, JPainter::kHAlignRight, JPainter::kVAlignCenter);
			}
		}
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
GXRaggedFloatTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		return;
		}
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		s.ClearSelection();
		TableRefresh();
		return;
		}

	const JPoint newBoat   = cell;
	const JPoint newAnchor = cell;

	const JBoolean extendSelection = modifiers.shift();

	if (button == kJXLeftButton && clickCount == 2)
		{
		s.ClearSelection();
		s.SetBoat(newBoat);
		s.SetAnchor(newAnchor);
		BeginEditing(cell);
		}

	if (clickCount == 1)
		{
		if ((button == kJXLeftButton && extendSelection) || button == kJXRightButton)
			{
			if (s.OKToExtendSelection())
				{
				s.ExtendSelection(newBoat);
				}
			}
		else if (button == kJXLeftButton)
			{
			s.ClearSelection();
			s.SetBoat(newBoat);
			s.SetAnchor(newAnchor);
			s.SelectCell(cell);
			}
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
GXRaggedFloatTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ScrollForDrag(pt);
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	const JBoolean ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
	assert( ok );
	if (cell == s.GetBoat())
		{
		return;
		}
	const JPoint newBoat   = cell;

	s.ExtendSelection(newBoat);

	GetWindow()->Update();
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
GXRaggedFloatTable::HandleKeyPress
	(
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	if (key == kJEscapeKey)
		{
		(GetTableSelection()).ClearSelection();
		TableRefresh();
		}
	JPoint cell;
	JTableSelection& selection = GetTableSelection();
	if (!GetEditedCell(&cell) && selection.HasSelection())
		{
		JTableSelectionIterator* iter =
			new JTableSelectionIterator(&selection);
		assert(iter != NULL);
		JBoolean success = iter->Next(&cell);
		delete iter;
		if (key == kJReturnKey || key == XK_KP_Enter)
			{
			selection.ClearSelection();
			BeginEditing(cell);
			}
		else if (key == kJUpArrow)
			{
			if (cell.y > 1)
				{
				if (modifiers.meta())
					{
					cell.y = 1;
					}
				else
					{
					cell.y--;
					}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
				}
			}

		else if (key == kJDownArrow)
			{
			if (cell.y < (JCoordinate)GetRowCount())
				{
				if (modifiers.meta())
					{
					if (itsFloatData->ColIndexValid(cell.x))
						{
						JSize row = itsFloatData->GetDataRowCount(cell.x) + 1;
						if (row > GetRowCount())
							{
							row = GetRowCount();
							}
						cell.y = row;
						}
					}
				else
					{
					cell.y++;
					}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
				}
			}
		else if (key == kJLeftArrow)
			{
			if (cell.x > 1)
				{
				if (modifiers.meta())
					{
					cell.x = 1;
					}
				else
					{
					cell.x--;
					}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
				}
			}
		else if (key == kJRightArrow)
			{
			if (cell.x < (JCoordinate)GetColCount())
				{
				if (modifiers.meta())
					{
					JSize col = itsFloatData->GetDataColCount() + 1;
					if (col > GetColCount())
						{
						col = GetColCount();
						}
					cell.x = col;
					}
				else
					{
					cell.x++;
					}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
				}
			}
		else
			{
			JXEditTable::HandleKeyPress(key, modifiers);
			}
		}
	else
		{
		JXEditTable::HandleKeyPress(key, modifiers);
		}

}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GXRaggedFloatTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsFloatInputField == NULL );
	itsOKButton->Activate();

	itsFloatInputField =
		new JXFloatInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsFloatInputField != NULL );

	JFloat value;
	if (itsFloatData->GetElement(cell, &value))
		{
		itsFloatInputField->SetValue(value);
		}
	return itsFloatInputField;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsFloatInputField != NULL );

	if (itsFloatInputField->InputValid())
		{
		JFloat value;
		const JBoolean valid = itsFloatInputField->GetValue(&value);
		assert( valid );

		// save old value for undo
		JFloat oldvalue;
		JBoolean exists = itsFloatData->GetElement(cell, &oldvalue);

		// set to new value
		itsFloatData->SetElement(cell, value);

		// create and install undo object with old value
		if (exists)
			{
			GLUndoElementChange* undo =
				new GLUndoElementChange(this, cell, oldvalue);
			assert(undo != NULL);
			NewUndo(undo);
			}
		else
			{
			GLUndoElementAppend* undo =
				new GLUndoElementAppend(this, cell);
			assert(undo != NULL);
			NewUndo(undo);
			}

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GXRaggedFloatTable::PrepareDeleteXInputField()
{
	itsFloatInputField = NULL;
	itsOKButton->Deactivate();
}

/******************************************************************************
 ExtendSelection

 ******************************************************************************/

void
GXRaggedFloatTable::ExtendSelection
	(
	const JPoint cell
	)
{
	JTableSelectionIterator* iter =
		new JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != NULL);

	JPoint sCell;
	JBoolean success = iter->Next(&sCell);
	if (!success)
		{
		delete iter;
		return;
		}
	JIndex sCol;
	JIndex eCol;
	JIndex sRow;
	JIndex eRow;

	if (cell.x > sCell.x)
		{
		sCol = sCell.x;
		eCol = cell.x;
		}
	else
		{
		eCol = sCell.x;
		sCol = cell.x;
		}

	if (cell.y > sCell.y)
		{
		sRow = sCell.y;
		eRow = cell.y;
		}
	else
		{
		eRow = sCell.y;
		sRow = cell.y;
		}

	(GetTableSelection()).ClearSelection();
	for (JIndex i = sCol; i <= eCol; i++)
		{
		for (JIndex j = sRow; j <= eRow; j++)
			{
			(GetTableSelection()).SelectCell(j,i);
			}
		}

	delete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCell

 ******************************************************************************/

void
GXRaggedFloatTable::SelectCell
	(
	const JPoint cell
	)
{
	(GetTableSelection()).ClearSelection();
	(GetTableSelection()).SelectCell(cell);
	TableRefresh();
}

/******************************************************************************
 SelectRow

 ******************************************************************************/

void
GXRaggedFloatTable::SelectRow
	(
	const JIndex row
	)
{
	const JSize colCount = GetColCount();
	(GetTableSelection()).ClearSelection();

	for (JSize i = 1; i <= colCount; i++)
		{
		(GetTableSelection()).SelectCell(row, i);
		}
	TableRefresh();
}

/******************************************************************************
 AddRowToSelection

 ******************************************************************************/

void
GXRaggedFloatTable::AddRowToSelection
	(
	const JIndex row
	)
{

}

/******************************************************************************
 ExtendSelectionToRow

 ******************************************************************************/

void
GXRaggedFloatTable::ExtendSelectionToRow
	(
	const JIndex row
	)
{
	JTableSelectionIterator* iter =
		new JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != NULL);

	JPoint sCell;
	JBoolean success = iter->Next(&sCell);
	if (!success)
		{
		delete iter;
		return;
		}
	JSize sRow = sCell.y;
	JSize eRow = row;

	if (sRow > eRow)
		{
		sRow = row;
		eRow = sCell.y;
		}

	const JSize colCount = GetColCount();
	(GetTableSelection()).ClearSelection();
	for (JSize rowI = sRow; rowI <= eRow; rowI ++)
		{
		for (JSize col = 1; col <= colCount; col++)
			{
			(GetTableSelection()).SelectCell(rowI, col);
			}
		}

	delete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCol

 ******************************************************************************/

void
GXRaggedFloatTable::SelectCol
	(
	const JIndex col
	)
{
	const JSize rowCount = GetRowCount();
	JTableSelection& selection = GetTableSelection();
	selection.ClearSelection();
	selection.SelectCol(col);

//	for (JSize row = 1; row <= rowCount; row++)
//		{
//		(GetTableSelection()).SelectCell(row, col);
//		}
	TableRefresh();
}

/******************************************************************************
 AddColToSelection

 ******************************************************************************/

void
GXRaggedFloatTable::AddColToSelection
	(
	const JIndex col
	)
{
	JTableSelection& selection = GetTableSelection();
	selection.SelectCol(col);
}

/******************************************************************************
 ExtendSelectionToCol

 ******************************************************************************/

void
GXRaggedFloatTable::ExtendSelectionToCol
	(
	const JIndex col
	)
{
	const JSize rowCount = GetRowCount();
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator* iter =
		new JTableSelectionIterator(&selection);
	assert (iter != NULL);

	JPoint sCell;
	JBoolean success = iter->Next(&sCell);
	if (!success)
		{
		delete iter;
		return;
		}

	JSize sCol = sCell.x;
	JSize eCol = col;

	if (sCol > eCol)
		{
		sCol = col;
		eCol = sCell.x;
		}

	selection.ClearSelection();

	for (JSize colI = sCol; colI <= eCol; colI ++)
		{
		selection.SelectCol(colI);
//		for (JSize row = 1; row <= rowCount; row++)
//			{
//			(GetTableSelection()).SelectCell(row, colI);
//			}
		}

	delete iter;
	TableRefresh();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GXRaggedFloatTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleEditMenu(selection->GetIndex());
		}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditMenu();
		}

	else if (sender == itsDataMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleDataMenu(selection->GetIndex());
		}

	else if (sender == itsDataMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDataMenu();
		}

	else if (sender == itsModuleMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleModuleMenu(selection->GetIndex());
		}

	else if (sender == itsCreatePlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			PlotData(kDataPlot);
			}
		itsCreatePlotDialog = NULL;
		}

	else if (sender == itsCreateVectorPlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			PlotData(kVectorPlot);
			}
		itsCreateVectorPlotDialog = NULL;
		}

	else if (sender == itsTransDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			EvaluateTransformFunction();
			}
		itsTransDialog = NULL;
		delete itsVarList;
		itsVarList = NULL;
		}

	else if (sender == itsColByIncDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CreateNewColByInc();
			}
		itsColByIncDialog = NULL;
		}

	else if (sender == itsColByRangeDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			CreateNewColByRange();
			}
		itsColByRangeDialog = NULL;
		}

	else if (sender == itsOKButton && message.Is(JXButton::kPushed))
		{
		EndEditing();
		}

	else if (sender == itsFloatData && message.Is(GRaggedFloatTableData::kElementRemoved))
		{
		const GRaggedFloatTableData::ElementRemoved* info =
			dynamic_cast<const GRaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != NULL );
		TableRefreshCol(info->GetCol());
		}
	else if (sender == itsFloatData && message.Is(GRaggedFloatTableData::kElementInserted))
		{
		const GRaggedFloatTableData::ElementRemoved* info =
			dynamic_cast<const GRaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != NULL );
		TableRefreshCol(info->GetCol());
		}
	else if (sender == itsFloatData && message.Is(GRaggedFloatTableData::kDataChanged))
		{
		AdjustToTableData();
		Refresh();
		}

	else
		{
		JXEditTable::Receive(sender, message);
		}
}

/******************************************************************************
 HandleEditMenu

 ******************************************************************************/

void
GXRaggedFloatTable::HandleEditMenu
	(
	const JIndex index
	)
{
	if (index == kUndoCmd)
		{
		Undo();
		}
	else if (index == kRedoCmd)
		{
		Redo();
		}
	else if (index == kCutCmd)
		{
		HandleCutCmd();
		}

	else if (index == kCopyCmd)
		{
		HandleCopyCmd();
		}

	else if (index ==kPasteCmd)
		{
		HandlePasteCmd();
		}

	else if (index == kSpecialPasteCmd)
		{
		HandleSpecialPasteCmd();
		}

	else if (index == kInsertCmd)
		{
		HandleInsertion();
		}

	else if (index == kDuplicateCmd)
		{
		HandleDuplication();
		}

	else if (index == kDeleteCmd)
		{
		HandleDeletion();
		}
}

/******************************************************************************
 UpdateEditMenu

 ******************************************************************************/

void
GXRaggedFloatTable::UpdateEditMenu()
{
	if (HasUndo())
		{
		itsEditMenu->EnableItem(kUndoCmd);
		}
	else
		{
		itsEditMenu->DisableItem(kUndoCmd);
		}
	if (HasRedo())
		{
		itsEditMenu->EnableItem(kRedoCmd);
		}
	else
		{
		itsEditMenu->DisableItem(kRedoCmd);
		}

	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
		{
		itsEditMenu->DisableItem(kCutCmd);
		itsEditMenu->DisableItem(kCopyCmd);
		itsEditMenu->DisableItem(kInsertCmd);
		itsEditMenu->DisableItem(kDuplicateCmd);
		itsEditMenu->DisableItem(kDeleteCmd);
		}

	else
		{
		itsEditMenu->EnableItem(kCutCmd);
		itsEditMenu->EnableItem(kCopyCmd);
		itsEditMenu->EnableItem(kInsertCmd);
		itsEditMenu->EnableItem(kDuplicateCmd);
		itsEditMenu->EnableItem(kDeleteCmd);
		}

/*	const JXWindow* window = GetWindow();

	JBoolean hasGloveData = kJFalse;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName, window, &typeList))
		{
		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
				{
				hasGloveData = kJTrue;
				break;
				}
			}
		}

		if (hasGloveData)
			{
			itsEditMenu->EnableItem(kPasteCmd);
			}
		else
			{
			itsEditMenu->DisableItem(kPasteCmd);
			}*/
}

/******************************************************************************
 HandleCutCmd

 ******************************************************************************/

void
GXRaggedFloatTable::HandleCutCmd()
{
	HandleCopyCmd();
	HandleDeletion();
}

/******************************************************************************
 HandleCopyCmd

 ******************************************************************************/

void
GXRaggedFloatTable::HandleCopyCmd()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
		{
		return;
		}

	std::ostringstream os;

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (startCol + cols == GetColCount() + 1)
		{
		cols--;
		}

	if (startRow + rows == GetRowCount() + 1)
		{
		rows--;
		}

	for (JIndex row = startRow; row < startRow + rows; row++)
		{
		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			JFloat value;
			if (itsFloatData->GetElement(row, col, &value))
				{
				os << value;
				}
			if (col != startCol + cols - 1)
				{
				os << "\t";
				}
			}
		os << endl;
		}

	GXTextSelection* data = new GXTextSelection(GetDisplay(), os.str());
	assert(data != NULL);

	std::ostringstream os2;
	os2 << (int)type << " ";
	os2 << cols << " ";
	for (JSize i = startCol; i < startCol + cols; i++)
		{
		JSize rowCount = itsFloatData->GetDataRowCount(i);
		if (rowCount - startRow + 1 < rows)
			{
			os2 << rowCount - startRow + 1 << " ";
			}
		else
			{
			os2 << rows << " ";
			}
		for (JIndex row = startRow; row < startRow + rows; row++)
			{
			JFloat value;
			if (itsFloatData->GetElement(row, i, &value))
				{
				os2 << value << " ";
				}
			}
		}

	data->SetGloveData(os2.str());

	if (!GetSelectionManager()->SetData(kJXClipboardName, data))
		{
		(JGetUserNotification())->ReportError("Unable to copy to the X Clipboard.");
		}
}

/******************************************************************************
 HandlePasteCmd

 ******************************************************************************/

void
GXRaggedFloatTable::HandlePasteCmd()
{
	const JXWindow* window = GetWindow();

	JBoolean gotData = kJFalse;
	JBoolean hasGloveData = kJFalse;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName,
					CurrentTime, &typeList))
		{

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
				{
				hasGloveData = kJTrue;
				break;
				}
			}

		if (!hasGloveData)
			{
			return;
			}

		unsigned char* data = NULL;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager::DeleteMethod dMethod;
		if (selManager->GetData(kJXClipboardName,
				CurrentTime, itsGloveTextXAtom,
				&returnType, &data, &dataLength, &dMethod))
			{
			std::string s(reinterpret_cast<char*>(data), dataLength);
			std::istringstream is(s);
			JIndex type;
			JSize rows;
			JSize cols;
			is >> type;
			is >> cols;
			is >> rows;

			JFloat value;
			SelectionType type1 = GetSelectionType();

			JIndex rows1;
			JIndex cols1;
			JIndex startRow;
			JIndex startCol;

			GetSelectionArea(&rows1, &cols1, &startRow, &startCol);

			JSize realrowcount;
			JBoolean ok = kJFalse;
			if ((type1 == kElementsSelected) &&
				(cols1 == 1) &&
				((rows1 == 1) || (rows == rows1) ))
				{
				realrowcount = itsFloatData->GetDataRowCount(startCol);
				if (startRow <= realrowcount + 1)
					{
					ok = kJTrue;
					}
				}
			if (ok)
				{
				if (rows1 == 1)
					{
					if (rows == 1)
						{
						is >> value;
						JPoint cell(startCol, startRow);
						JFloat oldvalue;
						if (itsFloatData->GetElement(cell, &oldvalue))
							{
							GLUndoElementChange* undo =
								new GLUndoElementChange(this, cell, oldvalue);
							assert(undo != NULL);
							itsFloatData->SetElement(cell, value);
							NewUndo(undo);
							}
						else
							{
							GLUndoElementAppend* undo =
								new GLUndoElementAppend(this, cell);
							assert(undo != NULL);
							itsFloatData->InsertElement(cell, value);
							NewUndo(undo);
							}
						}
					else
						{
						GLUndoElementsInsert* undo =
							new GLUndoElementsInsert(this, JPoint(startCol, startRow),
													 JPoint(startCol, startRow + rows - 1),
													 GLUndoElementsBase::kElements);
						assert(undo != NULL);
						NewUndo(undo);
						for (JIndex row = startRow; row <= startRow + rows - 1; row++)
							{
							is >> value;
							itsFloatData->InsertElement(row,startCol, value);
							}
						}

					}
				else
					{
					GLUndoElementsChange* undo =
						new GLUndoElementsChange(this, JPoint(startCol, startRow),
												 JPoint(startCol + cols1 - 1, startRow + rows1 - 1),
												 GLUndoElementsBase::kElements);
					assert(undo != NULL);
					NewUndo(undo);
					for (JIndex col = startCol; col < startCol + cols1; col++)
						{
						for (JIndex row = startRow; row <= startRow + rows1 - 1; row++)
							{
							is >> value;
							itsFloatData->SetElement(row,col, value);
							}
						}
					}
				}

			else if (type1 == kNoneSelected ||
				type1 == kRowsSelected ||
				type1 == kElementsSelected)
				{
				const JSize count = itsFloatData->GetDataColCount() + 1;
				GLUndoElementsInsert* undo =
					new GLUndoElementsInsert(this, JPoint(count, 1),
											 JPoint(count + cols - 1, 1),
											 GLUndoElementsBase::kCols);
				assert(undo != NULL);
				NewUndo(undo);
				itsFloatData->InsertCols(count, cols);
				for (JSize i = 1; i <= cols; i++)
					{
					for (JSize j = 1; j <= rows; j++)
						{
						is >> value;
						itsFloatData->SetElement(j, i+count-1, value);
						}
					}
				}

			else
				{
				GLUndoElementsInsert* undo =
					new GLUndoElementsInsert(this, JPoint(startCol, 1),
											 JPoint(startCol + cols - 1, 1),
											 GLUndoElementsBase::kCols);
				assert(undo != NULL);
				NewUndo(undo);
				itsFloatData->InsertCols(startCol, cols);
				for (JSize i = 1; i <= cols; i++)
					{
					for (JSize j = 1; j <= rows; j++)
						{
						is >> value;
						if (is.good())
							{
							itsFloatData->SetElement(j, i+startCol-1, value);
							}
						}
					}

				}
			TableRefresh();

			selManager->DeleteData(&data, dMethod);
			}
		}
}

/******************************************************************************
 HandleSpecialPasteCmd

 ******************************************************************************/

void
GXRaggedFloatTable::HandleSpecialPasteCmd()
{
	const JXWindow* window = GetWindow();

	JBoolean gotData = kJFalse;
	JBoolean hasGloveData = kJFalse;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName,
					CurrentTime, &typeList))
		{

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
				{
				hasGloveData = kJTrue;
				break;
				}
			}

		if (!hasGloveData)
			{
			return;
			}

		unsigned char* data = NULL;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager::DeleteMethod dMethod;
		if (selManager->GetData(kJXClipboardName,
				CurrentTime, itsGloveTextXAtom,
				 &returnType, &data, &dataLength, &dMethod))
			{
			std::string s(reinterpret_cast<char*>(data), dataLength);
			std::istringstream is(s);
			JIndex type;
			JSize rows, cols;
			is >> type;
			is >> cols;

			JFloat value;
			SelectionType type1 = GetSelectionType();

			JIndex rows1;
			JIndex cols1;
			JIndex startRow;
			JIndex startCol;

			GetSelectionArea(&rows1, &cols1, &startRow, &startCol);

			if (type1 == kNoneSelected)
				{
				JGetUserNotification()->ReportError("Nothing is selected.");
				}

			else if (type1 == kRowsSelected)
				{
				if (cols == GetColCount() - 1)
					{
					for (JSize i = 1; i <= cols; i++)
						{
						is >> rows;
						for (JSize j = 1; j <= rows; j++)
							{
							is >> value;
							itsFloatData->InsertElement(j + startRow - 1, i, value);
							}
						}
					}
				else
					{
					JGetUserNotification()->ReportError("The selected area doesn't match the clipboard area.");
					}
				}

			else if (type1 == kElementsSelected)
				{
				if (cols == cols1)
					{
					for (JSize i = 1; i <= cols; i++)
						{
						is >> rows;
						for (JSize j = 1; j <= rows; j++)
							{
							is >> value;
							itsFloatData->InsertElement(j + startRow - 1, i + startCol - 1, value);
							}
						}
					}
				else
					{
					JGetUserNotification()->ReportError("The selected area doesn't match the clipboard area.");
					}
				}

			else
				{

				itsFloatData->InsertCols(startCol, cols);
				for (JSize i = 1; i <= cols; i++)
					{
					is >> rows;
					for (JSize j = 1; j <= rows; j++)
						{
						is >> value;
						if (is.good())
							{
							itsFloatData->SetElement(j, i+startCol-1, value);
							}
						}
					}

				}
			TableRefresh();

			selManager->DeleteData(&data, dMethod);
			}
		}
}


/******************************************************************************
 HandleInsertion

 ******************************************************************************/

void
GXRaggedFloatTable::HandleInsertion
	(
	const JBoolean undo
	)
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
		{
		return;
		}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (type == kRowsSelected)
		{
		if (undo)
			{
			GLUndoElementsInsert* undo =
				new GLUndoElementsInsert(this, JPoint(1, startRow),
										 JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
										 GLUndoElementsBase::kRows);
			assert(undo != NULL);
			NewUndo(undo);
			}
		itsFloatData->InsertRows(startRow, rows);
		}

	else if (type == kColsSelected)
		{
		if (undo)
			{
			GLUndoElementsInsert* undo =
				new GLUndoElementsInsert(this, JPoint(startCol, 1),
										 JPoint(startCol + cols - 1, GetRowCount()),
										 GLUndoElementsBase::kCols);
			assert(undo != NULL);
			NewUndo(undo);
			}
		itsFloatData->InsertCols(startCol, cols);
		}

	else if (type == kElementsSelected)
		{
		if ((cols == 1) && (rows == 1) && undo)
			{
			JPoint cell(startCol, startRow);
			GLUndoElementAppend* undo =
				new GLUndoElementAppend(this, cell);
			assert(undo != NULL);
			NewUndo(undo);
			}
		else if (undo)
			{
			GLUndoElementsInsert* undo =
				new GLUndoElementsInsert(this, JPoint(startCol, startRow),
										 JPoint(startCol + cols - 1, startRow + rows - 1),
										 GLUndoElementsBase::kElements);
			assert(undo != NULL);
			NewUndo(undo);
			}

		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			JIndex end = JMin(startRow + rows - 1, itsFloatData->GetDataRowCount(col));
			for (JIndex row = startRow; row <= end; row++)
				{
				itsFloatData->InsertElement(row,col);
				}
			}
		}
	TableRefresh();
}

/******************************************************************************
 HandleDuplication

 ******************************************************************************/

void
GXRaggedFloatTable::HandleDuplication()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
		{
		return;
		}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);
	HandleInsertion();

	if ( (type == kRowsSelected) || (type == kElementsSelected) )
		{
//		if ((cols == 1) && (rows == 1))
//			{
//			JPoint cell(startCol, startRow + 1);
//			GLUndoElementAppend* undo =
//				new GLUndoElementAppend(this, cell);
//			assert(undo != NULL);
//			NewUndo(undo);
//			}
		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			for (JIndex row = startRow; row < startRow + rows; row++)
				{
				JFloat value;
				if (itsFloatData->GetElement(row+rows, col, &value))
					{
					itsFloatData->SetElement(row,col, value);
					}
				}
			}
		}

	else if (type == kColsSelected)
		{
		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			for (JIndex row = startRow; row < startRow + rows; row++)
				{
				JFloat value;
				if (itsFloatData->GetElement(row, col+cols, &value))
					{
					itsFloatData->SetElement(row,col, value);
					}
				}
			}
		}
	TableRefresh();
}

/******************************************************************************
 HandleDeletion

 ******************************************************************************/

void
GXRaggedFloatTable::HandleDeletion()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
		{
		return;
		}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (startCol + cols == GetColCount() + 1)
		{
		cols--;
		}

	if (startRow + rows == GetRowCount() + 1)
		{
		rows--;
		}

	if (type == kRowsSelected)
		{
		GLUndoElementsCut* undo =
			new GLUndoElementsCut(this, JPoint(1, startRow),
								  JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
								  GLUndoElementsBase::kRows);
		assert(undo != NULL);
		NewUndo(undo);
		for (JIndex i = startRow; i < startRow + rows; i++)
			{
			itsFloatData->RemoveRow(startRow);
			}
		}

	else if (type == kColsSelected)
		{
		GLUndoElementsCut* undo =
			new GLUndoElementsCut(this, JPoint(startCol, 1),
								  JPoint(startCol + cols - 1, GetRowCount()),
								  GLUndoElementsBase::kCols);
		assert(undo != NULL);
		NewUndo(undo);
		for (JIndex i = startCol; i < startCol + cols; i++)
			{
			itsFloatData->RemoveCol(startCol);
			}
		}

	else if (type == kElementsSelected)
		{
		// special case if just one cell - use special undo
		if ((cols == 1) && (rows == 1))
			{
			JPoint cell(startCol, startRow);
			JFloat value;
			if (itsFloatData->GetElement(cell, &value))
				{
				GLUndoElementCut* undo =
					new GLUndoElementCut(this, cell, value);
				assert(undo != NULL);
				NewUndo(undo);
				}
			}
		else
			{
			GLUndoElementsCut* undo =
				new GLUndoElementsCut(this, JPoint(startCol, startRow),
									  JPoint(startCol + cols - 1, startRow + rows - 1),
									  GLUndoElementsBase::kElements);
			assert(undo != NULL);
			NewUndo(undo);
			}
		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			for (JIndex row = startRow; row < startRow + rows; row++)
				{
				if (itsFloatData->CellValid(startRow,col))
					{
					itsFloatData->RemoveElement(startRow,col);
					}
				}
			}
		}

	(GetTableSelection()).ClearSelection();
	TableRefresh();
}

/******************************************************************************
 GetSelectionType

 ******************************************************************************/

GXRaggedFloatTable::SelectionType
GXRaggedFloatTable::GetSelectionType()
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint cell;
	if (selection.GetFirstSelectedCell(&cell))
		{
		if (cell.x > (JCoordinate)(itsFloatData->GetDataColCount()))
			{
			return kNoneSelected;
			}
		}
	else
		{
		return kNoneSelected;
		}

	JBoolean found = iter.Next(&cell);
	if (found)
		{
		if (cell.y == 1)
			{
			const JSize rowCount = GetRowCount();
			JBoolean colSelected = kJTrue;
			JIndex row = 2;
			while (row <= rowCount && colSelected)
				{
				if (!((GetTableSelection()).IsSelected(row,cell.x)))
					{
					colSelected = kJFalse;
					}
				row++;
				}

			if (colSelected)
				{
				return kColsSelected;
				}

			if (cell.x != 1)
				{
				return kElementsSelected;
				}

			const JSize colCount = GetColCount();
			JBoolean rowSelected = kJTrue;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
				{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
					{
					rowSelected = kJFalse;
					}
				col++;
				}

			if (rowSelected)
				{
				return kRowsSelected;
				}

			else
				{
				return kElementsSelected;
				}
			}

		else if (cell.x == 1)
			{
			const JSize colCount = GetColCount();
			JBoolean rowSelected = kJTrue;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
				{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
					{
					rowSelected = kJFalse;
					}
				col++;
				}

			if (rowSelected)
				{
				return kRowsSelected;
				}

			else
				{
				return kElementsSelected;
				}

			}

		else
			{
			return kElementsSelected;
			}
		}

	return kNoneSelected;
}

/******************************************************************************
 GetSelectionArea

 ******************************************************************************/

void
GXRaggedFloatTable::GetSelectionArea
	(
	JIndex* rows,
	JIndex* cols,
	JIndex* startRow,
	JIndex* startCol
	)
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint cell;
	const JBoolean found = iter.Next(&cell);

	if (!found)
		{
		*rows = 1;
		*cols = 1;
		return;
		}

	*startRow = cell.y;
	*startCol = cell.x;

	JPoint cell2;

	iter.MoveTo(kJIteratorStartAtEnd,0,0);
	iter.Prev(&cell2);

	*rows = cell2.y - cell.y + 1;
	*cols = cell2.x - cell.x + 1;
}

/******************************************************************************
 UpdateDataMenu

 ******************************************************************************/

void
GXRaggedFloatTable::UpdateDataMenu()
{
	if (itsFloatData->GetDataColCount() == 0)
		{
		itsDataMenu->DisableItem(kPlotCmd);
		itsDataMenu->DisableItem(kTransCmd);
		}
	else
		{
		itsDataMenu->EnableItem(kPlotCmd);
		itsDataMenu->EnableItem(kTransCmd);
		}
}

/******************************************************************************
 HandleDataMenu

 ******************************************************************************/

void
GXRaggedFloatTable::HandleDataMenu
	(
	const JIndex index
	)
{
	if (index == kPlotCmd)
		{
		ChoosePlotColumns(kPlotCmd);
		}

	else if (index == kPlotVectorCmd)
		{
		ChoosePlotColumns(kPlotVectorCmd);
		}

	else if (index == kTransCmd)
		{
		ChooseNewTransformFunction();
		}

	else if (index == kGenerateRangeColCmd)
		{
		GetNewColByRange();
		}

	else if (index == kGenerateIncrementColCmd)
		{
		GetNewColByInc();
		}

	else if (index == kDataModuleCmd)
		{
		JString modName;
		if (JGetChooseSaveFile()->ChooseFile("Select data module", "", &modName))
			{
			DataModule* dm;
			DataModule::Create(&dm, this, itsFloatData, modName);
			}
		}
}

/******************************************************************************
 HandleModuleMenu

 ******************************************************************************/

void
GXRaggedFloatTable::HandleModuleMenu
	(
	const JIndex index
	)
{
	if (index == kReloadModuleCmd)
		{
		UpdateModuleMenu();
		}
	else
		{
		JString modName;
		(GLGetApplication())->GetDataModulePath(index - 1, &modName);
		DataModule* dm;
		DataModule::Create(&dm, this, itsFloatData, modName);
		}
}

/******************************************************************************
 UpdateModuleMenu

 ******************************************************************************/

void
GXRaggedFloatTable::UpdateModuleMenu()
{
	const JSize mCount = itsModuleMenu->GetItemCount();
	JSize i;
	for (i = 2; i <= mCount; i++)
		{
		itsModuleMenu->RemoveItem(2);
		}

	(GLGetApplication())->ReloadDataModules();
	JPtrArray<JString>* names = (GLGetApplication())->GetDataModules();
	for (i = 1; i <= names->GetElementCount(); i++)
		{
		itsModuleMenu->AppendItem(*(names->NthElement(i)));
		}
}

/******************************************************************************
 ChoosePlotColumns

 ******************************************************************************/

void
GXRaggedFloatTable::ChoosePlotColumns
	(
	const JIndex type
	)
{
	if (itsFloatData->GetDataColCount() == 0)
		{
		JGetUserNotification()->ReportError("You have no data to plot.");
		return;
		}

	SelectionType selType = GetSelectionType();

	JIndex xCol;
	JIndex x2Col;
	JIndex yCol;
	JIndex y2Col;

	xCol = 0;
	x2Col = 0;
	yCol = 0;
	y2Col = 0;

	if (selType != kNoneSelected)
		{
		JIndex rows;
		JIndex cols;
		JIndex startRow;
		JIndex startCol;

		GetSelectionArea(&rows, &cols, &startRow, &startCol);

		xCol = startCol;
		if (cols > 1)
			{
			yCol = startCol+1;
			}
		if (cols > 2)
			{
			y2Col = startCol + 2;
			}
		if (cols > 3)
			{
			x2Col = startCol + 3;
			}
		}

	if (type == kPlotCmd)
		{
		assert (itsCreatePlotDialog == NULL);
		itsCreatePlotDialog =
			new GXCreatePlotDialog(itsTableDir, itsFloatData, xCol,x2Col,yCol,y2Col);
		assert (itsCreatePlotDialog != NULL);
		ListenTo(itsCreatePlotDialog);
		itsCreatePlotDialog->BeginDialog();
		}
	else if (type == kPlotVectorCmd)
		{
		assert (itsCreateVectorPlotDialog == NULL);
		itsCreateVectorPlotDialog =
			new GXCreateVectorPlotDialog(itsTableDir, itsFloatData, xCol, yCol, y2Col, x2Col);
		assert (itsCreateVectorPlotDialog != NULL);
		ListenTo(itsCreateVectorPlotDialog);
		itsCreateVectorPlotDialog->BeginDialog();
		}
}

/******************************************************************************
 PlotData

 ******************************************************************************/

void
GXRaggedFloatTable::PlotData
	(
	const JIndex type
	)
{
	if (type == kDataPlot)
		{
		assert ( itsCreatePlotDialog != NULL );
		}
	else if (type == kVectorPlot)
		{
		assert ( itsCreateVectorPlotDialog != NULL );
		}

	JIndex xCol, x2Col, yCol, y2Col;
	if (type == kDataPlot)
		{
		itsCreatePlotDialog->GetColumns(&xCol, &x2Col, &yCol, &y2Col);
		}
	else if (type == kVectorPlot)
		{
		itsCreateVectorPlotDialog->GetColumns(&xCol, &yCol, &x2Col, &y2Col);
		}

	const JArray<JFloat>& xData = itsFloatData->GetColPointer(xCol);
	const JArray<JFloat>& yData = itsFloatData->GetColPointer(yCol);

	const JArray<JFloat>* xErr = NULL;
	if (x2Col != 0)
		{
		xErr = &(itsFloatData->GetColPointer(x2Col));
		}

	const JArray<JFloat>* yErr = NULL;
	if (y2Col != 0)
		{
		yErr = &(itsFloatData->GetColPointer(y2Col));
		}

	JIndex index;
	JBoolean oldPlot;
	JString label;
	if (type == kDataPlot)
		{
		label = itsCreatePlotDialog->GetLabel();
		oldPlot = itsCreatePlotDialog->GetPlotIndex(&index);
		}
	else if (type == kVectorPlot)
		{
		label = itsCreateVectorPlotDialog->GetLabel();
		oldPlot = itsCreateVectorPlotDialog->GetPlotIndex(&index);
		}

	if (oldPlot)
		{
		itsTableDir->AddToPlot(index, type, xData, xErr, yData, yErr, kJTrue, label);
		}
	else
		{
		itsTableDir->CreateNewPlot(type, xData, xErr, yData, yErr, kJTrue, label);
		}
}

/******************************************************************************
 GetNewColByRange

 ******************************************************************************/

void
GXRaggedFloatTable::GetNewColByRange()
{
	assert (itsColByRangeDialog == NULL);
	itsColByRangeDialog =
		new GXColByRangeDialog(GetWindow()->GetDirector(), itsFloatData->GetDataColCount() + 1);
	assert (itsColByRangeDialog != NULL);
	ListenTo(itsColByRangeDialog);
	itsColByRangeDialog->BeginDialog();

}

/******************************************************************************
 CreateNewColByRange

 ******************************************************************************/

void
GXRaggedFloatTable::CreateNewColByRange()
{
	assert( itsColByRangeDialog != NULL );

	JIndex dest;
	itsColByRangeDialog->GetDestination(&dest);
	JFloat beg;
	JFloat end;
	JInteger count;
	itsColByRangeDialog->GetValues(&beg, &end, &count);
	JBoolean ascending = itsColByRangeDialog->IsAscending();

	JBoolean replace = kJFalse;
	JSize colCount = itsFloatData->GetDataColCount();
	if (dest <= colCount)
		{
		replace = JGetUserNotification()->AskUserYes("Replace destination column?");
		}

	if (!replace)
		{
		GLUndoElementsInsert* undo =
			new GLUndoElementsInsert(this, JPoint(dest, 1),
									 JPoint(dest, 1),
									 GLUndoElementsBase::kCols);
		assert(undo != NULL);
		NewUndo(undo);
		itsFloatData->InsertCols(dest, 1);
		}
	else if (dest <= colCount)
		{
		GLUndoElementsChange* undo =
			new GLUndoElementsChange(this, JPoint(dest, 1),
									 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
									 GLUndoElementsBase::kCols);
		assert(undo != NULL);
		NewUndo(undo);
		itsFloatData->RemoveAllElements(dest);
		}

	// this takes care of whether or not it was ascending.
	JFloat inc = (end - beg)/(JFloat)( count - 1 );

	for (JInteger i = 1; i <= count; i++)
		{
		itsFloatData->SetElement(i, dest, beg + inc*(i-1));
		}
}

/******************************************************************************
 GetNewColByInc

 ******************************************************************************/

void
GXRaggedFloatTable::GetNewColByInc()
{
	assert (itsColByIncDialog == NULL);
	itsColByIncDialog =
		new GXColByIncDialog(GetWindow()->GetDirector(), itsFloatData->GetDataColCount() + 1);
	assert (itsColByIncDialog != NULL);
	ListenTo(itsColByIncDialog);
	itsColByIncDialog->BeginDialog();
}

/******************************************************************************
 CreateNewColByInc

 ******************************************************************************/

void
GXRaggedFloatTable::CreateNewColByInc()
{
	assert( itsColByIncDialog != NULL );

	JIndex dest;
	itsColByIncDialog->GetDestination(&dest);
	JFloat beg;
	JFloat inc;
	JInteger count;
	itsColByIncDialog->GetValues(&beg, &inc, &count);
	JBoolean ascending = itsColByIncDialog->IsAscending();

	JBoolean replace = kJFalse;
	JSize colCount = itsFloatData->GetDataColCount();
	if (dest <= colCount)
		{
		replace = JGetUserNotification()->AskUserYes("Replace destination column?");
		}

	if (!replace)
		{
		GLUndoElementsInsert* undo =
			new GLUndoElementsInsert(this, JPoint(dest, 1),
									 JPoint(dest, 1),
									 GLUndoElementsBase::kCols);
		assert(undo != NULL);
		NewUndo(undo);
		itsFloatData->InsertCols(dest, 1);
		}
	else if (dest <= colCount)
		{
		GLUndoElementsChange* undo =
			new GLUndoElementsChange(this, JPoint(dest, 1),
									 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
									 GLUndoElementsBase::kCols);
		assert(undo != NULL);
		NewUndo(undo);
		itsFloatData->RemoveAllElements(dest);
		}

	for (JInteger i = 1; i <= count; i++)
		{
		itsFloatData->SetElement(i, dest, beg + inc*(i-1));
		}
}

/******************************************************************************
 WriteData

 ******************************************************************************/

void
GXRaggedFloatTable::WriteData
	(
	ostream& os
	)
{
	os << kCurrentTableVersion << ' ';
	const JSize colCount = itsFloatData->GetDataColCount();
	os << colCount << ' ';
	JString temp("column x");
	for (JSize i = 1; i <= colCount; i++)
		{
		os << temp << ' ';
		const JSize rowCount = itsFloatData->GetDataRowCount(i);
		os << rowCount << ' ';
		for (JSize j = 1; j <= rowCount; j++)
			{
			JFloat value;
			itsFloatData->GetElement(j, i, &value);
			os << value << ' ';
			}
		}
}

/******************************************************************************
 ReadData

 ******************************************************************************/

void
GXRaggedFloatTable::ReadData
	(
	istream& 		is,
	const JFloat	gloveVersion
	)
{
	JFileVersion vers = 0;
	if (gloveVersion > 0.5)
		{
		is >> vers;
		assert(vers <= kCurrentTableVersion);
		}

	itsFloatData->ShouldBroadcast(kJFalse);

	JSize colCount;
	is >> colCount;
	JString temp;
	JSize rowCount;
	for (JSize i= 1; i <= colCount; i++)
		{
		is >> temp;
		is >> rowCount;
		for (JSize j = 1; j <= rowCount; j++)
			{
			JFloat value;
			is >> value;
			itsFloatData->SetElement(j, i, value);
			}
		}

	itsFloatData->ShouldBroadcast(kJTrue);
}

/******************************************************************************
 ChooseNewTransformFunction

 ******************************************************************************/

void
GXRaggedFloatTable::ChooseNewTransformFunction()
{
	assert (itsTransDialog == NULL);
	assert (itsVarList == NULL);

	const JSize count = itsFloatData->GetDataColCount() + 1;
	if (count == 1)
		{
		JGetUserNotification()->ReportError("You have no data to transform.");
		return;
		}

	itsVarList = new GVarList();
	JArray<JFloat>* ar = new JArray<JFloat>;
	for (JSize i = 1; i < count; i++)
		{
		ar->AppendElement(0);
		}
	itsVarList->AddArray("col",*ar);

	itsTransDialog =
		new GXTransformFunctionDialog(GetWindow()->GetDirector(), itsVarList, count);
	assert (itsTransDialog != NULL);
	ListenTo(itsTransDialog);
	itsTransDialog->BeginDialog();
}

/******************************************************************************
 EvaluateTransformFunction

 ******************************************************************************/

void
GXRaggedFloatTable::EvaluateTransformFunction()
{
	assert (itsTransDialog != NULL);
	JIndex dest =  itsTransDialog->GetDestination();
	JString fnStr(itsTransDialog->GetFunctionString());

	const JSize count = itsFloatData->GetDataColCount();
	JBoolean replace = kJFalse;
	if (dest <= count)
		{
		replace = JGetUserNotification()->AskUserYes("Replace destination column?");
		}

	JArray<JFloat> newArray;

	JFunction* f;
	if (JParseFunction(fnStr,itsVarList,&f))
		{
		JFloat val = 0;
		JExprNodeList* nl = new JExprNodeList(f);
		const JSize count = nl->GetElementCount();
		JArray<JIndex>* inds = new JArray<JIndex>;
		inds->SetCompareFunction(JCompareIndices);
		JIndex colArrayIndex;
		for (JSize i = 1; i <= count; i++)
			{
			if (nl->GetNodeType(i) == kJFunctionNode)
				{
				JFunction* tf = nl->GetFunction(i);
				JFunctionWithVar* sf = tf->CastToJFunctionWithVar();
				if (nl->GetFunctionType(i) == kJVariableValueType)
					{
					const JFunction* ai = sf->GetArrayIndex();
					if (ai != NULL)
						{
						colArrayIndex = sf->GetVariableIndex();
						JString nstr(ai->Print());
						JFloat temp;
						nstr.ConvertToFloat(&temp);
						JIndex ti = (JIndex)temp;
						JIndex junk;
						if (!inds->SearchSorted(ti, JOrderedSetT::kAnyMatch, &junk))
							{
							inds->InsertSorted(ti, kJFalse);
							}
						}
					}
				}
			}
		const JSize indCount = inds->GetElementCount();
		if (indCount == 0)
			{
			JGetUserNotification()->ReportError("Use \"Generate column\" if you aren't transforming column values.");
			delete f;
			return;
			}
		JSize minRowCount = itsFloatData->GetDataRowCount(inds->GetElement(1));
		for (JIndex i = 2; i <= indCount; i++)
			{
			const JSize rowCount = itsFloatData->GetDataRowCount(inds->GetElement(i));
			if (rowCount < minRowCount)
				{
				minRowCount = rowCount;
				}
			}
		for (JSize r = 1; r <= minRowCount; r++)
			{
			for (JIndex i = 1; i <= indCount; i++)
				{
				JFloat value;
				itsFloatData->GetElement(r, inds->GetElement(i), &value);
				itsVarList->SetNumericValue(colArrayIndex, inds->GetElement(i), value);
				}
			JFloat value;
			f->Evaluate(&value);
			newArray.AppendElement(value);
			}
		if (!replace)
			{
			GLUndoElementsInsert* undo =
				new GLUndoElementsInsert(this, JPoint(dest, 1),
										 JPoint(dest, 1),
										 GLUndoElementsBase::kCols);
			assert(undo != NULL);
			NewUndo(undo);
			itsFloatData->InsertCols(dest, 1);
			itsFloatData->SetCol(dest, newArray);
			}
		else
			{
			GLUndoElementsChange* undo =
				new GLUndoElementsChange(this, JPoint(dest, 1),
										 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
										 GLUndoElementsBase::kCols);
			assert(undo != NULL);
			NewUndo(undo);
			itsFloatData->RemoveAllElements(dest);
			itsFloatData->SetCol(dest, newArray);
			}
		}
	delete f;
}

/******************************************************************************
 WriteDataCols

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::WriteDataCols
	(
	ostream& os,
	const int cols
	)
{
	SelectionType type = GetSelectionType();
	if (type != kColsSelected)
		{
		JString str = "This module requires " + JString(cols);
		if (cols == 1)
			{
			str += " column to be selected.";
			}
		else
			{
			str += " columns to be selected.";
			}
		JGetUserNotification()->ReportError(str);
		return kJFalse;
		}
	JIndex nrows;
	JIndex ncols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&nrows, &ncols, &startRow, &startCol);
	if (startCol + ncols == GetColCount() + 1)
		{
		ncols--;
		}

	if (startRow + nrows == GetRowCount() + 1)
		{
		nrows--;
		}

	if (cols > (int)ncols)
		{
		JString str = "This module requires " + JString(cols) + " columns to be selected.";
		JGetUserNotification()->ReportError(str);
		return kJFalse;
		}
	os << nrows << endl;
	for (JIndex row = startRow; row < startRow + nrows; row++)
		{
		for (JIndex col = startCol; col < startCol + ncols; col++)
			{
			JFloat value;
			if (itsFloatData->GetElement(row, col, &value))
				{
				os << value;
				}
			if (col != startCol + ncols - 1)
				{
				os << "\t";
				}
			}
		os << endl;
		}
	return kJTrue;
}

/******************************************************************************
 ExportDataMatrix

 ******************************************************************************/

void
GXRaggedFloatTable::ExportDataMatrix
	(
	ostream& os
	)
{
	JSize rowCount = GetRowCount() - 1;
	JSize colCount = GetColCount() - 1;
	os << colCount << " ";
	os << rowCount << " ";
	for (JSize i = 1; i <= rowCount; i++)
		{
		for (JSize j = 1; j <= colCount; j++)
			{
			JFloat value;
			if (itsFloatData->GetElement(i, j, &value))
				{
				os << value << " ";
				}
			else
				{
				os << "0" << " ";
				}
			}
		}
}

/******************************************************************************
 ExportData

 ******************************************************************************/

void
GXRaggedFloatTable::ExportData
	(
	ostream& os
	)
{
	JSize colCount = GetColCount() - 1;
	os << colCount << " ";
	for (JSize i = 1; i <= colCount; i++)
		{
		JSize rowCount = itsFloatData->GetDataRowCount(i);
		os << rowCount << " ";
		for (JSize j = 1; j <= rowCount; j++)
			{
			JFloat value;
			if (itsFloatData->GetElement(j, i, &value))
				{
				os << value << " ";
				}
			else
				{
				os << "0" << " ";
				}
			}
		}
}

/******************************************************************************
 PrintRealTable (public)

 ******************************************************************************/

void
GXRaggedFloatTable::PrintRealTable
	(
	JPSPrinter& p
	)
{
	JSize cols = itsFloatData->GetDataColCount();
	JSize maxrows = 0;
	for (JSize i = 1; i <= cols; i++)
		{
		JSize rows = itsFloatData->GetDataRowCount(i);
		if (rows > maxrows)
			{
			maxrows = rows;
			}
		}
	JSize tableRowCount = GetRowCount();
	JSize tableColCount = GetColCount();

	while (GetColCount() > cols)
		{
		RemoveCol(GetColCount());
		}

	while (GetRowCount() > maxrows)
		{
		RemoveRow(GetRowCount());
		}

	const JColorIndex gray50Color = GetColormap()->GetGrayColor(50);
	SetRowBorderInfo(0, gray50Color);
	SetColBorderInfo(0, gray50Color);

	Print(p, kJFalse, kJFalse);

	while (GetColCount() < tableColCount)
		{
		AppendCols(kDefColWidth, 1);
		}
	while(GetRowCount() < tableRowCount)
		{
		AppendRows(kDefRowWidth, 1);
		}
	SetRowBorderInfo(1, gray50Color);	// calls TableSetScrollSteps()
	SetColBorderInfo(1, gray50Color);
}

/******************************************************************************
 Undo (public)

	This is the function that is called when the user asks to undo.

 ******************************************************************************/

void
GXRaggedFloatTable::Undo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an undo object available.
	JUndo* undo;
	const JBoolean hasUndo = GetCurrentUndo(&undo);

	// Perform the undo.
	if (hasUndo)
		{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 Redo (public)

	This is the function that is called when the user asks to redo.

 ******************************************************************************/

void
GXRaggedFloatTable::Redo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an redo object available.
	JUndo* undo;
	const JBoolean hasUndo = GetCurrentRedo(&undo);

	// Perform the redo.
	if (hasUndo)
		{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::GetCurrentUndo
	(
	JUndo** undo
	)
	const
{
	if (HasUndo())
		{
		*undo = itsUndoList->NthElement(itsFirstRedoIndex - 1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::GetCurrentRedo
	(
	JUndo** redo
	)
	const
{
	if (HasRedo())
		{
		*redo = itsUndoList->NthElement(itsFirstRedoIndex);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HasUndo (public)

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::HasUndo()
	const
{
	return JConvertToBoolean(itsFirstRedoIndex > 1);
}

/******************************************************************************
 HasRedo (public)

 ******************************************************************************/

JBoolean
GXRaggedFloatTable::HasRedo()
	const
{
	return JConvertToBoolean(itsFirstRedoIndex <= itsUndoList->GetElementCount());
}

/******************************************************************************
 NewUndo (private)

 ******************************************************************************/

void
GXRaggedFloatTable::NewUndo
	(
	JUndo* undo
	)
{
	if (itsUndoList != NULL && itsUndoState == kIdle)
		{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
			{
			itsUndoList->DeleteElement(i);
			}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		assert( !itsUndoList->IsEmpty() );
		}

	else if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		JUndo* oldUndo = itsUndoList->NthElement(itsFirstRedoIndex);
//		delete oldUndo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(kJTrue);
		undo->Deactivate();
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		JUndo* oldRedo = itsUndoList->NthElement(itsFirstRedoIndex);
//		delete oldRedo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		undo->Deactivate();
		}

}
