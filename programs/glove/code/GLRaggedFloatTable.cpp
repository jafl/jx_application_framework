/******************************************************************************
 GLRaggedFloatTable.cpp

	Draws a table of numbers stored in a GLRaggedFloatTableData object.

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLRaggedFloatTable.h"
#include "GLRaggedFloatTableData.h"
#include "GLColByIncDialog.h"
#include "GLColByRangeDialog.h"
#include "GLDataDocument.h"
#include "GLTransformFunctionDialog.h"
#include "GLVarList.h"
#include "GLDataModule.h"
#include "GLCreatePlotDialog.h"
#include "GLCreateVectorPlotDialog.h"
#include "GLPlotApp.h"
#include <GLTextSelection.h>
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
#include <JXColorManager.h>
#include <JXDisplay.h>
#include <JXFloatInput.h>
#include <JXImage.h>
#include <JXMenuBar.h>
#include <JXSelectionManager.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXWindowDirector.h>

#include <JExprParser.h>
#include <JFunctionWithVar.h>
#include <JFunctionWithArgs.h>
#include <JPainter.h>
#include <JFontStyle.h>
#include <JUserNotification.h>
#include <JChooseSaveFile.h>
#include <JTableSelection.h>
#include <JTableSelectionIterator.h>
#include <JFontManager.h>
#include <JString.h>
#include <JPSPrinter.h>
#include <JListUtil.h>

#include <jXActionDefs.h>

#include <jStreamUtil.h>
#include <jASCIIConstants.h>
#include <sstream>

#include <jMath.h>
#include <jAssert.h>

// Setup information

const JFileVersion	kCurrentTableVersion = 0;

const JCoordinate kDefColWidth  = 100;
const JCoordinate kDefRowWidth 	= 20;
const JCoordinate kHMarginWidth = 2;

// Edit menu information

static const JUtf8Byte* kEditMenuStr =
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


static const JUtf8Byte* kDataMenuStr =
	"    Plot data... %i Plot::GLRaggedFloatTable"
	"  | Plot vector field...  %i PlotVector::GLRaggedFloatTable"
	"%l| Transform... %i Transform::GLRaggedFloatTable"
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

static const JUtf8Byte* kModuleMenuStr =
	"Reload %l";

enum
{
	kReloadModuleCmd = 1
};

// Selection Targets for Cut/Copy/Paste

static const JUtf8Byte* kGloveTextDataXAtomName = "GLOVE_TEXT";

/******************************************************************************
 Constructor

 ******************************************************************************/

GLRaggedFloatTable::GLRaggedFloatTable
	(
	GLDataDocument*			dir,
	JXTextButton* 			okButton,
	GLRaggedFloatTableData*	data,
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
	assert( data != nullptr );

	ListenTo(itsOKButton);
	itsOKButton->Deactivate();

	itsFloatData = data;

	itsFloatInputField = nullptr;

	itsEditMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXGlobal"));
	itsEditMenu->SetMenuItems(kEditMenuStr);
	itsEditMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditMenu);


	auto* image = jnew JXImage(GetDisplay(), JXPM(editcut));
	assert(image != nullptr);
	itsEditMenu->SetItemImage(kCutCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(editcopy));
	assert(image != nullptr);
	itsEditMenu->SetItemImage(kCopyCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(editpaste));
	assert(image != nullptr);
	itsEditMenu->SetItemImage(kPasteCmd, image, true);

	itsDataMenu = menuBar->AppendTextMenu(JGetString("DataMenuTitle::GLRaggedFloatTable"));
	itsDataMenu->SetMenuItems(kDataMenuStr);
	itsDataMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDataMenu);


	image = jnew JXImage(GetDisplay(), JXPM(plotdata));
	assert(image != nullptr);
	itsDataMenu->SetItemImage(kPlotCmd, image, true);

	image = jnew JXImage(GetDisplay(), JXPM(plotvectordata));
	assert(image != nullptr);
	itsDataMenu->SetItemImage(kPlotVectorCmd, image, true);


	image = jnew JXImage(GetDisplay(), JXPM(glv_transform));
	assert(image != nullptr);
	itsDataMenu->SetItemImage(kTransCmd, image, true);

	itsModuleMenu = jnew JXTextMenu(itsDataMenu, kDataModuleCmd, menuBar);
	assert( itsModuleMenu != nullptr );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsModuleMenu);

	itsCreatePlotDialog 		= nullptr;
	itsCreateVectorPlotDialog	= nullptr;
	itsColByRangeDialog 		= nullptr;
	itsColByIncDialog 			= nullptr;
	itsTransDialog 				= nullptr;
	itsTransformVarList			= nullptr;

	itsFirstRedoIndex   		= 1;
	itsUndoState        		= kIdle;

	itsUndoList = jnew JPtrArray<JUndo>(JPtrArrayT::kDeleteAll);
	assert(itsUndoList != nullptr);

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

