/******************************************************************************
 JXHorizDockPartition.cpp

	Maintains a set of compartments arranged horizontally.  The user can dock
	a window in any compartment.

	BASE CLASS = JXHorizPartition

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#include "JXHorizDockPartition.h"
#include "JXVertDockPartition.h"
#include "JXDockDirector.h"
#include "JXDockWidget.h"
#include "JXWindow.h"
#include "JXTextMenu.h"
#include "JXDockTabGroup.h"
#include <jx-af/jcore/jGlobals.h>
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

// Docking menu

static const JUtf8Byte* kDockMenuStr =
	"    Split left compartment horizontally"
	"  | Split left compartment vertically"
	"  | Remove left compartment"
	"%l| Split right compartment horizontally"
	"  | Split right compartment vertically"
	"  | Remove right compartment"
	"%l| Make left compartment elastic"
	"  | Make right compartment elastic"
	"  | Make all compartments elastic";

enum
{
	kSplitLeftHorizCmd = 1,
	kSplitLeftVertCmd,
	kRemoveLeftCmd,
	kSplitRightHorizCmd,
	kSplitRightVertCmd,
	kRemoveRightCmd,
	kSetLeftElasticCmd,
	kSetRightElasticCmd,
	kSetAllElasticCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHorizDockPartition::JXHorizDockPartition
	(
	JXDockDirector*				director,
	JXDockWidget*				parent,
	const JArray<JCoordinate>&	widths,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minWidths,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXHorizPartition(enclosure, widths, elasticIndex, minWidths,
					 hSizing, vSizing, x,y, w,h),
	itsDirector(director),
	itsDockMenu(nullptr),
	itsParentDock(parent)
{
	itsDockList = jnew JPtrArray<JXDockWidget>(JPtrArrayT::kForgetAll);
	assert( itsDockList != nullptr );

	CreateInitialCompartments();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHorizDockPartition::~JXHorizDockPartition()
{
	jdelete itsDockList;
}

/******************************************************************************
 CreateCompartment (virtual protected)

 ******************************************************************************/

JXWidgetSet*
JXHorizDockPartition::CreateCompartment
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	JXWidgetSet* compartment =
		JXHorizPartition::CreateCompartment(index, position, size);

	auto* tabGroup =
		jnew JXDockTabGroup(compartment, kHElastic, kVElastic, 0,0, 100,100);
	assert( tabGroup != nullptr );
	tabGroup->FitToEnclosure();

	auto* dock =
		jnew JXDockWidget(itsDirector, this, true, tabGroup,
						 tabGroup->GetCardEnclosure(), kHElastic, kVElastic,
						 0,0, 100,100);
	assert( dock != nullptr );
	dock->FitToEnclosure();
	itsDockList->InsertAtIndex(index, dock);

	tabGroup->SetDockWidget(dock);

	return compartment;
}

/******************************************************************************
 FindDock

 ******************************************************************************/

bool
JXHorizDockPartition::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	for (auto* d : *itsDockList)
	{
		assert( d != nullptr );

		if (d->GetID() == id)
		{
			*dock = d;
			return true;
		}

		JXVertDockPartition* p;
		if (d->GetVertChildPartition(&p) && p->FindDock(id, dock))
		{
			return true;
		}
	}

	*dock = nullptr;
	return false;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

