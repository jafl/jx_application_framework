/******************************************************************************
 CBSymbolTable.cpp

	Displays a filtered version of CBSymbolList.

	BASE CLASS = JXTable

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBSymbolTable.h"
#include "CBSymbolDirector.h"
#include "CBSymbolList.h"
#include "CBSymbolTypeList.h"
#include "CBProjectDocument.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>

#include <JPainter.h>
#include <JTableSelection.h>
#include <JFontManager.h>
#include <JRegex.h>
#include <JSubstitute.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JCoordinate kIconWidth    = 20;
const JCoordinate kTextPadding  = 5;
const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolTable::CBSymbolTable
	(
	CBSymbolDirector*	symbolDirector,
	CBSymbolList*		symbolList,
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
	JXTable(10, 10, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsSymbolDirector = symbolDirector;
	itsSymbolList     = symbolList;
	itsMaxStringWidth = 0;

	itsVisibleList = jnew JArray<JIndex>(CBSymbolList::kBlockSize);
	assert( itsVisibleList != nullptr );

	itsVisibleListLockedFlag = false;
	itsNameFilter            = nullptr;
	itsNameLiteral           = nullptr;

	const JIndex blackColor = JColorManager::GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);

	AppendCols(1);
	SetDefaultRowHeight(JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()) +
						2*kVMarginWidth);

	SetSelectionBehavior(true, true);

	ListenTo(itsSymbolList);
	ListenTo(CBGetSymbolTypeList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSymbolTable::~CBSymbolTable()
{
	jdelete itsVisibleList;
	jdelete itsNameFilter;
	jdelete itsNameLiteral;
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
CBSymbolTable::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 SelectSingleEntry

 ******************************************************************************/

void
CBSymbolTable::SelectSingleEntry
	(
	const JIndex	index,
	const bool	scroll
	)
{
	SelectSingleCell(JPoint(1, index), scroll);
	itsKeyBuffer.Clear();
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
CBSymbolTable::ClearSelection()
{
	(GetTableSelection()).ClearSelection();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 DisplaySelectedSymbols

 ******************************************************************************/

void
CBSymbolTable::DisplaySelectedSymbols()
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JString missingFiles;

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JIndex symbolIndex = CellToSymbolIndex(cell);
			JIndex lineIndex;
			const JString& fileName =
				itsSymbolList->GetFile(symbolIndex, &lineIndex);

			CBTextDocument* doc;
			if (CBGetDocumentManager()->OpenTextDocument(fileName, lineIndex, &doc))
				{
				CBLanguage lang;
				CBSymbolList::Type type;
				itsSymbolList->GetSymbol(symbolIndex, &lang, &type);

				if (CBSymbolList::ShouldSmartScroll(type))
					{
					(doc->GetTextEditor())->ScrollForDefinition(lang);
					}
				}
			else
				{
				missingFiles += fileName;
				missingFiles += "\n";
				}
			}

		if (!missingFiles.IsEmpty())
			{
			const JUtf8Byte* map[] =
				{
				"list", missingFiles.GetBytes()
				};
			const JString msg = JGetString("MissingFiles::CBSymbolTable", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		}
}

/******************************************************************************
 FindSelectedSymbols

 ******************************************************************************/

void
CBSymbolTable::FindSelectedSymbols
	(
	const JXMouseButton button
	)
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			CBLanguage lang;
			CBSymbolList::Type type;
			const JString& name = itsSymbolList->GetSymbol(CellToSymbolIndex(cell), &lang, &type);
			itsSymbolDirector->FindSymbol(name, JString::empty, button);
			}
		}
}

/******************************************************************************
 CopySelectedSymbolNames

 ******************************************************************************/

void
CBSymbolTable::CopySelectedSymbolNames()
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);

		CBLanguage lang;
		CBSymbolList::Type type;

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JString& name = itsSymbolList->GetSymbol(CellToSymbolIndex(cell), &lang, &type);
			list.Append(const_cast<JString*>(&name));
			}

		auto* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 GetFileNamesForSelection

 ******************************************************************************/

void
CBSymbolTable::GetFileNamesForSelection
	(
	JPtrArray<JString>*	fileNameList,
	JArray<JIndex>*		lineIndexList
	)
	const
{
	itsKeyBuffer.Clear();

	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JIndex symbolIndex = CellToSymbolIndex(cell);
			JIndex lineIndex;
			const JString& fileName =
				itsSymbolList->GetFile(symbolIndex, &lineIndex);

			fileNameList->Append(fileName);
			lineIndexList->AppendElement(lineIndex);
			}
		}
}