GLRaggedFloatTable::~GLRaggedFloatTable()
{
	jdelete itsEditMenu;
	jdelete itsDataMenu;

	itsUndoList->DeleteAll();
	jdelete itsUndoList;
}

/******************************************************************************
 LoadDefaultToolButtons (public)

 ******************************************************************************/

void
GLRaggedFloatTable::LoadDefaultToolButtons
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
GLRaggedFloatTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if ((GetTableSelection()).IsSelected(cell))
		{
		p.SetFilling(true);
		p.SetPenColor(JColorManager::GetDefaultSelectionColor());
		p.Rect(rect);
		p.SetFilling(false);
		p.SetPenColor(JColorManager::GetBlackColor());
		}

	JPoint editCell;
	if (!GetEditedCell(&editCell) || cell != editCell)
		{
		p.SetFont(JFontManager::GetDefaultFont());

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
GLRaggedFloatTable::HandleMouseDown
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

	const bool extendSelection = modifiers.shift();

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
GLRaggedFloatTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ScrollForDrag(pt);
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
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
GLRaggedFloatTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJEscapeKey)
		{
		(GetTableSelection()).ClearSelection();
		TableRefresh();
		}
	JPoint cell;
	JTableSelection& selection = GetTableSelection();
	if (!GetEditedCell(&cell) && selection.HasSelection())
		{
		JTableSelectionIterator iter(&selection);
		iter.Next(&cell);

		if (c == kJReturnKey || keySym == XK_KP_Enter)
			{
			selection.ClearSelection();
			BeginEditing(cell);
			}
		else if (c == kJUpArrow)
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

		else if (c == kJDownArrow)
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
		else if (c == kJLeftArrow)
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
		else if (c == kJRightArrow)
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
			JXEditTable::HandleKeyPress(c, keySym, modifiers);
			}
		}
	else
		{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
		}

}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GLRaggedFloatTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsFloatInputField == nullptr );
	itsOKButton->Activate();

	itsFloatInputField =
		jnew JXFloatInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsFloatInputField != nullptr );

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

	Returns true if the data is valid and the process succeeded.

 ******************************************************************************/

