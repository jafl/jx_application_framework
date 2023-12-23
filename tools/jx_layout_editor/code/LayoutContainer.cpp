/******************************************************************************
 LayoutContainer.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutContainer.h"
#include "LayoutDocument.h"
#include "LayoutSelection.h"
#include "LayoutUndo.h"
#include "BaseWidget.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

#include "LayoutContainer-Edit.h"

const JSize kMaxUndoCount = 100;	// maximum length of itsUndoList

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutContainer::LayoutContainer
	(
	LayoutDocument*		doc,
	JXMenuBar*			menuBar,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsDoc(doc),
	itsGridSpacing(10),
	itsUndoList(jnew JPtrArray<LayoutUndo>(JPtrArrayT::kDeleteAll, kMaxUndoCount+1)),
	itsFirstRedoIndex(1),
	itsLastSaveRedoIndex(itsFirstRedoIndex),
	itsUndoState(kIdle),
	itsResizeUndo(nullptr),
	itsIgnoreResizeFlag(false)
{
	SetBorderWidth(10);

	itsLayoutXAtom =
		GetDisplay()->RegisterXAtom(LayoutSelection::GetXAtomName());

	itsEditMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::LayoutContainer_Edit"));
	itsEditMenu->SetMenuItems(kEditMenuStr);
	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsEditMenu->AttachHandlers(this,
		&LayoutContainer::UpdateEditMenu,
		&LayoutContainer::HandleEditMenu);
	ConfigureEditMenu(itsEditMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutContainer::~LayoutContainer()
{
	jdelete itsUndoList;
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
LayoutContainer::HasSelection()
	const
{
	return AnyOf([](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		return widget != nullptr && widget->IsSelected();
	},
	true);
}

/******************************************************************************
 SelectAllWidgets

 ******************************************************************************/

