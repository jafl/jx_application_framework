/******************************************************************************
 LayoutContainer.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutContainer.h"
#include "LayoutDocument.h"
#include "LayoutSelection.h"
#include "BaseWidget.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

#include "LayoutContainer-Edit.h"
#include "LayoutContainer-Arrange.h"

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
	itsUndoChain(jnew JUndoRedoChain(true)),
	itsIgnoreResizeFlag(false),
	itsDropRectList(nullptr)
{
	WantInput(true);
	SetFocusColor(JColorManager::GetDefaultBackColor());
	SetBorderWidth(10);

	itsLayoutDataXAtom = GetDisplay()->RegisterXAtom(LayoutSelection::GetDataXAtomName());
	itsLayoutMetaXAtom = GetDisplay()->RegisterXAtom(LayoutSelection::GetMetaXAtomName());

	itsEditMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::LayoutContainer_Edit"));
	itsEditMenu->SetMenuItems(kEditMenuStr);
	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsEditMenu->AttachHandlers(this,
		&LayoutContainer::UpdateEditMenu,
		&LayoutContainer::HandleEditMenu);
	ConfigureEditMenu(itsEditMenu);

	itsArrangeMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::LayoutContainer_Arrange"));
	itsArrangeMenu->SetMenuItems(kArrangeMenuStr);
	itsArrangeMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsArrangeMenu->AttachHandlers(this,
		&LayoutContainer::UpdateArrangeMenu,
		&LayoutContainer::HandleArrangeMenu);
	ConfigureArrangeMenu(itsArrangeMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutContainer::~LayoutContainer()
{
	jdelete itsUndoChain;
	jdelete itsDropRectList;
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
 GetSelectionCount

 ******************************************************************************/

JSize
LayoutContainer::GetSelectionCount()
	const
{
	JSize count = 0;
	ForEach([&count](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		if (widget != nullptr && widget->IsSelected())
		{
			count++;
		}
	},
	true);

	return count;
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
	false);
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

bool
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

	return !list->IsEmpty();
}

/******************************************************************************
 SetSelectedWidgetsVisible

 ******************************************************************************/

