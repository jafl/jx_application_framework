/******************************************************************************
 JXDockWidget.cpp

	Maintains a set of docked JXWindows.

	BASE CLASS = JXWidgetSet

	Copyright (C) 2002-08 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXDockWidget.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockDragData.h>
#include <JXUpdateMinSizeTask.h>
#include <JXFileDocument.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXPartition.h>
#include <JXTabGroup.h>
#include <JXHintDirector.h>
#include <JXTimerTask.h>
#include <JXWindowPainter.h>
#include <JXGC.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <jAssert.h>

const Time kDeleteHintDelay = 10000;	// 10 sec (ms)

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockWidget::JXDockWidget
	(
	JXDockDirector*		director,
	JXPartition*		partition,
	const JBoolean		isHorizPartition,
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
	itsWindowList(NULL),
	itsMinSize(kDefaultMinSize, kDefaultMinSize),
	itsChildPartition(NULL),
	itsHintDirector(NULL),
	itsDeleteHintTask(NULL)
{
	assert( itsDirector != NULL );
	assert( itsPartition != NULL );
	assert( itsTabGroup != NULL );

	if (!(JXGetDockManager())->IsReadingSetup())
		{
		(JXGetDockManager())->IDUsed(itsID);
		}

	ListenTo(itsTabGroup);
	ListenTo(itsTabGroup->GetCardEnclosure());

	JXUpdateMinSizeTask* task = jnew JXUpdateMinSizeTask(this);
	assert( task != NULL );
	task->Go();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockWidget::~JXDockWidget()
{
	const JXDockManager::CloseDockMode mode = (JXGetDockManager())->GetCloseDockMode();
	if (itsWindowList != NULL && mode == JXDockManager::kUndockWindows)
		{
		// can't call UndockAll() because that calls UpdateMinSize()

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);
			w->Undock();
			}

		jdelete itsWindowList;
		}
	else if (itsWindowList != NULL)
		{
		assert( mode == JXDockManager::kCloseWindows );

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
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

JBoolean
JXDockWidget::WindowWillFit
	(
	JXWindow* w
	)
	const
{
	const JPoint minSize = w->GetMinSize();
	return JI2B(GetApertureWidth()  >= minSize.x &&
				GetApertureHeight() >= minSize.y);
}

/******************************************************************************
 Dock

 ******************************************************************************/

JBoolean
JXDockWidget::Dock
	(
	JXWindowDirector*	d,
	const JBoolean		reportError
	)
{
	return Dock(d->GetWindow(), reportError);
}

JBoolean
JXDockWidget::Dock
	(
	JXWindow*		w,
	const JBoolean	reportError
	)
{
	if (itsChildPartition != NULL)
		{
		return kJFalse;
		}

	const JRect geom = GetApertureGlobal();
	if (w->Dock(this, (JXWidgetSet::GetWindow())->GetXWindow(), geom))
		{
		if (itsWindowList == NULL)
			{
			itsWindowList = jnew JPtrArray<JXWindow>(JPtrArrayT::kForgetAll);
			assert( itsWindowList != NULL );
			}

		const JIndex index = GetTabInsertionIndex(w);

		itsWindowList->InsertAtIndex(index, w);
		ListenTo(w);
		UpdateMinSize();

		itsTabGroup->InsertTab(index, w->GetTitle());
		// tab will be selected when window shows itself

		return kJTrue;
		}
	else
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString("WindowWillNotFit::JXDockWidget"));
			}
		return kJFalse;
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

	const JCharacter* title =
		JXFileDocument::SkipNeedsSavePrefix(w->GetTitle());

	JIndex index = count+1;
	for (JIndex i=1; i<=count; i++)
		{
		if (i == ignoreIndex)
			{
			continue;
			}

		const JCharacter* t =
			JXFileDocument::SkipNeedsSavePrefix((itsWindowList->NthElement(i))->GetTitle());
		if (JStringCompare(title, t, kJFalse) < 0)
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
	Returns kJFalse if not all windows could be transferred.

 ******************************************************************************/

JBoolean
JXDockWidget::TransferAll
	(
	JXDockWidget* target
	)
{
	JBoolean success = kJTrue;

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			if (!target->Dock(w))
				{
				success = kJFalse;
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
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsTabGroup->DeleteTab(1);

			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);
			w->Undock();
			}

		itsWindowList->RemoveAll();
		jdelete itsWindowList;
		itsWindowList = NULL;

		UpdateMinSize();
		}
}

/******************************************************************************
 CloseAll

 ******************************************************************************/

JBoolean
JXDockWidget::CloseAll()
{
	if (itsWindowList != NULL)
		{
		JXDisplay* display = GetDisplay();
		Display* xDisplay  = display->GetXDisplay();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);

			Window xWindow = w->GetXWindow();
			if (w->Close())
				{
				if (JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
					w->Undock();
					}
				else if ((JXGetApplication())->DisplayExists(xDisplay))
					{
					itsTabGroup->DeleteTab(i);
					itsWindowList->RemoveElement(i);
					}
				else
					{
					return kJFalse;
					}
				}
			else
				{
				UpdateMinSize();
				return kJFalse;
				}
			}

		jdelete itsWindowList;
		itsWindowList = NULL;

		UpdateMinSize();
		}

	return kJTrue;
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