/******************************************************************************
 ShowAll

 ******************************************************************************/

void
CBSymbolTable::ShowAll()
{
	itsVisibleListLockedFlag = false;
	itsVisibleList->RemoveAll();	// force rebuild of entire list

	jdelete itsNameFilter;
	itsNameFilter = nullptr;

	jdelete itsNameLiteral;
	itsNameLiteral = nullptr;

	ScrollTo(0,0);
	RebuildTable();
}

/******************************************************************************
 SetNameFilter

 ******************************************************************************/

JError
CBSymbolTable::SetNameFilter
	(
	const JString&	filterStr,
	const bool	isRegex
	)
{
	jdelete itsNameFilter;
	itsNameFilter = nullptr;

	jdelete itsNameLiteral;
	itsNameLiteral = nullptr;

	JError result = JNoError();
	if (isRegex)
		{
		itsNameFilter = jnew JRegex;
		assert( itsNameFilter != nullptr );
		result = itsNameFilter->SetPattern(filterStr);
		if (!result.OK())
			{
			jdelete itsNameFilter;
			itsNameFilter = nullptr;
			}
		}
	else
		{
		itsNameLiteral = jnew JString(filterStr);
		assert( itsNameLiteral != nullptr );
		}

	itsVisibleListLockedFlag = false;

	ScrollTo(0,0);
	RebuildTable();
	return result;
}

/******************************************************************************
 SetDisplayList

 ******************************************************************************/