void
LayoutContainer::SetSelectedWidgetsVisible
	(
	const bool visible
	)
{
	JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
	GetSelectedWidgets(&list);
	for (auto* w : list)
	{
		w->SetVisible(visible);
	}
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
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
LayoutContainer::Clear
	(
	const bool isUndoRedo
	)
{
	DeleteEnclosedObjects();

	if (!isUndoRedo)
	{
		itsUndoChain->ClearUndo();
	}
}

/******************************************************************************
 GenerateCode (private)

 ******************************************************************************/

void
LayoutContainer::GenerateCode
	(
	std::ostream&		output,
	const JString&		indent,
	JPtrArray<JString>*	objTypes,
	JPtrArray<JString>*	objNames,
	JStringManager*		stringdb
	)
	const
{
	JPtrArray<BaseWidget> inputWidgets(JPtrArrayT::kForgetAll);
	ForEach([&output, &indent, objTypes, objNames, stringdb, &inputWidgets](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		if (widget == nullptr)
		{
			return;
		}

		if (widget->WantsInput())
		{
			inputWidgets.Append(const_cast<BaseWidget*>(widget));
		}
		else
		{
			widget->GenerateCode(output, indent, objTypes, objNames, stringdb);
		}
	},
	true);

	// ensure tab order is maintained

	inputWidgets.SetCompareFunction(CompareTabOrder);
	inputWidgets.Sort();

	for (auto* widget: inputWidgets)
	{
		widget->GenerateCode(output, indent, objTypes, objNames, stringdb);
	}
}

/******************************************************************************
 GetEnclosureName

 ******************************************************************************/

JString
LayoutContainer::GetEnclosureName()
	const
{
	return "window";
}

/******************************************************************************
 GetStringNamespace

 ******************************************************************************/

JString
LayoutContainer::GetStringNamespace()
	const
{
	JString ns("::");
	ns += itsDoc->GetName();
	ns += "::";
	ns += itsDoc->GetCodeTag();
	return ns;
}

/******************************************************************************
 GenerateUniqueVarName

 ******************************************************************************/

JString
LayoutContainer::GenerateUniqueVarName()
	const
{
	const JString& base = JGetString("VarNameBase::LayoutContainer");

	JUInt i = 0;
	ForEach([&base, &i](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		bool isMember;
		if (widget != nullptr && widget->GetVarName(&isMember).StartsWith(base))
		{
			const JString s(widget->GetVarName(&isMember).GetBytes() + base.GetByteCount(), JString::kNoCopy);
			JUInt j;
			if (s.ConvertToUInt(&j) && j > i)
			{
				i = j;
			}
		}
	},
	true);

	return base + JString((JUInt64) (i+1));
}

/******************************************************************************
 GetNextTabIndex

 ******************************************************************************/

JIndex
LayoutContainer::GetNextTabIndex()
	const
{
	JIndex i = 0;
	ForEach([&i](const JXContainer* obj)
	{
		auto* widget = dynamic_cast<const BaseWidget*>(obj);
		JIndex j;
		if (widget != nullptr && widget->GetTabIndex(&j))
		{
			i = JMax(i, j);
		}
	},
	true);

	return i+1;
}

/******************************************************************************
 TabIndexRemoved

 ******************************************************************************/

void
LayoutContainer::TabIndexRemoved
	(
	const JIndex i
	)
{
	ForEach([i](JXContainer* obj)
	{
		auto* widget = dynamic_cast<BaseWidget*>(obj);
		JIndex j;
		if (widget != nullptr && widget->GetTabIndex(&j) && j > i)
		{
			widget->SetTabIndex(j-1);
		}
	},
	true);
}

/******************************************************************************
 SnapToGrid

 ******************************************************************************/

JPoint
LayoutContainer::SnapToGrid
	(
	const JPoint& pt
	)
	const
{
	if (GetDisplay()->GetLatestKeyModifiers().shift())
	{
		return pt;
	}

	return JPoint(
		JRound(pt.x / JFloat(itsGridSpacing)) * itsGridSpacing,
		JRound(pt.y / JFloat(itsGridSpacing)) * itsGridSpacing);
}

JPoint
LayoutContainer::SnapToGrid
	(
	JXContainer* obj
	)
	const
{
	const JPoint tl = obj->GetFrame().topLeft();
	const JPoint d  = SnapToGrid(tl) - tl;
	obj->Move(d.x, d.y);
	return d;
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
 DrawOver (virtual protected)

	Draw DND outlines.

 ******************************************************************************/

void
LayoutContainer::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsDropRectList == nullptr || (itsDropPt.x == -1 && itsDropPt.y == -1))
	{
		return;
	}

	p.SetPenColor(JColorManager::GetDefaultSelectionColor());

	for (auto r : *itsDropRectList)
	{
		r.Shift(itsDropPt);
		r.Shift(-itsDropOffset);

		const JPoint d = SnapToGrid(r.topLeft());
		r.Shift(d - r.topLeft());

		p.Rect(r);
	}
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

	We sometimes get spurious resize events of 1 pixel, which we ignore.

 ******************************************************************************/

void
LayoutContainer::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	if ((labs(dwb) >= JCoordinate(itsGridSpacing) || labs(dhb) >= JCoordinate(itsGridSpacing)) &&
		!itsIgnoreResizeFlag && !CurrentUndoIs(LayoutUndo::kWindowResizeType))
	{
		auto* newUndo = jnew LayoutUndo(itsDoc, LayoutUndo::kWindowResizeType);
		NewUndo(newUndo, false);
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
	if (labs(dw) >= JCoordinate(itsGridSpacing) || labs(dh) >= JCoordinate(itsGridSpacing))
	{
		itsDoc->DataModified();
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
LayoutContainer::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
	if (!GetSelectedWidgets(&list))
	{
		return;
	}

	LayoutUndo* undo = nullptr;
	if (!CurrentUndoIs(LayoutUndo::kArrowType))
	{
		undo = jnew LayoutUndo(itsDoc, LayoutUndo::kArrowType);
	}

	bool changed = false;

	const JCoordinate delta = modifiers.shift() ? 1 : itsGridSpacing;

	if (c == kJUpArrow && modifiers.meta())
	{
		for (auto* w : list)
		{
			w->AdjustSize(0, -delta);
		}
		changed = true;
	}
	else if (c == kJDownArrow && modifiers.meta())
	{
		for (auto* w : list)
		{
			w->AdjustSize(0, delta);
		}
		changed = true;
	}
	else if (c == kJLeftArrow && modifiers.meta())
	{
		for (auto* w : list)
		{
			w->AdjustSize(-delta, 0);
		}
		changed = true;
	}
	else if (c == kJRightArrow && modifiers.meta())
	{
		for (auto* w : list)
		{
			w->AdjustSize(delta, 0);
		}
		changed = true;
	}

	else if (c == kJUpArrow)
	{
		for (auto* w : list)
		{
			w->Move(0, -delta);
		}
		changed = true;
	}
	else if (c == kJDownArrow)
	{
		for (auto* w : list)
		{
			w->Move(0, delta);
		}
		changed = true;
	}
	else if (c == kJLeftArrow)
	{
		for (auto* w : list)
		{
			w->Move(-delta, 0);
		}
		changed = true;
	}
	else if (c == kJRightArrow)
	{
		for (auto* w : list)
		{
			w->Move(delta, 0);
		}
		changed = true;
	}

	if (changed && undo != nullptr)
	{
		NewUndo(undo);
	}
	else if (undo != nullptr)
	{
		jdelete undo;
	}
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

	bool found = false;
	for (auto type : typeList)
	{
		if (type == itsLayoutMetaXAtom)
		{
			JXSelectionManager* selMgr = GetSelectionManager();
			JXDNDManager* dndMgr       = GetDNDManager();
			const Atom selName         = dndMgr->GetDNDSelectionName();

			unsigned char* data = nullptr;
			JSize dataLength;
			Atom returnType;
			JXSelectionManager::DeleteMethod delMethod;
			if (selMgr->GetData(selName, CurrentTime, itsLayoutMetaXAtom,
								&returnType, &data, &dataLength, &delMethod))
			{
				if (returnType == itsLayoutMetaXAtom)
				{
					const std::string s((char*) data, dataLength);
					std::istringstream input(s);

					itsDropRectList = jnew JArray<JRect>;
					JRect bounds;
					LayoutSelection::ReadMetaData(input, &itsBoundsOffset, &bounds, &itsDropOffset, itsDropRectList);

					if (GetAperture().Contains(bounds))
					{
						itsDropPt.Set(-1,-1);
						found = true;
					}
					else
					{
						jdelete itsDropRectList;
						itsDropRectList = nullptr;
					}
				}

				selMgr->DeleteData(&data, delMethod);
			}

			break;
		}
	}

	return found;
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
	itsDropPt = pt;
	Refresh();	// draw outlines
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
LayoutContainer::HandleDNDLeave()
{
	jdelete itsDropRectList;
	itsDropRectList = nullptr;
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
	const BaseWidget* sourceWidget = dynamic_cast<const BaseWidget*>(source);
	if (sourceWidget != nullptr && sourceWidget->GetLayoutContainer() == this &&
		action == GetDNDManager()->GetDNDActionMoveXAtom())
	{
		auto* newUndo = jnew LayoutUndo(itsDoc);

		const JPoint pt1G  = sourceWidget->GetDragStartPointGlobal();
		const JPoint pt2G  = LocalToGlobal(pt);
		const JPoint delta = pt2G - pt1G;

		JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
		GetSelectedWidgets(&list);
		for (auto* w : list)
		{
			w->SetVisible(true);
			w->Move(delta.x, delta.y);
			SnapToGrid(w);
		}

		NewUndo(newUndo);

		HandleDNDLeave();
		Refresh();
		return;
	}

	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();
	const Atom selName         = dndMgr->GetDNDSelectionName();

	unsigned char* data = nullptr;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(selName, time, itsLayoutDataXAtom,
						&returnType, &data, &dataLength, &delMethod))
	{
		if (returnType == itsLayoutDataXAtom)
		{
			auto* newUndo = jnew LayoutUndo(itsDoc);
			bool changed  = false;

			JPoint delta = -itsBoundsOffset;
			delta += pt;
			delta -= itsDropOffset;

			const std::string s((char*) data, dataLength);
			std::istringstream input(s);

			JPtrArray<JXWidget> widgetList(JPtrArrayT::kForgetAll);
			while (!input.eof() && !input.fail())
			{
				bool keepGoing;
				input >> keepGoing;
				if (!keepGoing)
				{
					break;
				}

				BaseWidget* widget = itsDoc->ReadWidget(input, this, &widgetList);
				widget->Move(delta.x, delta.y);
				SnapToGrid(widget);
				changed = true;
			}

			if (!input.fail() && action == dndMgr->GetDNDActionMoveXAtom())
			{
				selMgr->SendDeleteRequest(selName, time);
			}

			if (changed)
			{
				NewUndo(newUndo);
			}
			else
			{
				jdelete newUndo;
			}
		}

		selMgr->DeleteData(&data, delMethod);
	}

	HandleDNDLeave();
	Refresh();
}

/******************************************************************************
 AppendToToolBar

 ******************************************************************************/

void
LayoutContainer::AppendToToolBar
	(
	JXToolBar* toolBar
	)
	const
{
	toolBar->NewGroup();
	toolBar->AppendButton(itsEditMenu, kUndoCmd);
	toolBar->AppendButton(itsEditMenu, kRedoCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsArrangeMenu, kAlignLeftCmd);
	toolBar->AppendButton(itsArrangeMenu, kAlignHorizCenterCmd);
	toolBar->AppendButton(itsArrangeMenu, kAlignRightCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsArrangeMenu, kAlignTopCmd);
	toolBar->AppendButton(itsArrangeMenu, kAlignVertCenterCmd);
	toolBar->AppendButton(itsArrangeMenu, kAlignBottomCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsArrangeMenu, kDistrHorizCmd);
	toolBar->AppendButton(itsArrangeMenu, kDistrVertCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(itsArrangeMenu, kExpandHorizCmd);
	toolBar->AppendButton(itsArrangeMenu, kExpandVertCmd);
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
LayoutContainer::UpdateEditMenu()
{
	bool canUndo, canRedo;
	itsUndoChain->HasMultipleUndo(&canUndo, &canRedo);

	itsEditMenu->SetItemEnabled(kUndoCmd, canUndo);
	itsEditMenu->SetItemEnabled(kRedoCmd, canRedo);
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
		itsUndoChain->Undo();
	}
	else if (index == kRedoCmd)
	{
		itsUndoChain->Redo();
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
 CurrentUndoIs

 ******************************************************************************/

bool
LayoutContainer::CurrentUndoIs
	(
	const LayoutUndo::Type type
	)
	const
{
	JUndo* tmpUndo;
	return (itsUndoChain->GetCurrentUndo(&tmpUndo) &&
			tmpUndo->IsActive() &&
			dynamic_cast<LayoutUndo*>(tmpUndo)->GetType() == type);
}

/******************************************************************************
 NewUndo

	Register a new Undo object.

 ******************************************************************************/

void
LayoutContainer::NewUndo
	(
	LayoutUndo*	undo,
	const bool	setChanged
	)
{
	itsUndoChain->NewUndo(undo);
	if (setChanged)
	{
		itsDoc->UpdateSaveState();
	}
}

/******************************************************************************
 UpdateArrangeMenu (private)

 ******************************************************************************/

void
LayoutContainer::UpdateArrangeMenu()
{
	JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
	GetSelectedWidgets(&list);

	if (list.GetItemCount() > 1)
	{
		itsArrangeMenu->EnableItem(kAlignLeftCmd);
		itsArrangeMenu->EnableItem(kAlignHorizCenterCmd);
		itsArrangeMenu->EnableItem(kAlignRightCmd);
		itsArrangeMenu->EnableItem(kAlignTopCmd);
		itsArrangeMenu->EnableItem(kAlignVertCenterCmd);
		itsArrangeMenu->EnableItem(kAlignBottomCmd);
	}

	if (list.GetItemCount() > 2)
	{
		itsArrangeMenu->EnableItem(kDistrHorizCmd);
		itsArrangeMenu->EnableItem(kDistrVertCmd);
	}

	if (!list.IsEmpty())
	{
		itsArrangeMenu->EnableItem(kExpandHorizCmd);
		itsArrangeMenu->EnableItem(kExpandVertCmd);
	}

	const JIndex max = GetNextTabIndex()-1;
	if (max > 0)
	{
		bool incr = true, decr = true;
		JIndex i;
		for (auto* w : list)
		{
			if (!w->GetTabIndex(&i))
			{
				continue;
			}

			if (i == 1)
			{
				decr = false;
			}
			if (i == max)
			{
				incr = false;
			}
		}

		itsArrangeMenu->SetItemEnabled(kDecrementTabIndexCmd, decr);
		itsArrangeMenu->SetItemEnabled(kIncrementTabIndexCmd, incr);
	}
}

/******************************************************************************
 HandleArrangeMenu (private)

 ******************************************************************************/

void
LayoutContainer::HandleArrangeMenu
	(
	const JIndex index
	)
{
	JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
	GetSelectedWidgets(&list);

	JRect bounds;
	for (auto* w : list)
	{
		bounds = bounds.IsEmpty() ? w->GetFrame() : JCovering(bounds, w->GetFrame());
	}

	auto* newUndo = jnew LayoutUndo(itsDoc);

	if (index == kAlignLeftCmd)
	{
		for (auto* w : list)
		{
			w->Move(bounds.left - w->GetFrame().left, 0);
		}
	}
	else if (index == kAlignHorizCenterCmd)
	{
		const auto x = bounds.xcenter();
		for (auto* w : list)
		{
			w->Move(x - w->GetFrame().xcenter(), 0);
		}
	}
	else if (index == kAlignRightCmd)
	{
		for (auto* w : list)
		{
			w->Move(bounds.right - w->GetFrame().right, 0);
		}
	}

	else if (index == kAlignTopCmd)
	{
		for (auto* w : list)
		{
			w->Move(0, bounds.top - w->GetFrame().top);
		}
	}
	else if (index == kAlignVertCenterCmd)
	{
		const auto y = bounds.ycenter();
		for (auto* w : list)
		{
			w->Move(0, y - w->GetFrame().ycenter());
		}
	}
	else if (index == kAlignBottomCmd)
	{
		for (auto* w : list)
		{
			w->Move(0, bounds.bottom - w->GetFrame().bottom);
		}
	}

	else if (index == kDistrHorizCmd)
	{
		JCoordinate w = bounds.width();
		for (auto* widget : list)
		{
			w -= widget->GetFrameGlobal().width();
		}

		const auto count = list.GetItemCount();

		w /= (count-1);
		if (w > 0)
		{
			list.SetCompareFunction(std::function([](const BaseWidget* w1, const BaseWidget* w2)
			{
				return w1->GetFrameGlobal().left <=> w2->GetFrameGlobal().left;
			}));
			list.Sort();

			auto x = bounds.left + list.GetFirstItem()->GetFrameWidth();
			for (JIndex i=2; i<=count; i++)
			{
				auto* widget = list.GetItem(i);
				auto r       = widget->GetFrame();
				widget->Move(x + w - r.left, 0);
				x += w + r.width();
			}
		}
	}
	else if (index == kDistrVertCmd)
	{
		JCoordinate h = bounds.height();
		for (auto* widget : list)
		{
			h -= widget->GetFrameGlobal().height();
		}

		const auto count = list.GetItemCount();

		h /= (count-1);
		if (h > 0)
		{
			list.SetCompareFunction(std::function([](const BaseWidget* w1, const BaseWidget* w2)
			{
				return w1->GetFrameGlobal().top <=> w2->GetFrameGlobal().top;
			}));
			list.Sort();

			auto y = bounds.top + list.GetFirstItem()->GetFrameHeight();
			for (JIndex i=2; i<=count; i++)
			{
				auto* widget = list.GetItem(i);
				auto r       = widget->GetFrame();
				widget->Move(0, y + h - r.top);
				y += h + r.height();
			}
		}
	}

	else if (index == kExpandHorizCmd)
	{
		const JCoordinate w1 = GetFrameWidth();
		for (auto* w : list)
		{
			const JRect r = w->GetFrame();
			w->Place(0, r.top);
			w->SetSize(w1, r.height());
		}
	}
	else if (index == kExpandVertCmd)
	{
		const JCoordinate h = GetFrameHeight();
		for (auto* w : list)
		{
			const JRect r = w->GetFrame();
			w->Place(r.left, 0);
			w->SetSize(r.width(), h);
		}
	}

	else if (index == kDecrementTabIndexCmd)
	{
		list.SetCompareFunction(CompareTabOrder);
		list.Sort();

		for (auto* w : list)
		{
			JIndex i;
			if (w->GetTabIndex(&i) && i > 1)
			{
				AnyOf([i,w](const JXContainer* obj)
				{
					auto* widget = dynamic_cast<const BaseWidget*>(obj);
					JIndex j;
					if (widget != nullptr && widget != w &&
						widget->GetTabIndex(&j) && j == i-1)
					{
						w->SetTabIndex(j);
						const_cast<BaseWidget*>(widget)->SetTabIndex(i);
						return true;
					}
					return false;
				},
				true);
			}
		}
	}
	else if (index == kIncrementTabIndexCmd)
	{
		list.SetCompareFunction(std::function([](const BaseWidget* w1, const BaseWidget* w2)
		{
			JIndex i1,i2;
			w1->GetTabIndex(&i1);
			w2->GetTabIndex(&i2);
			return i2 <=> i1;
		}));
		list.Sort();

		for (auto* w : list)
		{
			JIndex i;
			if (w->GetTabIndex(&i))
			{
				AnyOf([i,w](const JXContainer* obj)
				{
					auto* widget = dynamic_cast<const BaseWidget*>(obj);
					JIndex j;
					if (widget != nullptr && widget != w &&
						widget->GetTabIndex(&j) && j == i+1)
					{
						w->SetTabIndex(j);
						const_cast<BaseWidget*>(widget)->SetTabIndex(i);
						return true;
					}
					return false;
				},
				true);
			}
		}
	}

	NewUndo(newUndo);
}

/******************************************************************************
 CompareTabOrder (static private)

 ******************************************************************************/

std::weak_ordering
LayoutContainer::CompareTabOrder
	(
	BaseWidget *const w1,
	BaseWidget *const w2
	)
{
	JIndex i1,i2;
	w1->GetTabIndex(&i1);
	w2->GetTabIndex(&i2);
	return i1 <=> i2;
}
