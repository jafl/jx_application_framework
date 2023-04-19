/******************************************************************************
 JXDockWidget.cpp

	Maintains a set of docked JXWindows.

	BASE CLASS = JXWidgetSet

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#include "JXDockWidget.h"
#include "JXDockManager.h"
#include "JXDockDirector.h"
#include "JXHorizDockPartition.h"
#include "JXVertDockPartition.h"
#include "JXDockDragData.h"
#include "JXUrgentFunctionTask.h"
#include "JXFileDocument.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXPartition.h"
#include "JXTabGroup.h"
#include "JXHintDirector.h"
#include "JXFunctionTask.h"
#include "JXWindowPainter.h"
#include "JXGC.h"
#include "jXPainterUtil.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const Time kDeleteHintDelay = 10000;	// 10 sec (ms)

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockWidget::JXDockWidget
	(
	JXDockDirector*		director,
	JXPartition*		partition,
	const bool		isHorizPartition,
	JXTabGroup*			tabGroup,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h),
	itsDirector(director),
	itsPartition(partition),
	itsIsHorizFlag(isHorizPartition),
	itsTabGroup(tabGroup),
	itsID((JXGetDockManager())->GetUniqueDockID()),
	itsWindowList(nullptr),
	itsMinSize(kDefaultMinSize, kDefaultMinSize),
	itsChildPartition(nullptr),
	itsHintDirector(nullptr),
	itsDeleteHintTask(nullptr)
{
	assert( itsDirector != nullptr );
	assert( itsPartition != nullptr );
	assert( itsTabGroup != nullptr );

	if (!JXGetDockManager()->IsReadingSetup())
	{
		JXGetDockManager()->IDUsed(itsID);
	}

	ListenTo(itsTabGroup);
	ListenTo(itsTabGroup->GetCardEnclosure());

	auto* task = jnew JXUrgentFunctionTask(this, std::bind(&JXDockWidget::UpdateMinSize, this));
	assert( task != nullptr );
	task->Go();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockWidget::~JXDockWidget()
{
	const JXDockManager::CloseDockMode mode = JXGetDockManager()->GetCloseDockMode();
	if (itsWindowList != nullptr && mode == JXDockManager::kUndockWindows)
	{
		// can't call UndockAll() because that calls UpdateMinSize()

		for (auto* w : *itsWindowList)
		{
			StopListening(w);
			w->Undock();
		}

		jdelete itsWindowList;
	}
	else if (itsWindowList != nullptr)
	{
		assert( mode == JXDockManager::kCloseWindows );

		for (auto* w : *itsWindowList)
		{
			StopListening(w);
			w->Close();
		}

		jdelete itsWindowList;
	}

	jdelete itsDeleteHintTask;
}

/******************************************************************************
 SetID

 ******************************************************************************/

void
JXDockWidget::SetID
	(
	const JIndex id
	)
{
	itsID = id;
	(JXGetDockManager())->IDUsed(id);
}

/******************************************************************************
 WindowWillFit

 ******************************************************************************/

bool
JXDockWidget::WindowWillFit
	(
	JXWindow* w
	)
	const
{
	const JPoint minSize = w->GetMinSize();
	return (GetApertureWidth()  >= minSize.x &&
			GetApertureHeight() >= minSize.y);
}

/******************************************************************************
 Dock

 ******************************************************************************/

bool
JXDockWidget::Dock
	(
	JXWindowDirector*	d,
	const bool		reportError
	)
{
	return Dock(d->GetWindow(), reportError);
}

