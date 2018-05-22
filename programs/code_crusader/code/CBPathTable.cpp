/******************************************************************************
 CBPathTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBPathTable.h"
#include "CBDirList.h"
#include "CBRelPathCSF.h"
#include <JXTextButton.h>
#include <JXPathInput.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXColorManager.h>
#include <JXImage.h>
#include <jXUtil.h>
#include <jXConstants.h>
#include <JStringTableData.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const JCoordinate kRecurseColumn = 1;
const JCoordinate kIconColumn    = 2;
const JCoordinate kTextColumn    = 3;

static const JCharacter* kFlagOnStr  = "R";
static const JCharacter* kFlagOffStr = "";		// must be empty

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPathTable::CBPathTable
	(
	const CBDirList&	pathList,
	JXTextButton*		addPathButton,
	JXTextButton*		removePathButton,
	JXTextButton*		choosePathButton,
	CBRelPathCSF*		csf,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsBasePath(pathList.GetBasePath())
{
	const JSize rowHeight = 2*kVMarginWidth +
		GetFontManager()->GetDefaultMonospaceFont().GetLineHeight();
	SetDefaultRowHeight(rowHeight);

	itsFolderIcon = jnew JXImage(GetDisplay(), jx_folder_small);
	assert( itsFolderIcon != NULL );

	itsPathInput        = NULL;
	itsAddPathButton    = addPathButton;
	itsRemovePathButton = removePathButton;
	itsChoosePathButton = choosePathButton;
	itsCSF              = csf;

	ListenTo(itsAddPathButton);
	ListenTo(itsRemovePathButton);
	ListenTo(itsChoosePathButton);

	itsData = jnew JStringTableData;
	assert( itsData != NULL );

	itsData->AppendCols(3);		// recurse flag, folder icon, path

	const JSize rowCount = pathList.GetElementCount();
	itsData->AppendRows(rowCount);

	for (JIndex i=1; i<=rowCount; i++)
		{
		JBoolean recurse;
		const JString& path = pathList.GetPath(i, &recurse);
		if (recurse)
			{
			itsData->SetString(i,kRecurseColumn, kFlagOnStr);
			}
		itsData->SetString(i,kTextColumn, path);
		}

	SetTableData(itsData);

	const JSize flagColWidth = 2*kHMarginWidth +
		GetFontManager()->GetDefaultFont().GetStringWidth(kFlagOnStr);

	SetColBorderInfo(0, GetColormap()->GetBlackColor());
	SetColWidth(kRecurseColumn, flagColWidth);
	SetColWidth(kIconColumn,    itsFolderIcon->GetWidth() + 2*kHMarginWidth);
	FitToEnclosure();

	UpdateButtons();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPathTable::~CBPathTable()
{
	jdelete itsData;
	jdelete itsFolderIcon;
}

/******************************************************************************
 AddDirectories

 ******************************************************************************/

void
CBPathTable::AddDirectories
	(
	const JPtrArray<JString>& list
	)
{
	JIndex firstNewRow = 0;

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* name = list.GetElement(i);
		if (JDirectoryExists(*name))
			{
			itsData->AppendRows(1);
			const JSize rowCount = GetRowCount();
			itsData->SetString(rowCount, kRecurseColumn, kFlagOnStr);
			itsData->SetString(rowCount, kTextColumn,    *name);

			if (firstNewRow == 0)
				{
				firstNewRow = rowCount;
				}
			}
		}

	if (firstNewRow != 0)
		{
		ScrollTo((GetBounds()).bottomLeft());
		BeginEditing(JPoint(kTextColumn, firstNewRow));
		}
}

/******************************************************************************
 GetPathList

 ******************************************************************************/