void
CBSymbolTable::SetDisplayList
	(
	const JArray<JIndex>& list
	)
{
	*itsVisibleList = list;
	itsVisibleList->SetBlockSize(CBSymbolList::kBlockSize);
	itsVisibleListLockedFlag = true;
	ScrollTo(0,0);
	RebuildTable();
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	const JIndex symbolIndex = CellToSymbolIndex(cell);

	CBLanguage lang;
	CBSymbolList::Type type;
	const JString& symbolName =
		itsSymbolList->GetSymbol(symbolIndex, &lang, &type);

	const JString* signature;
	itsSymbolList->GetSignature(symbolIndex, &signature);

	if (CalcColWidths(symbolName, signature))
		{
		AdjustColWidths();
		}

	const JXImage* icon     = nullptr;
	const JFontStyle& style = (CBGetSymbolTypeList())->GetStyle(type, &icon);

	// draw icon

	if (icon != nullptr)
		{
		JRect r = rect;
		r.right = r.left + kIconWidth;
		p.Image(*icon, icon->GetBounds(), r);
		}

	// draw name

	p.SetFontStyle(style);

	JRect r = rect;
	r.left += kIconWidth + kHMarginWidth;
	p.String(r, symbolName, JPainter::kHAlignLeft, JPainter::kVAlignCenter);

	if (signature != nullptr)
		{
		r.left += JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), symbolName);
		p.String(r, *signature, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsKeyBuffer.Clear();

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
		{
		return;
		}
	else if (!GetCell(pt, &cell))
		{
		(GetTableSelection()).ClearSelection();
		}
	else if (button == kJXLeftButton &&
			 clickCount == 2 &&
			 !modifiers.shift() && modifiers.meta() && modifiers.control())
		{
		SelectSingleCell(cell, false);
		DisplaySelectedSymbols();
		GetWindow()->Close();
		return;
		}
	else if (clickCount == 2 && modifiers.meta())
		{
		SelectSingleCell(cell, false);
		FindSelectedSymbols(button);
		}
	else if (button == kJXLeftButton &&
			 clickCount == 2 &&
			 !modifiers.shift() && !modifiers.control())
		{
		DisplaySelectedSymbols();
		}
	else
		{
		BeginSelectionDrag(cell, button, modifiers);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ContinueSelectionDrag(pt, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishSelectionDrag();
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::HandleFocusEvent()
{
	JXTable::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint topSelCell;
	JTableSelection& s          = GetTableSelection();
	const bool hadSelection = s.GetFirstSelectedCell(&topSelCell);

	if (c == ' ')
		{
		ClearSelection();
		}

	else if (c == kJReturnKey)
		{
		DisplaySelectedSymbols();
		if (modifiers.meta() && modifiers.control())
			{
			GetWindow()->Close();
			return;
			}
		}

	else if (c == kJUpArrow || c == kJDownArrow)
		{
		itsKeyBuffer.Clear();
		if (!hadSelection && c == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleEntry(GetRowCount());
			}
		else
			{
			HandleSelectionKeyPress(c, modifiers);
			}
		}

	else if ((c == 'c' || c == 'C') && modifiers.meta() && !modifiers.shift())
		{
		CopySelectedSymbolNames();
		}

	else if (c.IsPrint() && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.Append(c);

		JIndex index;
		if (itsSymbolList->ClosestMatch(itsKeyBuffer, *itsVisibleList, &index))
			{
			JString saveBuffer = itsKeyBuffer;
			SelectSingleEntry(index);
			itsKeyBuffer = saveBuffer;
			}
		}

	else
		{
		JXTable::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSymbolList && message.Is(CBSymbolList::kChanged))
		{
		RebuildTable();
		}

	else if (sender == CBGetSymbolTypeList() &&
			 message.Is(CBSymbolTypeList::kVisibilityChanged))
		{
		RebuildTable();
		}
	else if (sender == CBGetSymbolTypeList() &&
			 message.Is(CBSymbolTypeList::kStyleChanged))
		{
		Refresh();
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 RebuildTable (private)

 ******************************************************************************/

void
CBSymbolTable::RebuildTable()
{
	JXGetApplication()->DisplayBusyCursor();

	const JPoint scrollPt = (GetAperture()).topLeft();

	ClearSelection();
	itsKeyBuffer.Clear();
	itsMaxStringWidth = 0;

	if (itsVisibleListLockedFlag)
		{
		const JSize count = itsVisibleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JIndex j = itsVisibleList->GetElement(i);

			const JString* signature;
			itsSymbolList->GetSignature(j, &signature);

			CBLanguage lang;
			CBSymbolList::Type type;
			CalcColWidths(itsSymbolList->GetSymbol(j, &lang, &type), signature);
			}
		}
	else if (itsNameFilter != nullptr || itsNameLiteral != nullptr)
		{
		itsVisibleList->RemoveAll();

		const JSize symbolCount = itsSymbolList->GetElementCount();
		for (JIndex i=1; i<=symbolCount; i++)
			{
			CBLanguage lang;
			CBSymbolList::Type type;
			const JString& symbolName = itsSymbolList->GetSymbol(i, &lang, &type);

			const JString* signature;
			itsSymbolList->GetSignature(i, &signature);

			if (/* (CBGetSymbolTypeList())->IsVisible(type) && */

				((itsNameFilter  != nullptr && itsNameFilter->Match(symbolName)) ||
				 (itsNameLiteral != nullptr && symbolName.Contains(*itsNameLiteral)))
				)
				{
				itsVisibleList->AppendElement(i);
				CalcColWidths(symbolName, signature);
				}
			}
		}
	else	// optimize because scanning 1e5 symbols takes a while!
		{
		const JSize symbolCount = itsSymbolList->GetElementCount();
		if (itsVisibleList->GetElementCount() > symbolCount)
			{
			itsVisibleList->RemoveNextElements(
				symbolCount+1, itsVisibleList->GetElementCount() - symbolCount);
			}
		while (itsVisibleList->GetElementCount() < symbolCount)
			{
			itsVisibleList->AppendElement(itsVisibleList->GetElementCount()+1);
			}
		}

	const JSize rowCount = itsVisibleList->GetElementCount();
	if (GetRowCount() < rowCount)
		{
		AppendRows(rowCount - GetRowCount());
		}
	else if (GetRowCount() > rowCount)
		{
		RemoveNextRows(rowCount+1, GetRowCount() - rowCount);
		}

	AdjustColWidths();
	ScrollTo(scrollPt);
	Refresh();
}

/******************************************************************************
 CalcColWidths (private)

 ******************************************************************************/

bool
CBSymbolTable::CalcColWidths
	(
	const JString& symbolName,
	const JString* signature
	)
{
	JSize w = JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), symbolName);

	if (signature != nullptr)
		{
		w += JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), *signature);
		}

	if (w > itsMaxStringWidth)
		{
		itsMaxStringWidth = w;
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CBSymbolTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
CBSymbolTable::AdjustColWidths()
{
	const JCoordinate minWidth = kIconWidth + itsMaxStringWidth + kTextPadding;
	SetColWidth(1, JMax(minWidth, GetApertureWidth()));
}

/******************************************************************************
 CellToSymbolIndex (private)

 ******************************************************************************/

inline JIndex
CBSymbolTable::CellToSymbolIndex
	(
	const JPoint& cell
	)
	const
{
	return itsVisibleList->GetElement(cell.y);
}