bool
JXDockWidget::Dock
	(
	JXWindow*		w,
	const bool	reportError
	)
{
	if (itsChildPartition != nullptr)
	{
		return false;
	}

	const JRect geom = GetApertureGlobal();
	if (w->Dock(this, JXWidgetSet::GetWindow()->GetXWindow(), geom))
	{
		if (itsWindowList == nullptr)
		{
			itsWindowList = jnew JPtrArray<JXWindow>(JPtrArrayT::kForgetAll);
			assert( itsWindowList != nullptr );
		}

		const JIndex index = GetTabInsertionIndex(w);

		itsWindowList->InsertAtIndex(index, w);
		ListenTo(w);
		UpdateMinSize();

		itsTabGroup->InsertTab(index, w->GetTitle());
		// tab will be selected when window shows itself

		return true;
	}
	else
	{
		if (reportError)
		{
			JGetUserNotification()->ReportError(JGetString("WindowWillNotFit::JXDockWidget"));
		}
		return false;
	}
}

/******************************************************************************
 GetTabInsertionIndex (private)

	Linear search is sufficient.  If you have enough tabs that this would
	be a performance problem, you already have much bigger problems.

 ******************************************************************************/

JIndex
JXDockWidget::GetTabInsertionIndex
	(
	JXWindow*		w,
	const JIndex	ignoreIndex
	)
	const
{
	const JSize count = itsWindowList->GetElementCount();
	assert( count == itsTabGroup->GetTabCount() );

	const JUtf8Byte* title =
		JXFileDocument::SkipNeedsSavePrefix(w->GetTitle().GetBytes());

	JIndex index = count+1;
	for (JIndex i=1; i<=count; i++)
	{
		if (i == ignoreIndex)
		{
			continue;
		}

		const JUtf8Byte* t =
			JXFileDocument::SkipNeedsSavePrefix(itsWindowList->GetElement(i)->GetTitle().GetBytes());
		if (JString::Compare(title, t, JString::kIgnoreCase) < 0)
		{
			index = i;
			break;
		}
	}

	return index;
}

/******************************************************************************
 TransferAll

	Transfers as many windows as possible to the target docking location.
	Returns false if not all windows could be transferred.

 ******************************************************************************/

bool
JXDockWidget::TransferAll
	(
	JXDockWidget* target
	)
{
	bool success = true;

	if (itsWindowList != nullptr)
	{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
		{
			JXWindow* w = itsWindowList->GetElement(i);
			if (!target->Dock(w))
			{
				success = false;
			}
		}
	}

	return success;
}

/******************************************************************************
 UndockAll

 ******************************************************************************/

void
JXDockWidget::UndockAll()
{
	if (itsWindowList != nullptr)
	{
		for (auto* w : *itsWindowList)
		{
			itsTabGroup->DeleteTab(1);

			StopListening(w);
			w->Undock();
		}

		itsWindowList->RemoveAll();
		jdelete itsWindowList;
		itsWindowList = nullptr;

		UpdateMinSize();
	}
}

/******************************************************************************
 CloseAll

 ******************************************************************************/

bool
JXDockWidget::CloseAll()
{
	if (itsWindowList != nullptr)
	{
		JXDisplay* display = GetDisplay();
		Display* xDisplay  = display->GetXDisplay();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
		{
			JXWindow* w = itsWindowList->GetElement(i);
			StopListening(w);

			Window xWindow = w->GetXWindow();
			if (w->Close())
			{
				if (JXDisplay::WindowExists(display, xDisplay, xWindow))
				{
					w->Undock();
				}
				else if (JXGetApplication()->DisplayExists(xDisplay))
				{
					itsTabGroup->DeleteTab(i);
					itsWindowList->RemoveElement(i);
				}
				else
				{
					return false;
				}
			}
			else
			{
				UpdateMinSize();
				return false;
			}
		}

		jdelete itsWindowList;
		itsWindowList = nullptr;

		UpdateMinSize();
	}

	return true;
}

/******************************************************************************
 ShowPreviousWindow

 ******************************************************************************/

void
JXDockWidget::ShowPreviousWindow()
{
	itsTabGroup->ShowPreviousTab();
}

/******************************************************************************
 ShowNextWindow

 ******************************************************************************/