void
CBPathTable::GetPathList
	(
	CBDirList* pathList
	)
	const
{
	assert( !IsEditing() );
	pathList->RemoveAll();

	const JSize rowCount = GetRowCount();
	for (JIndex i=1; i<=rowCount; i++)
		{
		const JString& dirName = itsData->GetString(i,kTextColumn);
		if (!dirName.IsEmpty())
			{
			pathList->AddPath(dirName, !(itsData->GetString(i,kRecurseColumn)).IsEmpty());
			}
		}

	pathList->SetBasePath(itsBasePath);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CBPathTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
		{
		return;
		}
	else if (cell.x == kRecurseColumn)
		{
		const JString& str = itsData->GetString(cell);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else if (cell.x == kIconColumn)
		{
		p.Image(*itsFolderIcon, itsFolderIcon->GetBounds(), rect);
		}
	else if (cell.x == kTextColumn)
		{
		const JString& str = itsData->GetString(cell);

		JSize size;
		const JCharacter* name = JXPathInput::GetFont(&size);
		JFont font = GetFontManager()->GetFont(name, size);
		font.SetColor(JXPathInput::GetTextColor(str, itsBasePath, kJFalse, p.GetColormap()));
		p.SetFont(font);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBPathTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button != kJXLeftButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (!GetCell(pt, &cell))
		{
		EndEditing();
		}
	else if (cell.x == kRecurseColumn)
		{
		const JString& flag = itsData->GetString(cell);
		if (flag.IsEmpty())
			{
			itsData->SetString(cell, kFlagOnStr);
			}
		else
			{
			itsData->SetString(cell, kFlagOffStr);
			}
		}
	else
		{
		cell.x = kTextColumn;
		BeginEditing(cell);
		}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

JBoolean
CBPathTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return JI2B(cell.x == kTextColumn);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CBPathTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsPathInput == NULL && cell.x == kTextColumn );

	itsPathInput = jnew JXPathInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsPathInput != NULL );

	itsPathInput->SetText(itsData->GetString(cell));
	itsPathInput->SetBasePath(itsBasePath);
	itsPathInput->ShouldAllowInvalidPath();
	UpdateButtons();
	return itsPathInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
CBPathTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsPathInput != NULL && cell.x == kTextColumn );

	if (itsPathInput->InputValid())
		{
		itsData->SetString(cell, itsPathInput->GetText());
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
CBPathTable::PrepareDeleteXInputField()
{
	if (itsPathInput != NULL)
		{
		itsPathInput = NULL;
		UpdateButtons();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBPathTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddPathButton && message.Is(JXButton::kPushed))
		{
		if (EndEditing())
			{
			itsData->AppendRows(1);
			itsData->SetString(GetRowCount(), kRecurseColumn, kFlagOnStr);
			BeginEditing(JPoint(kTextColumn, itsData->GetRowCount()));
			}
		}
	else if (sender == itsRemovePathButton && message.Is(JXButton::kPushed))
		{
		JPoint editCell;
		if (GetEditedCell(&editCell))
			{
			CancelEditing();
			itsData->RemoveRow(editCell.y);
			}
		}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		ChoosePath();
		}
	else
		{
		JXEditTable::Receive(sender, message);
		}
}

/******************************************************************************
 ChoosePath (private)

 ******************************************************************************/

void
CBPathTable::ChoosePath()
{
	JPoint cell;
	if (itsPathInput != NULL && GetEditedCell(&cell))
		{
		JString path      = itsPathInput->GetText();
		const JBoolean ok = itsCSF->ChooseRelRPath("", NULL, path, &path);	// kills itsPathInput
		if (BeginEditing(cell) && ok && itsPathInput != NULL)
			{
			itsPathInput->SetText(path);
			}
		}
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CBPathTable::UpdateButtons()
{
	if (itsPathInput != NULL)
		{
		itsRemovePathButton->Activate();
		itsChoosePathButton->Activate();
		}
	else
		{
		itsRemovePathButton->Deactivate();
		itsChoosePathButton->Deactivate();
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CBPathTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	SetColWidth(kTextColumn, GetApertureWidth()
							 - GetColWidth(kRecurseColumn)
							 - GetColWidth(kIconColumn));
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list.

 ******************************************************************************/

JBoolean
CBPathTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == urlXAtom)
			{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
CBPathTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						time, selMgr->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == selMgr->GetURLXAtom())
			{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
			AddDirectories(fileNameList);
			JXReportUnreachableHosts(urlList);
			}

		selMgr->DeleteData(&data, delMethod);
		}
}
