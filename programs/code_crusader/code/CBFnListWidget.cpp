/******************************************************************************
 CBFnListWidget.cpp

	Displays the list of the functions implemented by a class.

	BASE CLASS = JXStringList

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBFnListWidget.h"
#include "CBTree.h"
#include "CBClass.h"
#include "CBTreeWidget.h"
#include "CBFnMenuUpdater.h"
#include "cbmUtil.h"
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <JPagePrinter.h>
#include <JTableSelection.h>
#include <jMath.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFnListWidget::CBFnListWidget
	(
	const CBTreeWidget*	treeWidget,
	const CBClass*		theClass,
	const JBoolean		showInheritedFns,
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
	JXStringList(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	CBFnListWidgetX(treeWidget, showInheritedFns);
	InstallClass(theClass);
}

CBFnListWidget::CBFnListWidget
	(
	std::istream&			input,
	const JFileVersion	vers,
	JBoolean*			keep,
	const CBTreeWidget*	treeWidget,
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
	JXStringList(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	JBoolean showInheritedFns;
	input >> itsClassName >> showInheritedFns;

	CBFnListWidgetX(treeWidget, showInheritedFns);

	*keep = Reconnect();
}

// private

void
CBFnListWidget::CBFnListWidgetX
	(
	const CBTreeWidget*	treeWidget,
	const JBoolean		showInheritedFns
	)
{
	WantInput(kJTrue, kJFalse, kJTrue);	// need Meta-Tab

	itsTreeWidget = treeWidget;

	itsFnNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsFnNames != NULL );
	itsFnNames->SetCompareFunction(JCompareStringsCaseInsensitive);
	itsFnNames->SetSortOrder(JOrderedSetT::kSortAscending);

	itsShowInheritedFnsFlag = showInheritedFns;

	itsHasImagesFlag = kJFalse;

	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kForgetAll);
	assert( itsImageList != NULL );

	itsFnMenu     = NULL;
	itsMenuButton = kJXLeftButton;

	// must be last to avoid crash in Receive()

	CBTree* tree  = itsTreeWidget->GetTree();
	ListenTo(tree);
	SetFont(JGetDefaultFontName(), tree->GetFontSize());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFnListWidget::~CBFnListWidget()
{
	jdelete itsFnNames;
	jdelete itsImageList;	// contents owned by CBTreeWidget
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBFnListWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << ' ' << itsClassName << ' ' << itsShowInheritedFnsFlag << ' ';
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
CBFnListWidget::SkipSetup
	(
	std::istream& input
	)
{
	JString className;
	JBoolean showInheritedFns;
	input >> className >> showInheritedFns;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
CBFnListWidget::Print
	(
	JPagePrinter& p
	)
{
	if (GetRowCount() > 0)
		{
		JXStringList::Print(p);
		}
}

/******************************************************************************
 Print header and footer (virtual protected)

 ******************************************************************************/

JCoordinate
CBFnListWidget::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight()+1;
}

JCoordinate
CBFnListWidget::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
CBFnListWidget::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JRect pageRect = p.GetPageRect();

	JString titleStr;
	if (itsClass->IsEnum())
		{
		titleStr = "Values defined by " + itsClass->GetFullName();
		}
	else
		{
		titleStr = "Functions implemented by " + itsClass->GetFullName();
		}

	p.String(pageRect.left, pageRect.top, titleStr);
	p.Line(pageRect.left, pageRect.top + headerHeight-1,
		   pageRect.right, pageRect.top + headerHeight-1);
}

void
CBFnListWidget::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	const JString pageNumberStr = "Page " + JString(p.GetPageIndex());
	p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
			 pageRect.width(), JPainter::kHAlignCenter,
			 footerHeight, JPainter::kVAlignBottom);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBFnListWidget::HandleMouseDown
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
	else if (clickCount == 1)
		{
		SelectFunction(cell.y);
		}
	else if (clickCount == 2)
		{
		HandleDoubleClick(button, cell.y);
		}
}

/******************************************************************************
 HandleDoubleClick (private)

 ******************************************************************************/