bool
GLRaggedFloatTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsFloatInputField != nullptr );

	if (itsFloatInputField->InputValid())
		{
		JFloat value;
		const bool valid = itsFloatInputField->GetValue(&value);
		assert( valid );

		// save old value for undo
		JFloat oldvalue;
		bool exists = itsFloatData->GetElement(cell, &oldvalue);

		// set to new value
		itsFloatData->SetElement(cell, value);

		// create and install undo object with old value
		if (exists)
			{
			auto* undo =
				jnew GLUndoElementChange(this, cell, oldvalue);
			assert(undo != nullptr);
			NewUndo(undo);
			}
		else
			{
			auto* undo =
				jnew GLUndoElementAppend(this, cell);
			assert(undo != nullptr);
			NewUndo(undo);
			}

		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLRaggedFloatTable::PrepareDeleteXInputField()
{
	itsFloatInputField = nullptr;
	itsOKButton->Deactivate();
}

/******************************************************************************
 ExtendSelection

 ******************************************************************************/

void
GLRaggedFloatTable::ExtendSelection
	(
	const JPoint cell
	)
{
	auto* iter =
		jnew JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != nullptr);

	JPoint sCell;
	bool success = iter->Next(&sCell);
	if (!success)
		{
		jdelete iter;
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

	jdelete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCell

 ******************************************************************************/

void
GLRaggedFloatTable::SelectCell
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
GLRaggedFloatTable::SelectRow
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
GLRaggedFloatTable::AddRowToSelection
	(
	const JIndex row
	)
{

}

/******************************************************************************
 ExtendSelectionToRow

 ******************************************************************************/

void
GLRaggedFloatTable::ExtendSelectionToRow
	(
	const JIndex row
	)
{
	auto* iter =
		jnew JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != nullptr);

	JPoint sCell;
	bool success = iter->Next(&sCell);
	if (!success)
		{
		jdelete iter;
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

	jdelete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCol

 ******************************************************************************/

void
GLRaggedFloatTable::SelectCol
	(
	const JIndex col
	)
{
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
GLRaggedFloatTable::AddColToSelection
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
GLRaggedFloatTable::ExtendSelectionToCol
	(
	const JIndex col
	)
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint sCell;
	bool success = iter.Next(&sCell);
	if (!success)
		{
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

	TableRefresh();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GLRaggedFloatTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEditMenu(selection->GetIndex());
		}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditMenu();
		}

	else if (sender == itsDataMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDataMenu(selection->GetIndex());
		}

	else if (sender == itsDataMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDataMenu();
		}

	else if (sender == itsModuleMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleModuleMenu(selection->GetIndex());
		}

	else if (sender == itsCreatePlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			PlotData(kDataPlot);
			}
		itsCreatePlotDialog = nullptr;
		}

	else if (sender == itsCreateVectorPlotDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			PlotData(kVectorPlot);
			}
		itsCreateVectorPlotDialog = nullptr;
		}

	else if (sender == itsTransDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			EvaluateTransformFunction();
			}
		itsTransDialog = nullptr;
		jdelete itsTransformVarList;
		itsTransformVarList = nullptr;
		}

	else if (sender == itsColByIncDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CreateNewColByInc();
			}
		itsColByIncDialog = nullptr;
		}

	else if (sender == itsColByRangeDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			CreateNewColByRange();
			}
		itsColByRangeDialog = nullptr;
		}

	else if (sender == itsOKButton && message.Is(JXButton::kPushed))
		{
		EndEditing();
		}

	else if (sender == itsFloatData && message.Is(GLRaggedFloatTableData::kElementRemoved))
		{
		const auto* info =
			dynamic_cast<const GLRaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != nullptr );
		TableRefreshCol(info->GetCol());
		}
	else if (sender == itsFloatData && message.Is(GLRaggedFloatTableData::kElementInserted))
		{
		const auto* info =
			dynamic_cast<const GLRaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != nullptr );
		TableRefreshCol(info->GetCol());
		}
	else if (sender == itsFloatData && message.Is(GLRaggedFloatTableData::kDataChanged))
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
GLRaggedFloatTable::HandleEditMenu
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
GLRaggedFloatTable::UpdateEditMenu()
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

	bool hasGloveData = false;
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
				hasGloveData = true;
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
GLRaggedFloatTable::HandleCutCmd()
{
	HandleCopyCmd();
	HandleDeletion();
}

/******************************************************************************
 HandleCopyCmd

 ******************************************************************************/

void
GLRaggedFloatTable::HandleCopyCmd()
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
		os << std::endl;
		}

	auto* data = jnew GLTextSelection(GetDisplay(), os.str());
	assert(data != nullptr);

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
		JGetUserNotification()->ReportError(JGetString("CopyFailed::GLRaggedFloatTable"));
		}
}

/******************************************************************************
 HandlePasteCmd

 ******************************************************************************/