bool
JXHorizDockPartition::HasWindows()
	const
{
	return std::any_of(begin(*itsDockList), end(*itsDockList),
			[] (JXDockWidget* d) { return d->HasWindows(); });
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

bool
JXHorizDockPartition::CloseAllWindows()
{
	for (auto* d : *itsDockList)
	{
		assert( d != nullptr );

		if (!d->CloseAllWindows())
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXHorizDockPartition::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	JSize compartmentCount;
	input >> compartmentCount;

	while (GetCompartmentCount() < compartmentCount)
	{
		InsertCompartment(1);
	}
	while (GetCompartmentCount() > compartmentCount)
	{
		DeleteCompartment(1);
	}

	ReadGeometry(input);

	if (vers >= 3)
	{
		bool hasElasticIndex;
		JIndex elasticIndex;
		input >> JBoolFromString(hasElasticIndex) >> elasticIndex;
		if (hasElasticIndex)
		{
			SetElasticIndex(elasticIndex);
		}
	}

	for (JIndex i=1; i<=compartmentCount; i++)
	{
		JXDockWidget* dock = itsDockList->GetElement(i);
		assert( dock != nullptr );

		JIndex id;
		input >> id;
		dock->SetID(id);

		if (vers >= 2)
		{
			(dock->GetTabGroup())->ReadSetup(input);
		}

		bool hasPartition;
		input >> JBoolFromString(hasPartition);
		if (hasPartition)
		{
			SplitVert(i);

			JXVertDockPartition* p;
			const bool isSplit = dock->GetVertChildPartition(&p);
			assert( isSplit );
			p->ReadSetup(input, vers);
		}
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXHorizDockPartition::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << GetCompartmentCount();

	WriteGeometry(output);

	JIndex elasticIndex;
	output << ' ' << JBoolToString(GetElasticIndex(&elasticIndex));
	output << ' ' << elasticIndex;

	for (auto* dock : *itsDockList)
	{
		assert( dock != nullptr );

		output << ' ' << dock->GetID();

		(dock->GetTabGroup())->WriteSetup(output);

		JXVertDockPartition* v;
		if (dock->GetVertChildPartition(&v))
		{
			output << ' ' << JBoolToString(true);
			v->WriteSetup(output);
		}
		else
		{
			output << ' ' << JBoolToString(false);
		}
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXHorizDockPartition::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXRightButton)
	{
		if (!FindCompartment(pt.x, &itsCompartmentIndex) ||
			itsCompartmentIndex >= GetCompartmentCount())
		{
			return;
		}

		if (itsDockMenu == nullptr)
		{
			itsDockMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
			assert( itsDockMenu != nullptr );
			itsDockMenu->SetToHiddenPopupMenu();
			itsDockMenu->SetMenuItems(kDockMenuStr);
			itsDockMenu->SetUpdateAction(JXMenu::kDisableNone);
			ListenTo(itsDockMenu);
		}

		itsDockMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else
	{
		JXHorizPartition::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXHorizDockPartition::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDockMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateDockMenu();
	}
	else if (sender == itsDockMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDockMenu(selection->GetIndex());
	}

	else
	{
		JXHorizPartition::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateDockMenu (private)

 ******************************************************************************/

void
JXHorizDockPartition::UpdateDockMenu()
{
	const bool canRemove =
		GetEnclosure() != GetWindow() || GetCompartmentCount() > 2;

	itsDockMenu->SetItemEnabled(kRemoveLeftCmd,  canRemove);
	itsDockMenu->SetItemEnabled(kRemoveRightCmd, canRemove);

	JIndex i;
	if (GetElasticIndex(&i))
	{
		itsDockMenu->SetItemEnabled(kSetLeftElasticCmd,  i != itsCompartmentIndex );
		itsDockMenu->SetItemEnabled(kSetRightElasticCmd, i != itsCompartmentIndex+1 );
		itsDockMenu->EnableItem(kSetAllElasticCmd);
	}
	else
	{
		itsDockMenu->EnableItem(kSetLeftElasticCmd);
		itsDockMenu->EnableItem(kSetRightElasticCmd);
		itsDockMenu->DisableItem(kSetAllElasticCmd);
	}
}

/******************************************************************************
 HandleDockMenu (private)

 ******************************************************************************/

void
JXHorizDockPartition::HandleDockMenu
	(
	const JIndex index
	)
{
	if (itsCompartmentIndex == 0 || itsCompartmentIndex >= GetCompartmentCount())
	{
		return;
	}

	if (index == kSplitLeftHorizCmd)
	{
		InsertCompartment(itsCompartmentIndex+1, true);
	}
	else if (index == kSplitLeftVertCmd)
	{
		SplitVert(itsCompartmentIndex, nullptr, true);
	}
	else if (index == kRemoveLeftCmd)
	{
		DeleteCompartment(itsCompartmentIndex);		// may delete us
	}

	else if (index == kSplitRightHorizCmd)
	{
		InsertCompartment(itsCompartmentIndex+1, true);
	}
	else if (index == kSplitRightVertCmd)
	{
		SplitVert(itsCompartmentIndex+1, nullptr, true);
	}
	else if (index == kRemoveRightCmd)
	{
		DeleteCompartment(itsCompartmentIndex+1);	// may delete us
	}

	else if (index == kSetLeftElasticCmd)
	{
		SetElasticIndex(itsCompartmentIndex);
	}
	else if (index == kSetRightElasticCmd)
	{
		SetElasticIndex(itsCompartmentIndex+1);
	}
	else if (index == kSetAllElasticCmd)
	{
		SetElasticIndex(0);
	}
}

/******************************************************************************
 InsertCompartment

 ******************************************************************************/

void
JXHorizDockPartition::InsertCompartment
	(
	const JIndex	index,
	const bool	reportError
	)
{
	if (JPartition::InsertCompartment(index, 10, 10))
	{
		itsDirector->UpdateMinSize();
	}
	else if (reportError)
	{
		JGetUserNotification()->ReportError(JGetString("NoSpaceHoriz::JXHorizDockPartition"));
	}
}

/******************************************************************************
 SplitVert

	returnPartition can be nullptr.

 ******************************************************************************/

bool
JXHorizDockPartition::SplitVert
	(
	const JIndex			index,
	JXVertDockPartition**	returnPartition,
	const bool			reportError
	)
{
	if (returnPartition != nullptr)
	{
		*returnPartition = nullptr;
	}

	JXDockWidget* parent = itsDockList->GetElement(index);
	assert( parent != nullptr );

	JXVertDockPartition* p = nullptr;
	if (parent->GetVertChildPartition(&p))
	{
		p->InsertCompartment(p->GetCompartmentCount()+1, reportError);
	}
	else
	{
		JXContainer* encl = GetCompartment(index);
		assert( encl != nullptr );

		const JCoordinate h = encl->GetApertureHeight() - kDragRegionSize;
		if (h < 2*JXDockWidget::kDefaultMinSize)
		{
			return false;
		}

		JArray<JCoordinate> heights;
		heights.AppendElement(h/2);
		heights.AppendElement(h/2 + h%2);

		JArray<JCoordinate> minHeights;
		minHeights.AppendElement(JXDockWidget::kDefaultMinSize);
		minHeights.AppendElement(JXDockWidget::kDefaultMinSize);

		p = jnew JXVertDockPartition(itsDirector, parent, heights, 0, minHeights, encl,
									kHElastic, kVElastic, 0,0,
									encl->GetApertureWidth(), encl->GetApertureHeight());
		assert( p != nullptr );

		JXDockWidget* d1 = p->GetDockWidget(1);
		assert( d1 != nullptr );

		if (parent->TransferAll(d1))
		{
			parent->SetChildPartition(p);
		}
		else
		{
			d1->TransferAll(parent);
			jdelete p;
			p = nullptr;

			if (reportError)
			{
				JGetUserNotification()->ReportError(JGetString("NoSpaceVert::JXHorizDockPartition"));
			}
		}
	}

	if (returnPartition != nullptr)
	{
		*returnPartition = p;
	}
	return p != nullptr;
}

/******************************************************************************
 DeleteCompartment

	*** This function can delete us!

 ******************************************************************************/

void
JXHorizDockPartition::DeleteCompartment
	(
	const JIndex index
	)
{
	if (GetEnclosure() == GetWindow() && GetCompartmentCount() <= 2)
	{
		return;
	}

	itsDockList->RemoveElement(index);
	JPartition::DeleteCompartment(index);

	if (GetCompartmentCount() == 1)
	{
		assert( itsParentDock != nullptr );

		JXDockWidget* child = itsDockList->GetFirstElement();
		assert( child != nullptr );

		itsParentDock->SetChildPartition(nullptr);		// so docking will be allowed
		child->TransferAll(itsParentDock);
		jdelete this;
	}
}

/******************************************************************************
 UpdateMinSize

 ******************************************************************************/

JPoint
JXHorizDockPartition::UpdateMinSize()
{
	JCoordinate minWidth  = kDragRegionSize * (GetCompartmentCount()-1);
	JCoordinate minHeight = JXDockWidget::kDefaultMinSize;

	const JSize count = itsDockList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JPoint minSize = (itsDockList->GetElement(i))->GetMinSize();
		minWidth            += minSize.x;
		minHeight            = JMax(minSize.y, minHeight);

		JPartition::SetMinCompartmentSize(i, minSize.x);
	}

	return JPoint(minWidth, minHeight);
}

/******************************************************************************
 SaveGeometryForLater (virtual protected)

 ******************************************************************************/

bool
JXHorizDockPartition::SaveGeometryForLater
	(
	const JArray<JCoordinate>& sizes
	)
{
	return false;
}