void
CBFnListWidget::HandleDoubleClick
	(
	const JXMouseButton	button,
	const JIndex		index
	)
	const
{
	if (RowIndexValid(index))
		{
		// show code

		if (button == kJXLeftButton)
			{
			ViewDefinition(index);
			}

		// show definition

		else if (button == kJXMiddleButton)
			{
			ViewDeclaration(index);
			}

		// find all classes implementing this function

		else if (button == kJXRightButton)
			{
			itsTreeWidget->FindFunction(GetFunctionName(index), kJTrue);
			}
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBFnListWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	// find in source

	if (key == kJReturnKey)
		{
		ViewDefinition();
		itsKeyBuffer.Clear();
		}

	// find in header

	else if (key == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
		{
		ViewDeclaration();
		itsKeyBuffer.Clear();
		}

	// move selection with arrow keys

	else if (key == kJUpArrow)
		{
		MoveSelection(-1);
		}
	else if (key == kJDownArrow)
		{
		MoveSelection(+1);
		}

	// incremental search for function name

	else if (key == ' ')
		{
		itsKeyBuffer.Clear();
		(GetTableSelection()).ClearSelection();
		}
	else if (JXIsPrint(key) && !modifiers.meta() && !modifiers.control())
		{
		itsKeyBuffer.AppendCharacter(key);

		JIndex fnIndex;
		if (ClosestMatch(&fnIndex))
			{
			SelectFunction(fnIndex);
			}
		}

	else
		{
		JXStringList::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 ClosestMatch (private)

	Returns the closest match for the given function name prefix.

 ******************************************************************************/

JBoolean
CBFnListWidget::ClosestMatch
	(
	JIndex* fnIndex
	)
{
	const JSize fnCount = GetRowCount();
	if (fnCount == 0)
		{
		*fnIndex = 0;
		return kJFalse;
		}

	JBoolean found;
	*fnIndex = itsFnNames->SearchSorted1(&itsKeyBuffer, JOrderedSetT::kFirstMatch, &found);
	if (*fnIndex > fnCount)		// insert beyond end of list
		{
		*fnIndex = fnCount;
		}
	return kJTrue;
}

/******************************************************************************
 ViewDefinition (private)

 ******************************************************************************/

void
CBFnListWidget::ViewDefinition
	(
	const JIndex origIndex
	)
	const
{
	JIndex rowIndex = origIndex;
	if (rowIndex != 0 || GetSelectedFunction(&rowIndex))
		{
		itsClass->ViewDefinition(GetFunctionName(rowIndex), kJTrue);
		}
}

/******************************************************************************
 ViewDeclaration (private)

 ******************************************************************************/

void
CBFnListWidget::ViewDeclaration
	(
	const JIndex origIndex
	)
	const
{
	JIndex rowIndex = origIndex;
	if (rowIndex != 0 || GetSelectedFunction(&rowIndex))
		{
		itsClass->ViewDeclaration(GetFunctionName(rowIndex), kJTrue);
		}
}

/******************************************************************************
 GetSelectedFunction (private)

 ******************************************************************************/

JBoolean
CBFnListWidget::GetSelectedFunction
	(
	JIndex* rowIndex
	)
	const
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
		{
		*rowIndex = cell.y;
		return kJTrue;
		}
	else
		{
		*rowIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 SelectFunction (private)

 ******************************************************************************/

void
CBFnListWidget::SelectFunction
	(
	const JIndex rowIndex
	)
{
	if (RowIndexValid(rowIndex))
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectRow(rowIndex);

		TableScrollToCell(JPoint(1, rowIndex));
		}
}

/******************************************************************************
 MoveSelection (private)

 ******************************************************************************/

void
CBFnListWidget::MoveSelection
	(
	const JCoordinate delta
	)
{
	JIndex rowIndex;
	if (GetSelectedFunction(&rowIndex))
		{
		SelectFunction(rowIndex + delta);
		}
	else if (delta < 0)
		{
		SelectFunction(1);
		}
	else if (delta > 0)
		{
		SelectFunction(GetRowCount());
		}
}

/******************************************************************************
 SelectUnimplementedFns

 ******************************************************************************/

void
CBFnListWidget::SelectUnimplementedFns()
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();

	JBoolean found    = kJFalse;
	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetStyle(i).italic)
			{
			s.SelectRow(i);
			if (!found)
				{
				TableScrollToCell(JPoint(1, i));
				found = kJTrue;
				}
			}
		}
}

/******************************************************************************
 CopySelectedFunctionNames

	Copies the names of the selected functions to the text clipboard.

 ******************************************************************************/

void
CBFnListWidget::CopySelectedFunctionNames()
	const
{
	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			list.Append(itsFnNames->NthElement(cell.y));
			}

		JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != NULL );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 Reconnect

 ******************************************************************************/