void
JXDockWidget::ShowNextWindow()
{
	itsTabGroup->ShowNextTab();
}

/******************************************************************************
 GetHorizChildPartition

 ******************************************************************************/

bool
JXDockWidget::GetHorizChildPartition
	(
	JXHorizDockPartition** p
	)
	const
{
	if (!itsIsHorizFlag && itsChildPartition != nullptr)
	{
		*p = dynamic_cast<JXHorizDockPartition*>(itsChildPartition);
		assert( *p != nullptr );
		return true;
	}
	else
	{
		*p = nullptr;
		return false;
	}
}

/******************************************************************************
 GetVertChildPartition

 ******************************************************************************/

bool
JXDockWidget::GetVertChildPartition
	(
	JXVertDockPartition** p
	)
	const
{
	if (itsIsHorizFlag && itsChildPartition != nullptr)
	{
		*p = dynamic_cast<JXVertDockPartition*>(itsChildPartition);
		assert( *p != nullptr );
		return true;
	}
	else
	{
		*p = nullptr;
		return false;
	}
}

/******************************************************************************
 SetChildPartition

 ******************************************************************************/

void
JXDockWidget::SetChildPartition
	(
	JXPartition* p
	)
{
	assert( itsWindowList == nullptr );

	if (itsChildPartition != nullptr)
	{
		StopListening(itsChildPartition);
	}

	itsChildPartition = p;

	if (itsChildPartition != nullptr)
	{
		ClearWhenGoingAway(itsChildPartition, &itsChildPartition);
	}

	UpdateMinSize();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept JXDockDragData.

 ******************************************************************************/

bool
JXDockWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	// we only accept local drops

	if (source == nullptr)
	{
		return false;
	}

	// we accept drops of type JXDockDragData

	const Atom minSizeAtom = (JXGetDockManager())->GetDNDMinSizeAtom();

	bool acceptDrop = false;
	for (const auto type : typeList)
	{
		if (type != minSizeAtom)
		{
			continue;
		}

		JXSelectionManager* selMgr = GetSelectionManager();
		JXDNDManager* dndMgr       = GetDNDManager();

		*action = dndMgr->GetDNDActionPrivateXAtom();

		// check if window will fit

		const Atom selectionName = dndMgr->GetDNDSelectionName();
		Atom returnType;
		unsigned char* data;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (selMgr->GetData(selectionName, time, minSizeAtom,
							&returnType, &data, &dataLength, &delMethod))
		{
			if (returnType == XA_POINT)
			{
				auto* minSize = (XPoint*) data;
				if (GetApertureWidth()  >= minSize->x &&
					GetApertureHeight() >= minSize->y)
				{
					acceptDrop = true;
				}
			}
			selMgr->DeleteData(&data, delMethod);
		}

		break;
	}

	if (!acceptDrop && itsHintDirector == nullptr)
	{
		JRect r = GetBounds();
		r.Shrink(r.width()/2 - 5, r.height()/2 - 5);

		itsHintDirector =
			jnew JXHintDirector((JXContainer::GetWindow())->GetDirector(), this,
							   r, JGetString("WindowWillNotFit::JXDockWidget"));
		assert( itsHintDirector != nullptr );
		itsHintDirector->Activate();

		itsDeleteHintTask = jnew JXFunctionTask(kDeleteHintDelay, [this]()
		{
			itsDeleteHintTask = nullptr;
			itsHintDirector->Close();
			itsHintDirector = nullptr;
		},
		true);
		assert( itsDeleteHintTask != nullptr );
		itsDeleteHintTask->Start();
	}

	return acceptDrop;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
JXDockWidget::HandleDNDDrop
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
						time, (JXGetDockManager())->GetDNDWindowAtom(),
						&returnType, &data, &dataLength, &delMethod))
	{
		JXWindow* window;
		if (returnType == XA_WINDOW &&
			GetDisplay()->FindXWindow(* (Window*) data, &window))
		{
			Dock(window, true);
		}

		selMgr->DeleteData(&data, delMethod);
	}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	Remove the blue border.

 ******************************************************************************/