void
GLRaggedFloatTable::HandlePasteCmd()
{
	bool hasGloveData = false;
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
				hasGloveData = true;
				break;
				}
			}

		if (!hasGloveData)
			{
			return;
			}

		unsigned char* data = nullptr;
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
			bool ok = false;
			if ((type1 == kElementsSelected) &&
				(cols1 == 1) &&
				((rows1 == 1) || (rows == rows1) ))
				{
				realrowcount = itsFloatData->GetDataRowCount(startCol);
				if (startRow <= realrowcount + 1)
					{
					ok = true;
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
							auto* undo =
								jnew GLUndoElementChange(this, cell, oldvalue);
							assert(undo != nullptr);
							itsFloatData->SetElement(cell, value);
							NewUndo(undo);
							}
						else
							{
							auto* undo =
								jnew GLUndoElementAppend(this, cell);
							assert(undo != nullptr);
							itsFloatData->InsertElement(cell, value);
							NewUndo(undo);
							}
						}
					else
						{
						auto* undo =
							jnew GLUndoElementsInsert(this, JPoint(startCol, startRow),
													 JPoint(startCol, startRow + rows - 1),
													 GLUndoElementsBase::kElements);
						assert(undo != nullptr);
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
					auto* undo =
						jnew GLUndoElementsChange(this, JPoint(startCol, startRow),
												 JPoint(startCol + cols1 - 1, startRow + rows1 - 1),
												 GLUndoElementsBase::kElements);
					assert(undo != nullptr);
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
				auto* undo =
					jnew GLUndoElementsInsert(this, JPoint(count, 1),
											 JPoint(count + cols - 1, 1),
											 GLUndoElementsBase::kCols);
				assert(undo != nullptr);
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
				auto* undo =
					jnew GLUndoElementsInsert(this, JPoint(startCol, 1),
											 JPoint(startCol + cols - 1, 1),
											 GLUndoElementsBase::kCols);
				assert(undo != nullptr);
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
GLRaggedFloatTable::HandleSpecialPasteCmd()
{
	bool hasGloveData = false;
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
				hasGloveData = true;
				break;
				}
			}

		if (!hasGloveData)
			{
			return;
			}

		unsigned char* data = nullptr;
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
				JGetUserNotification()->ReportError(JGetString("NoSelection::GLRaggedFloatTable"));
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
					JGetUserNotification()->ReportError(JGetString("SelectionMismatch::GLRaggedFloatTable"));
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
					JGetUserNotification()->ReportError(JGetString("SelectionMismatch::GLRaggedFloatTable"));
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
GLRaggedFloatTable::HandleInsertion
	(
	const bool undo
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
			auto* undo1 =
				jnew GLUndoElementsInsert(this, JPoint(1, startRow),
										 JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
										 GLUndoElementsBase::kRows);
			assert(undo1 != nullptr);
			NewUndo(undo1);
			}
		itsFloatData->InsertRows(startRow, rows);
		}

	else if (type == kColsSelected)
		{
		if (undo)
			{
			auto* undo1 =
				jnew GLUndoElementsInsert(this, JPoint(startCol, 1),
										 JPoint(startCol + cols - 1, GetRowCount()),
										 GLUndoElementsBase::kCols);
			assert(undo1 != nullptr);
			NewUndo(undo1);
			}
		itsFloatData->InsertCols(startCol, cols);
		}

	else if (type == kElementsSelected)
		{
		if (cols == 1 && rows == 1 && undo)
			{
			JPoint cell(startCol, startRow);
			auto* undo1 =
				jnew GLUndoElementAppend(this, cell);
			assert(undo1 != nullptr);
			NewUndo(undo1);
			}
		else if (undo)
			{
			auto* undo1 =
				jnew GLUndoElementsInsert(this, JPoint(startCol, startRow),
										 JPoint(startCol + cols - 1, startRow + rows - 1),
										 GLUndoElementsBase::kElements);
			assert(undo1 != nullptr);
			NewUndo(undo1);
			}

		for (JIndex col = startCol; col < startCol + cols; col++)
			{
			JIndex end = JMin(startRow + rows - 1, itsFloatData->GetDataRowCount(col));
			for (JIndex row = startRow; row <= end; row++)
				{
				itsFloatData->InsertElement(row,col, 0);
				}
			}
		}
	TableRefresh();
}

/******************************************************************************
 HandleDuplication

 ******************************************************************************/