JBoolean
CBFnListWidget::Reconnect()
{
	CBTree* tree            = itsTreeWidget->GetTree();
	const CBClass* newClass = NULL;
	if (tree->FindClass(itsClassName, &newClass))
		{
		InstallClass(newClass);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 InstallClass (private)

 ******************************************************************************/

void
CBFnListWidget::InstallClass
	(
	const CBClass* theClass
	)
{
	itsClass     = theClass;
	itsClassName = theClass->GetFullName();

	BuildFnList();
}

/******************************************************************************
 BuildFnList (private)

	actually implements function => bold (if showing inherited functions)

	public => black, protected => brown, private => red

	pure virtual => italic

 ******************************************************************************/

void
CBFnListWidget::BuildFnList()
{
	itsFnNames->DeleteAll();
	itsHasUnimplementedFnsFlag = kJFalse;

	JRunArray<JFontStyle> styles;

	itsHasImagesFlag = kJFalse;
	itsImageList->RemoveAll();

	BuildFnList1(itsClass, CBClass::kInheritPublic, itsFnNames, &styles);

	SetStringList(itsFnNames);
	SetStyles(styles);

	itsKeyBuffer.Clear();
}

void
CBFnListWidget::BuildFnList1
	(
	const CBClass*				theClass,
	const CBClass::InheritType	maxAccess,
	JPtrArray<JString>*			names,
	JRunArray<JFontStyle>*		styles
	)
{
	const JBoolean bold = JI2B( itsShowInheritedFnsFlag && theClass == itsClass );

	const JSize fnCount = theClass->GetFunctionCount();
	for (JIndex i=1; i<=fnCount; i++)
		{
		CBClass::FnAccessLevel fnAccess = theClass->GetFnAccessLevel(i);
		if (theClass != itsClass &&
			!theClass->IsInherited(i, maxAccess, &fnAccess))
			{
			continue;
			}

		JString* s = jnew JString(theClass->GetFunctionName(i));
		assert( s != NULL );
		JIndex insertionIndex;
		if (!names->InsertSorted(s, kJFalse, &insertionIndex))
			{
			// no point in including duplicate names without name mangling
			jdelete s;
			continue;
			}

		const JBoolean isImplemented = theClass->IsImplemented(i);
		if (!isImplemented)
			{
			itsHasUnimplementedFnsFlag = kJTrue;
			}

		if (CBClass::IsSignal(fnAccess))
			{
			itsHasImagesFlag = kJTrue;
			itsImageList->InsertElementAtIndex(insertionIndex, itsTreeWidget->GetQtSignalImage());
			}
		else if (CBClass::IsSlot(fnAccess))
			{
			itsHasImagesFlag = kJTrue;
			itsImageList->InsertElementAtIndex(insertionIndex, itsTreeWidget->GetQtSlotImage());
			}
		else
			{
			itsImageList->InsertElementAtIndex(insertionIndex, (JXImage*) NULL);
			}

		JFontStyle style(bold, !isImplemented, 0, kJFalse);
		if (CBClass::IsProtected(fnAccess))
			{
			style.color = GetColormap()->GetBrownColor();
			}
		else if (CBClass::IsPrivate(fnAccess))
			{
			style.color = GetColormap()->GetRedColor();
			}
		else if (fnAccess == CBClass::kJavaDefaultAccess)
			{
			style.color = GetColormap()->GetDarkGreenColor();
			}

		styles->InsertElementAtIndex(insertionIndex, style);
		}

	if (itsShowInheritedFnsFlag)
		{
		const JSize parentCount = theClass->GetParentCount();
		for (JIndex i=1; i<=parentCount; i++)
			{
			const CBClass::InheritType parentAccess = theClass->GetParentType(i);
			if (theClass == itsClass ||
				parentAccess != CBClass::kInheritPrivate)
				{
				const CBClass* parent;
				const JBoolean ok = theClass->GetParent(i, &parent);
				assert( ok );

				CBClass::InheritType newMaxAccess = parentAccess;
				if (maxAccess == CBClass::kInheritPrivate)
					{
					newMaxAccess = CBClass::kInheritPrivate;
					}
				else if (maxAccess == CBClass::kInheritProtected &&
						 parentAccess == CBClass::kInheritPublic)
					{
					newMaxAccess = CBClass::kInheritProtected;
					}

				BuildFnList1(parent, newMaxAccess, names, styles);
				}
			}
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

const JCoordinate kHSpacing = 6;

void
CBFnListWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JRect r = rect;
	if (itsHasImagesFlag)
		{
		const JXImage* image = itsTreeWidget->GetQtSignalImage();
		r.left += image->GetWidth() + kHSpacing;
		}

	JXStringList::TableDrawCell(p, cell, r);

	if (itsHasImagesFlag)
		{
		JXImage* image = itsImageList->NthElement(cell.y);
		if (image != NULL)
			{
			JPoint pt(rect.topLeft());
			pt.x += kHSpacing / 2;
			p.Image(*image, image->GetBounds(), pt);
			}
		}
}

/******************************************************************************
 PrepareFunctionMenu

	Copy our contents to the given menu and listen for a selection.

 ******************************************************************************/

void
CBFnListWidget::PrepareFunctionMenu
	(
	JXTextMenu* menu
	)
{
	menu->RemoveAllItems();
	menu->SetUpdateAction(JXMenu::kDisableNone);

	menu->SetDefaultFont(GetFont(), kJFalse);
	if (CBMGetFnMenuUpdater()->WillPackFnNames())
		{
		menu->SetDefaultFontSize(GetFont().GetSize()-2, kJFalse);
		}

	const JSize count = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		menu->AppendItem(GetFunctionName(i));
		menu->SetItemFontStyle(i, GetStyle(i));

		JXImage* image = itsImageList->NthElement(i);
		if (image != NULL)
			{
			menu->SetItemImage(i, image, kJFalse);
			}
		}

	if (itsFnMenu != NULL)
		{
		StopListening(itsFnMenu);
		}
	itsFnMenu = menu;
	ListenTo(itsFnMenu);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBFnListWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CBTree* tree = itsTreeWidget->GetTree();
	if (sender == tree && message.Is(CBTree::kFontSizeChanged))
		{
		SetFont(JGetDefaultFontName(), tree->GetFontSize());
		}

	else if (sender == itsFnMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleDoubleClick(itsMenuButton, selection->GetIndex());
		}

	else
		{
		JXStringList::Receive(sender, message);
		}
}