void
LayoutContainer::SelectAllWidgets()
{
	ForEach([](JXContainer* obj)
	{
		auto* widget = dynamic_cast<BaseWidget*>(obj);
		if (widget != nullptr)
		{
			widget->SetSelected(true);
		}
	},
	true);
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
LayoutContainer::ClearSelection()
{
	ForEach([](JXContainer* obj)
	{
		auto* widget = dynamic_cast<BaseWidget*>(obj);
		if (widget != nullptr)
		{
			widget->SetSelected(false);
		}
	},
	true);
}

/******************************************************************************
 GetSelectedWidgets

 ******************************************************************************/

void
LayoutContainer::GetSelectedWidgets
	(
	JPtrArray<BaseWidget>* list
	)
	const
{
	list->CleanOut();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	ForEach([&list](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		if (widget != nullptr && widget->IsSelected())
		{
			list->Append(const_cast<BaseWidget*>(widget));
		}
	},
	true);
}

/******************************************************************************
 RemoveSelectedWidgets

 ******************************************************************************/

void
LayoutContainer::RemoveSelectedWidgets()
{
	auto* newUndo = jnew LayoutUndo(itsDoc);

	JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
	GetSelectedWidgets(&list);
	list.DeleteAll();

	NewUndo(newUndo);
	itsDoc->DataChanged();
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
LayoutContainer::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor(JColorManager::GetWhiteColor());
	const JRect frame = GetAperture();

	JCoordinate x = itsGridSpacing;
	while (x < frame.width())
	{
		JCoordinate y = itsGridSpacing;
		while (y < frame.height())
		{
			p.Point(x,y);
			y += itsGridSpacing;
		}
		x += itsGridSpacing;
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
LayoutContainer::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (IsDNDTarget())
	{
		p.SetPenColor(JColorManager::GetDefaultDNDBorderColor());
	}
	else
	{
		p.SetPenColor(JColorManager::GetBlackColor());
	}
	p.SetFilling(true);
	p.Rect(frame);
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

 ******************************************************************************/

void
LayoutContainer::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	if (!itsIgnoreResizeFlag && itsResizeUndo == nullptr)
	{
		auto* newUndo = jnew LayoutUndo(itsDoc);
		NewUndo(newUndo);
		itsResizeUndo = newUndo;
	}

	JXWidget::EnclosingBoundsResized(dwb,dhb);
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
LayoutContainer::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidget::BoundsResized(dw,dh);
	itsDoc->DataChanged();
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && clickCount == 1)
	{
		ClearSelection();

		JPainter* p = CreateDragInsidePainter();
		p->Rect(JRect(pt, pt));
	}

	itsStartPt = itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPainter* p = nullptr;
	if (buttonStates.left() && pt != itsPrevPt && GetDragPainter(&p))	// no painter for multiple click
	{
		const JRect r(itsStartPt, pt);
		const JRect r1 = LocalToGlobal(r);
		ForEach([&r1](JXContainer* obj)
		{
			auto* widget = dynamic_cast<BaseWidget*>(obj);
			if (widget == nullptr)
			{
				return;		// used for rendering
			}

			JRect r2;
			widget->SetSelected(JIntersection(obj->GetFrameGlobal(), r1, &r2));
		},
		false);

		Redraw();
		p->Rect(r);
	}

	itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPainter* p = nullptr;
	if (button == kJXLeftButton && GetDragPainter(&p))	// no painter for multiple click
	{
		p->Rect(JRect(itsStartPt, itsPrevPt));
		DeleteDragPainter();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

bool
LayoutContainer::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == nullptr || !GetAperture().Contains(pt))
	{
		return false;
	}

	auto* widget = dynamic_cast<const BaseWidget*>(source);
	if (widget != nullptr && widget->GetLayoutContainer() == this)
	{
		return true;
	}

	for (auto type : typeList)
	{
		if (type == itsLayoutXAtom)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleDNDEnter()
{
}

/******************************************************************************
 HandleDNDHere (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget* source
	)
{
	// todo: show rects
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleDNDLeave()
{
	Refresh();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	auto* widget = dynamic_cast<const BaseWidget*>(source);
	if (widget != nullptr && widget->GetLayoutContainer() == this)
	{
		// todo: move widgets
		return;
	}

	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();
	const Atom selName         = dndMgr->GetDNDSelectionName();

	unsigned char* data = nullptr;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(selName, time, itsLayoutXAtom,
						&returnType, &data, &dataLength, &delMethod))
	{
		if (returnType == itsLayoutXAtom)
		{
			const std::string s((char*) data, dataLength);
			std::istringstream input(s);

			// todo: deserialize & place

			if (!input.fail())
			{
				// todo: select new widgets
				itsDoc->DataChanged();

				if (action == dndMgr->GetDNDActionMoveXAtom())
				{
					selMgr->SendDeleteRequest(selName, time);
				}
			}
		}

		selMgr->DeleteData(&data, delMethod);
	}

	Refresh();
}

/******************************************************************************
 AppendEditMenuToToolBar

 ******************************************************************************/

void
LayoutContainer::AppendEditMenuToToolBar
	(
	JXToolBar* toolBar
	)
	const
{
	toolBar->NewGroup();
	toolBar->AppendButton(itsEditMenu, kUndoCmd);
	toolBar->AppendButton(itsEditMenu, kRedoCmd);
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
LayoutContainer::UpdateEditMenu()
{
	itsEditMenu->SetItemEnabled(kUndoCmd, itsFirstRedoIndex > 1);
	itsEditMenu->SetItemEnabled(kRedoCmd, itsFirstRedoIndex <= itsUndoList->GetItemCount());
	itsEditMenu->SetItemEnabled(kClearCmd, HasSelection());
	itsEditMenu->EnableItem(kSelectAllCmd);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
LayoutContainer::HandleEditMenu
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

	else if (index == kClearCmd)
	{
		RemoveSelectedWidgets();
	}

	else if (index == kSelectAllCmd)
	{
		SelectAllWidgets();
	}
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
LayoutContainer::Undo()
{
	assert( itsUndoState == kIdle );

	LayoutUndo* undo;
	if (GetCurrentUndo(&undo))
	{
		itsUndoState = kUndo;
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
LayoutContainer::Redo()
{
	assert( itsUndoState == kIdle );

	LayoutUndo* undo;
	if (GetCurrentRedo(&undo))
	{
		itsUndoState = kRedo;
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

bool
LayoutContainer::GetCurrentUndo
	(
	LayoutUndo** undo
	)
	const
{
	if (itsFirstRedoIndex > 1)
	{
		*undo = itsUndoList->GetItem(itsFirstRedoIndex - 1);
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
LayoutContainer::GetCurrentRedo
	(
	LayoutUndo** redo
	)
	const
{
	if (itsFirstRedoIndex <= itsUndoList->GetItemCount())
	{
		*redo = itsUndoList->GetItem(itsFirstRedoIndex);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 NewUndo (private)

	Register a new Undo object.

	itsFirstRedoIndex points to the first redo object in itsUndoList.
	1 <= itsFirstRedoIndex <= itsUndoList->GetItemCount()+1
	Minimum => everything is redo
	Maximum => everything is undo

 ******************************************************************************/

void
LayoutContainer::NewUndo
	(
	LayoutUndo* undo
	)
{
	itsResizeUndo = nullptr;

	if (itsUndoState == kIdle)
	{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetItemCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
		{
			itsUndoList->DeleteItem(i);
		}

		if (itsLastSaveRedoIndex > 0 &&
			itsFirstRedoIndex < JIndex(itsLastSaveRedoIndex))
		{
			ClearLastSaveLocation();
		}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		ClearOutdatedUndo();
		assert( !itsUndoList->IsEmpty() );
	}

	else if (itsUndoState == kUndo)
	{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetItem(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(true);
	}

	else if (itsUndoState == kRedo)
	{
		assert( itsFirstRedoIndex <= itsUndoList->GetItemCount() );

		itsUndoList->SetItem(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(false);
	}
}

/******************************************************************************
 ReplaceUndo

 ******************************************************************************/

void
LayoutContainer::ReplaceUndo
	(
	LayoutUndo* oldUndo,
	LayoutUndo* newUndo
	)
{
#ifndef NDEBUG

	assert( itsUndoState != kIdle );

	if (itsUndoState == kUndo)
	{
		assert( itsFirstRedoIndex > 1 &&
				oldUndo == itsUndoList->GetItem(itsFirstRedoIndex - 1) );
	}
	else if (itsUndoState == kRedo)
	{
		assert( itsFirstRedoIndex <= itsUndoList->GetItemCount() &&
				oldUndo == itsUndoList->GetItem(itsFirstRedoIndex) );
	}

#endif

	NewUndo(newUndo);
}

/******************************************************************************
 ClearOutdatedUndo (private)

 ******************************************************************************/

void
LayoutContainer::ClearOutdatedUndo()
{
	while (itsUndoList->GetItemCount() > kMaxUndoCount)
	{
		itsUndoList->DeleteItem(1);
		itsFirstRedoIndex--;
		itsLastSaveRedoIndex--;

		// If we have to throw out redo's, we have to toss everything.

		if (itsFirstRedoIndex == 0)
		{
			ClearUndo();
			break;
		}
	}
}

/******************************************************************************
 ClearUndo (private)

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
LayoutContainer::ClearUndo()
{
	itsResizeUndo = nullptr;
	itsUndoList->CleanOut();
	itsFirstRedoIndex = 1;
	ClearLastSaveLocation();
}