void
GLRaggedFloatTable::HandleDuplication()
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
//				jnew GLUndoElementAppend(this, cell);
//			assert(undo != nullptr);
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
GLRaggedFloatTable::HandleDeletion()
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
		auto* undo =
			jnew GLUndoElementsCut(this, JPoint(1, startRow),
								  JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
								  GLUndoElementsBase::kRows);
		assert(undo != nullptr);
		NewUndo(undo);
		for (JIndex i = startRow; i < startRow + rows; i++)
			{
			itsFloatData->RemoveRow(startRow);
			}
		}

	else if (type == kColsSelected)
		{
		auto* undo =
			jnew GLUndoElementsCut(this, JPoint(startCol, 1),
								  JPoint(startCol + cols - 1, GetRowCount()),
								  GLUndoElementsBase::kCols);
		assert(undo != nullptr);
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
				auto* undo =
					jnew GLUndoElementCut(this, cell, value);
				assert(undo != nullptr);
				NewUndo(undo);
				}
			}
		else
			{
			auto* undo =
				jnew GLUndoElementsCut(this, JPoint(startCol, startRow),
									  JPoint(startCol + cols - 1, startRow + rows - 1),
									  GLUndoElementsBase::kElements);
			assert(undo != nullptr);
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

GLRaggedFloatTable::SelectionType
GLRaggedFloatTable::GetSelectionType()
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

	bool found = iter.Next(&cell);
	if (found)
		{
		if (cell.y == 1)
			{
			const JSize rowCount = GetRowCount();
			bool colSelected = true;
			JIndex row = 2;
			while (row <= rowCount && colSelected)
				{
				if (!((GetTableSelection()).IsSelected(row,cell.x)))
					{
					colSelected = false;
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
			bool rowSelected = true;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
				{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
					{
					rowSelected = false;
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
			bool rowSelected = true;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
				{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
					{
					rowSelected = false;
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
GLRaggedFloatTable::GetSelectionArea
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
	const bool found = iter.Next(&cell);

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
GLRaggedFloatTable::UpdateDataMenu()
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
GLRaggedFloatTable::HandleDataMenu
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
		if (JGetChooseSaveFile()->ChooseFile(JGetString("SelectDataModulePrompt::GLRaggedFloatTable"), JString::empty, &modName))
			{
			GLDataModule* dm;
			GLDataModule::Create(&dm, this, itsFloatData, modName);
			}
		}
}

/******************************************************************************
 HandleModuleMenu

 ******************************************************************************/

void
GLRaggedFloatTable::HandleModuleMenu
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
		GLDataModule* dm;
		GLDataModule::Create(&dm, this, itsFloatData, modName);
		}
}

/******************************************************************************
 UpdateModuleMenu

 ******************************************************************************/

void
GLRaggedFloatTable::UpdateModuleMenu()
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
		itsModuleMenu->AppendItem(*(names->GetElement(i)));
		}
}

/******************************************************************************
 ChoosePlotColumns

 ******************************************************************************/

void
GLRaggedFloatTable::ChoosePlotColumns
	(
	const JIndex type
	)
{
	if (itsFloatData->GetDataColCount() == 0)
		{
		JGetUserNotification()->ReportError(JGetString("NoDataToPlot::GLRaggedFloatTable"));
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
		assert (itsCreatePlotDialog == nullptr);
		itsCreatePlotDialog =
			jnew GLCreatePlotDialog(itsTableDir, itsFloatData, xCol,x2Col,yCol,y2Col);
		assert (itsCreatePlotDialog != nullptr);
		ListenTo(itsCreatePlotDialog);
		itsCreatePlotDialog->BeginDialog();
		}
	else if (type == kPlotVectorCmd)
		{
		assert (itsCreateVectorPlotDialog == nullptr);
		itsCreateVectorPlotDialog =
			jnew GLCreateVectorPlotDialog(itsTableDir, itsFloatData, xCol, yCol, y2Col, x2Col);
		assert (itsCreateVectorPlotDialog != nullptr);
		ListenTo(itsCreateVectorPlotDialog);
		itsCreateVectorPlotDialog->BeginDialog();
		}
}

/******************************************************************************
 PlotData

 ******************************************************************************/

void
GLRaggedFloatTable::PlotData
	(
	const JIndex type
	)
{
	if (type == kDataPlot)
		{
		assert ( itsCreatePlotDialog != nullptr );
		}
	else if (type == kVectorPlot)
		{
		assert ( itsCreateVectorPlotDialog != nullptr );
		}

	JIndex xCol, x2Col, yCol, y2Col;
	if (type == kDataPlot)
		{
		itsCreatePlotDialog->GetColumns(&xCol, &x2Col, &yCol, &y2Col);
		}
	else
		{
		assert( type == kVectorPlot );
		itsCreateVectorPlotDialog->GetColumns(&xCol, &yCol, &x2Col, &y2Col);
		}

	const JArray<JFloat>& xData = itsFloatData->GetColPointer(xCol);
	const JArray<JFloat>& yData = itsFloatData->GetColPointer(yCol);

	const JArray<JFloat>* xErr = nullptr;
	if (x2Col != 0)
		{
		xErr = &(itsFloatData->GetColPointer(x2Col));
		}

	const JArray<JFloat>* yErr = nullptr;
	if (y2Col != 0)
		{
		yErr = &(itsFloatData->GetColPointer(y2Col));
		}

	JIndex index;
	bool oldPlot;
	JString label;
	if (type == kDataPlot)
		{
		label = itsCreatePlotDialog->GetLabel();
		oldPlot = itsCreatePlotDialog->GetPlotIndex(&index);
		}
	else
		{
		assert( type == kVectorPlot );
		label = itsCreateVectorPlotDialog->GetLabel();
		oldPlot = itsCreateVectorPlotDialog->GetPlotIndex(&index);
		}

	if (oldPlot)
		{
		itsTableDir->AddToPlot(index, type, xData, xErr, yData, yErr, true, label);
		}
	else
		{
		itsTableDir->CreateNewPlot(type, xData, xErr, yData, yErr, true, label);
		}
}

/******************************************************************************
 GetNewColByRange

 ******************************************************************************/

void
GLRaggedFloatTable::GetNewColByRange()
{
	assert (itsColByRangeDialog == nullptr);
	itsColByRangeDialog =
		jnew GLColByRangeDialog(GetWindow()->GetDirector(), itsFloatData->GetDataColCount() + 1);
	assert (itsColByRangeDialog != nullptr);
	ListenTo(itsColByRangeDialog);
	itsColByRangeDialog->BeginDialog();

}

/******************************************************************************
 CreateNewColByRange

 ******************************************************************************/

void
GLRaggedFloatTable::CreateNewColByRange()
{
	assert( itsColByRangeDialog != nullptr );

	JIndex dest;
	itsColByRangeDialog->GetDestination(&dest);
	JFloat beg;
	JFloat end;
	JInteger count;
	itsColByRangeDialog->GetValues(&beg, &end, &count);

	bool replace = false;
	JSize colCount = itsFloatData->GetDataColCount();
	if (dest <= colCount)
		{
		replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::GLRaggedFloatTable"));
		}

	if (!replace)
		{
		auto* undo =
			jnew GLUndoElementsInsert(this, JPoint(dest, 1),
									 JPoint(dest, 1),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
		NewUndo(undo);
		itsFloatData->InsertCols(dest, 1);
		}
	else if (dest <= colCount)
		{
		auto* undo =
			jnew GLUndoElementsChange(this, JPoint(dest, 1),
									 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
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
GLRaggedFloatTable::GetNewColByInc()
{
	assert (itsColByIncDialog == nullptr);
	itsColByIncDialog =
		jnew GLColByIncDialog(GetWindow()->GetDirector(), itsFloatData->GetDataColCount() + 1);
	assert (itsColByIncDialog != nullptr);
	ListenTo(itsColByIncDialog);
	itsColByIncDialog->BeginDialog();
}

/******************************************************************************
 CreateNewColByInc

 ******************************************************************************/

void
GLRaggedFloatTable::CreateNewColByInc()
{
	assert( itsColByIncDialog != nullptr );

	JIndex dest;
	itsColByIncDialog->GetDestination(&dest);
	JFloat beg;
	JFloat inc;
	JInteger count;
	itsColByIncDialog->GetValues(&beg, &inc, &count);

	bool replace = false;
	JSize colCount = itsFloatData->GetDataColCount();
	if (dest <= colCount)
		{
		replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::GLRaggedFloatTable"));
		}

	if (!replace)
		{
		auto* undo =
			jnew GLUndoElementsInsert(this, JPoint(dest, 1),
									 JPoint(dest, 1),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
		NewUndo(undo);
		itsFloatData->InsertCols(dest, 1);
		}
	else if (dest <= colCount)
		{
		auto* undo =
			jnew GLUndoElementsChange(this, JPoint(dest, 1),
									 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
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
GLRaggedFloatTable::WriteData
	(
	std::ostream& os
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
GLRaggedFloatTable::ReadData
	(
	std::istream& 		is,
	const JFloat	gloveVersion
	)
{
	JFileVersion vers = 0;
	if (gloveVersion > 0.5)
		{
		is >> vers;
		assert(vers <= kCurrentTableVersion);
		}

	itsFloatData->ShouldBroadcast(false);

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

	itsFloatData->ShouldBroadcast(true);
}

/******************************************************************************
 ChooseNewTransformFunction

 ******************************************************************************/

void
GLRaggedFloatTable::ChooseNewTransformFunction()
{
	assert (itsTransDialog == nullptr);
	assert (itsTransformVarList == nullptr);

	const JSize count = itsFloatData->GetDataColCount() + 1;
	if (count == 1)
		{
		JGetUserNotification()->ReportError(JGetString("NoDataToTransform::GLRaggedFloatTable"));
		return;
		}

	itsTransformVarList = jnew GLVarList();
	JArray<JFloat>* ar = jnew JArray<JFloat>;
	for (JSize i = 1; i < count; i++)
		{
		ar->AppendElement(0);
		}
	itsTransformVarList->AddArray(JString("col", JString::kNoCopy), *ar);

	itsTransDialog =
		jnew GLTransformFunctionDialog(GetWindow()->GetDirector(), itsTransformVarList, count);
	assert (itsTransDialog != nullptr);
	ListenTo(itsTransDialog);
	itsTransDialog->BeginDialog();
}

/******************************************************************************
 EvaluateTransformFunction

 ******************************************************************************/

void
jCollectColumnIndexes
	(
	const JFunction*	root,
	JArray<JIndex>*		inds
	)
{
	const auto* fwv = dynamic_cast<const JFunctionWithVar*>(root);
	if (fwv != nullptr)
		{
		const JFunction* ai = fwv->GetArrayIndex();
		if (ai != nullptr)
			{
			JFloat x;
			const bool ok = ai->Evaluate(&x);
			assert( ok );
			const JIndex i = JRound(x);
			JIndex tmp;
			if (!inds->SearchSorted(i, JListT::kAnyMatch, &tmp))
				{
				inds->InsertSorted(i, false);
				}
			}
		return;
		}

	const auto* fwa = dynamic_cast<const JFunctionWithArgs*>(root);
	if (fwa != nullptr)
		{
		const JSize argCount = fwa->GetArgCount();
		for (JIndex i=1; i<=argCount; i++)
			{
			jCollectColumnIndexes(fwa->GetArg(i), inds);
			}
		}
}

void
GLRaggedFloatTable::EvaluateTransformFunction()
{
	assert (itsTransDialog != nullptr);
	JIndex dest =  itsTransDialog->GetDestination();
	JString fnStr(itsTransDialog->GetFunctionString());

	const JSize count = itsFloatData->GetDataColCount();
	bool replace = false;
	if (dest <= count)
		{
		replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::GLRaggedFloatTable"));
		}

	JArray<JFloat> newArray;

	JExprParser p(itsTransformVarList);

	JFunction* f;
	if (!p.Parse(fnStr, &f))
		{
		return;
		}

	JArray<JIndex> inds;
	inds.SetCompareFunction(JCompareIndices);
	jCollectColumnIndexes(f, &inds);

	const JSize indCount = inds.GetElementCount();
	if (indCount == 0)
		{
		JGetUserNotification()->ReportError(JGetString("GenerateIfNoTransform::GLRaggedFloatTable"));
		jdelete f;
		return;
		}

	JSize minRowCount = itsFloatData->GetDataRowCount(inds.GetElement(1));
	for (JIndex i = 2; i <= indCount; i++)
		{
		const JSize rowCount = itsFloatData->GetDataRowCount(inds.GetElement(i));
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
			itsFloatData->GetElement(r, inds.GetElement(i), &value);
			itsTransformVarList->SetNumericValue(1, inds.GetElement(i), value);
			}
		JFloat value;
		f->Evaluate(&value);
		newArray.AppendElement(value);
		}

	if (replace)
		{
		auto* undo =
			jnew GLUndoElementsChange(this, JPoint(dest, 1),
									 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
		NewUndo(undo);
		itsFloatData->RemoveAllElements(dest);
		itsFloatData->SetCol(dest, newArray);
		}
	else
		{
		auto* undo =
			jnew GLUndoElementsInsert(this, JPoint(dest, 1),
									 JPoint(dest, 1),
									 GLUndoElementsBase::kCols);
		assert(undo != nullptr);
		NewUndo(undo);
		itsFloatData->InsertCols(dest, 1);
		itsFloatData->SetCol(dest, newArray);
		}

	jdelete f;
}

/******************************************************************************
 WriteDataCols

 ******************************************************************************/

bool
GLRaggedFloatTable::WriteDataCols
	(
	std::ostream& os,
	const int cols
	)
{
	SelectionType type = GetSelectionType();
	if (type != kColsSelected)
		{
		JString str((JUInt64) cols);
		const JUtf8Byte* map[] =
			{
			"i", str.GetBytes()
			};
		JGetUserNotification()->ReportError(
			JGetString("MustSelectColumns::GLRaggedFloatTable", map, sizeof(map)));
		return false;
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

	if (cols > (int) ncols)
		{
		JString str((JUInt64) cols);
		const JUtf8Byte* map[] =
			{
			"i", str.GetBytes()
			};
		JGetUserNotification()->ReportError(
			JGetString("MustSelectColumns::GLRaggedFloatTable", map, sizeof(map)));
		return false;
		}
	os << nrows << std::endl;
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
		os << std::endl;
		}
	return true;
}

/******************************************************************************
 ExportDataMatrix

 ******************************************************************************/

void
GLRaggedFloatTable::ExportDataMatrix
	(
	std::ostream& os
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
GLRaggedFloatTable::ExportData
	(
	std::ostream& os
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
GLRaggedFloatTable::PrintRealTable
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

	const JColorID gray50Color = JColorManager::GetGrayColor(50);
	SetRowBorderInfo(0, gray50Color);
	SetColBorderInfo(0, gray50Color);

	Print(p, false, false);

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
GLRaggedFloatTable::Undo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an undo object available.
	JUndo* undo;
	const bool hasUndo = GetCurrentUndo(&undo);

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
GLRaggedFloatTable::Redo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an redo object available.
	JUndo* undo;
	const bool hasUndo = GetCurrentRedo(&undo);

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

bool
GLRaggedFloatTable::GetCurrentUndo
	(
	JUndo** undo
	)
	const
{
	if (HasUndo())
		{
		*undo = itsUndoList->GetElement(itsFirstRedoIndex - 1);
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

bool
GLRaggedFloatTable::GetCurrentRedo
	(
	JUndo** redo
	)
	const
{
	if (HasRedo())
		{
		*redo = itsUndoList->GetElement(itsFirstRedoIndex);
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 HasUndo (public)

 ******************************************************************************/

bool
GLRaggedFloatTable::HasUndo()
	const
{
	return itsFirstRedoIndex > 1;
}

/******************************************************************************
 HasRedo (public)

 ******************************************************************************/

bool
GLRaggedFloatTable::HasRedo()
	const
{
	return itsFirstRedoIndex <= itsUndoList->GetElementCount();
}

/******************************************************************************
 NewUndo (private)

 ******************************************************************************/

void
GLRaggedFloatTable::NewUndo
	(
	JUndo* undo
	)
{
	if (itsUndoList != nullptr && itsUndoState == kIdle)
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

	else if (itsUndoList != nullptr && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
//		JUndo* oldUndo = itsUndoList->GetElement(itsFirstRedoIndex);
//		jdelete oldUndo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(true);
		undo->Deactivate();
		}

	else if (itsUndoList != nullptr && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

//		JUndo* oldRedo = itsUndoList->GetElement(itsFirstRedoIndex);
//		jdelete oldRedo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(false);
		undo->Deactivate();
		}

}