JBoolean
JXDockWidget::GetHorizChildPartition
	(
	JXHorizDockPartition** p
	)
	const
{
	if (!itsIsHorizFlag && itsChildPartition != NULL)
		{
		*p = dynamic_cast<JXHorizDockPartition*>(itsChildPartition);
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 GetVertChildPartition

 ******************************************************************************/

JBoolean
JXDockWidget::GetVertChildPartition
	(
	JXVertDockPartition** p
	)
	const
{
	if (itsIsHorizFlag && itsChildPartition != NULL)
		{
		*p = dynamic_cast<JXVertDockPartition*>(itsChildPartition);
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
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
	assert( itsWindowList == NULL );

	if (itsChildPartition != NULL)
		{
		StopListening(itsChildPartition);
		}

	itsChildPartition = p;

	if (itsChildPartition != NULL)
		{
		ClearWhenGoingAway(itsChildPartition, &itsChildPartition);
		}

	UpdateMinSize();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept JXDockDragData.

 ******************************************************************************/

JBoolean
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

	if (source == NULL)
		{
		return kJFalse;
		}

	// we accept drops of type JXDockDragData

	const Atom minSizeAtom = (JXGetDockManager())->GetDNDMinSizeAtom();

	const JSize typeCount = typeList.GetElementCount();
	JBoolean acceptDrop   = kJFalse;
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == minSizeAtom)
			{
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
					XPoint* minSize = (XPoint*) data;
					if (GetApertureWidth()  >= minSize->x &&
						GetApertureHeight() >= minSize->y)
						{
						acceptDrop = kJTrue;
						}
					}
				selMgr->DeleteData(&data, delMethod);
				}

			break;
			}
		}

	if (!acceptDrop && itsHintDirector == NULL)
		{
		JRect r = GetBounds();
		r.Shrink(r.width()/2 - 5, r.height()/2 - 5);

		itsHintDirector =
			jnew JXHintDirector((JXContainer::GetWindow())->GetDirector(), this,
							   r, JGetString("WindowWillNotFit::JXDockWidget"));
		assert( itsHintDirector != NULL );
		itsHintDirector->Activate();

		itsDeleteHintTask = jnew JXTimerTask(kDeleteHintDelay, kJTrue);
		assert( itsDeleteHintTask != NULL );
		itsDeleteHintTask->Start();
		ListenTo(itsDeleteHintTask);
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
			Dock(window, kJTrue);
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
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->Refresh();
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

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->UndockedMove(dx, dy);
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

	if (itsWindowList != NULL)
		{
		// Use SetSize() instead of AdjustSize() because JXWindow might
		// have max size, in which case deltas will be wrong.

		const JRect boundsG = GetBoundsGlobal();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->UndockedSetSize(boundsG.width(), boundsG.height());
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
				const JBoolean hadSelection = itsTabGroup->GetCurrentTabIndex(&currIndex);

				JXWidgetSet* card = dynamic_cast<JXWidgetSet*>(itsTabGroup->RemoveTab(i));
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
		const JXCardFile::CardIndexChanged* info =
			dynamic_cast<const JXCardFile::CardIndexChanged*>(&message);
		assert( info != NULL );

		JIndex index;
		if (info->GetCardIndex(&index))
			{
			JXWindow* w = itsWindowList->NthElement(index);
			(w->GetDirector())->Activate();
			}
		}
	else if (sender == itsDeleteHintTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		itsDeleteHintTask = NULL;
		itsHintDirector->Close();
		itsHintDirector = NULL;
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

JBoolean
JXDockWidget::FindWindow
	(
	JBroadcaster*	sender,
	JXWindow**		window,
	JIndex*			index
	)
	const
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			if (w == sender)
				{
				*window = w;
				*index  = i;
				return kJTrue;
				}
			}
		}

	*window = NULL;
	*index  = 0;
	return kJFalse;
}

/******************************************************************************
 RemoveWindow (private)

 ******************************************************************************/

JBoolean
JXDockWidget::RemoveWindow
	(
	JBroadcaster* sender
	)
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (itsWindowList->NthElement(i) == sender)
				{
				itsTabGroup->DeleteTab(i);

				itsWindowList->RemoveElement(i);
				if (itsWindowList->IsEmpty())
					{
					jdelete itsWindowList;
					itsWindowList = NULL;
					}

				UpdateMinSize();
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

JBoolean
JXDockWidget::HasWindows()
	const
{
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		return dynamic_cast<JXVertDockPartition*>(itsChildPartition)->HasWindows();
		}
	else if (itsChildPartition != NULL)
		{
		return dynamic_cast<JXHorizDockPartition*>(itsChildPartition)->HasWindows();
		}
	else
		{
		if (itsWindowList == NULL || itsWindowList->IsEmpty())
			{
			return kJFalse;
			}

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((itsWindowList->NthElement(i))->IsVisible())
				{
				return kJTrue;
				}
			}

		return kJFalse;
		}
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

JBoolean
JXDockWidget::CloseAllWindows()
{
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		return dynamic_cast<JXVertDockPartition*>(itsChildPartition)->CloseAllWindows();
		}
	else if (itsChildPartition != NULL)
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
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		minSize = dynamic_cast<JXVertDockPartition*>(itsChildPartition)->UpdateMinSize();
		}
	else if (itsChildPartition != NULL)
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

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JPoint pt = (itsWindowList->NthElement(i))->GetMinSize();
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