void
JXDockWidget::HandleDNDLeave()
{
	if (itsWindowList != nullptr)
	{
		for (auto* w : *itsWindowList)
		{
			w->Refresh();
		}
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXDockWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect ap = GetAperture();
	if (IsDNDTarget())
	{
		(p.GetGC())->SetSubwindowMode(IncludeInferiors);
		JXDrawDNDBorder(p, ap, 2);
		(p.GetGC())->SetSubwindowMode(ClipByChildren);
	}
	else
	{
		JXDrawDownFrame(p, ap, 2);
	}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXDockWidget::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXWidgetSet::BoundsMoved(dx, dy);

	if (itsWindowList != nullptr)
	{
		for (auto* w : *itsWindowList)
		{
			w->UndockedMove(dx, dy);
		}
	}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXDockWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidgetSet::BoundsResized(dw, dh);

	if (itsWindowList != nullptr)
	{
		// Use SetSize() instead of AdjustSize() because JXWindow might
		// have max size, in which case deltas will be wrong.

		const JRect boundsG = GetBoundsGlobal();

		for (auto* w : *itsWindowList)
		{
			w->UndockedSetSize(boundsG.width(), boundsG.height());
		}
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDockWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXWindow* w;
	JIndex i;
	if (sender != JXWidgetSet::GetWindow() && message.Is(JXWindow::kUndocked))
	{
		RemoveWindow(sender);
		StopListening(sender);
	}
	else if (FindWindow(sender, &w, &i))
	{
		if (message.Is(JXWindow::kMinSizeChanged))
		{
			const JPoint minSize = w->GetMinSize();
			const JRect boundsG  = GetBoundsGlobal();
			if (boundsG.width()  < minSize.x ||
				boundsG.height() < minSize.y)
			{
				w->Undock();
			}
		}
		else if (message.Is(JXWindow::kTitleChanged))
		{
			JIndex index = GetTabInsertionIndex(w, i);
			if (index != i+1)
			{
				if (index > i)
				{
					index--;
				}

				JIndex currIndex;
				const bool hadSelection = itsTabGroup->GetCurrentTabIndex(&currIndex);

				auto* card = dynamic_cast<JXWidgetSet*>(itsTabGroup->RemoveTab(i));
				itsTabGroup->InsertTab(index, w->GetTitle(), card);

				itsWindowList->MoveElementToIndex(i, index);

				if (hadSelection && i == currIndex)
				{
					itsTabGroup->ShowTab(index);
				}
			}
			else
			{
				itsTabGroup->SetTabTitle(i, w->GetTitle());
			}
		}
		else if (message.Is(JXWindow::kRaised))
		{
			itsTabGroup->ShowTab(i);
			(JXWidgetSet::GetWindow())->Raise();
		}
	}
	else if (sender == itsTabGroup && message.Is(JXTabGroup::kAppearanceChanged))
	{
		UpdateMinSize();
	}
	else if (sender == itsTabGroup->GetCardEnclosure() &&
			 message.Is(JXCardFile::kCardIndexChanged))
	{
		const auto* info =
			dynamic_cast<const JXCardFile::CardIndexChanged*>(&message);
		assert( info != nullptr );

		JIndex index;
		if (info->GetCardIndex(&index))
		{
			JXWindow* w = itsWindowList->GetElement(index);
			(w->GetDirector())->Activate();
		}
	}
	else
	{
		JXWidgetSet::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXDockWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!RemoveWindow(sender))
	{
		JXWidgetSet::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 FindWindow (private)

 ******************************************************************************/

bool
JXDockWidget::FindWindow
	(
	JBroadcaster*	sender,
	JXWindow**		window,
	JIndex*			index
	)
	const
{
	if (itsWindowList != nullptr)
	{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			JXWindow* w = itsWindowList->GetElement(i);
			if (w == sender)
			{
				*window = w;
				*index  = i;
				return true;
			}
		}
	}

	*window = nullptr;
	*index  = 0;
	return false;
}

/******************************************************************************
 RemoveWindow (private)

 ******************************************************************************/

bool
JXDockWidget::RemoveWindow
	(
	JBroadcaster* sender
	)
{
	if (itsWindowList != nullptr)
	{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			if (itsWindowList->GetElement(i) == sender)
			{
				itsTabGroup->DeleteTab(i);

				itsWindowList->RemoveElement(i);
				if (itsWindowList->IsEmpty())
				{
					jdelete itsWindowList;
					itsWindowList = nullptr;
				}

				UpdateMinSize();
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

bool
JXDockWidget::HasWindows()
	const
{
	if (itsChildPartition != nullptr && itsIsHorizFlag)
	{
		return dynamic_cast<JXVertDockPartition*>(itsChildPartition)->HasWindows();
	}
	else if (itsChildPartition != nullptr)
	{
		return dynamic_cast<JXHorizDockPartition*>(itsChildPartition)->HasWindows();
	}
	else
	{
		if (itsWindowList == nullptr || itsWindowList->IsEmpty())
		{
			return false;
		}

		for (auto* w : *itsWindowList)
		{
			if (w->IsVisible())
			{
				return true;
			}
		}

		return false;
	}
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

bool
JXDockWidget::CloseAllWindows()
{
	if (itsChildPartition != nullptr && itsIsHorizFlag)
	{
		return dynamic_cast<JXVertDockPartition*>(itsChildPartition)->CloseAllWindows();
	}
	else if (itsChildPartition != nullptr)
	{
		return dynamic_cast<JXHorizDockPartition*>(itsChildPartition)->CloseAllWindows();
	}
	else
	{
		return CloseAll();
	}
}

/******************************************************************************
 GetMinSize

 ******************************************************************************/

JPoint
JXDockWidget::GetMinSize()
	const
{
	JPoint minSize;
	if (itsChildPartition != nullptr && itsIsHorizFlag)
	{
		minSize = dynamic_cast<JXVertDockPartition*>(itsChildPartition)->UpdateMinSize();
	}
	else if (itsChildPartition != nullptr)
	{
		minSize = dynamic_cast<JXHorizDockPartition*>(itsChildPartition)->UpdateMinSize();
	}

	minSize.x = JMax(minSize.x, itsMinSize.x);
	minSize.y = JMax(minSize.y, itsMinSize.y);
	return minSize;
}

/******************************************************************************
 UpdateMinSize

	Only call this when itsWindowList changes.

 ******************************************************************************/

void
JXDockWidget::UpdateMinSize()
{
	itsMinSize.x = kDefaultMinSize;
	itsMinSize.y = kDefaultMinSize;

	if (itsWindowList != nullptr)
	{
		for (auto* w : *itsWindowList)
		{
			const JPoint pt = w->GetMinSize();
			itsMinSize.x    = JMax(pt.x, itsMinSize.x);
			itsMinSize.y    = JMax(pt.y, itsMinSize.y);
		}
	}

	const JRect r1 = itsTabGroup->GetApertureGlobal();
	const JRect r2 = this->GetApertureGlobal();
	itsMinSize.x  += (r2.left - r1.left) + (r1.right - r2.right);
	itsMinSize.y  += (r2.top - r1.top)   + (r1.bottom - r2.bottom);

	if (itsIsHorizFlag)
	{
		itsPartition->SetMinCompartmentSize(itsTabGroup->GetEnclosure(), itsMinSize.x);
	}
	else
	{
		itsPartition->SetMinCompartmentSize(itsTabGroup->GetEnclosure(), itsMinSize.y);
	}

	itsDirector->UpdateMinSize();
}
