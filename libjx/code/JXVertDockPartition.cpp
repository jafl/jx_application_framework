/******************************************************************************
 JXVertDockPartition.cpp

	Maintains a set of compartments arranged vertically.  The user can dock
	a window in any compartment.

	BASE CLASS = JXVertPartition

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXVertDockPartition.h"
#include "jx-af/jx/JXHorizDockPartition.h"
#include "jx-af/jx/JXDockDirector.h"
#include "jx-af/jx/JXDockWidget.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextMenu.h"
#include "jx-af/jx/JXDockTabGroup.h"
#include <jx-af/jcore/jGlobals.h>
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

// Docking menu

static const JUtf8Byte* kDockMenuStr =
	"    Split top compartment horizontally"
	"  | Split top compartment vertically"
	"  | Remove top compartment"
	"%l| Split bottom compartment horizontally"
	"  | Split bottom compartment vertically"
	"  | Remove bottom compartment"
	"%l| Make top compartment elastic"
	"  | Make bottom compartment elastic"
	"  | Make all compartments elastic";

enum
{
	kSplitTopHorizCmd = 1,
	kSplitTopVertCmd,
	kRemoveTopCmd,
	kSplitBottomHorizCmd,
	kSplitBottomVertCmd,
	kRemoveBottomCmd,
	kSetTopElasticCmd,
	kSetBottomElasticCmd,
	kSetAllElasticCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVertDockPartition::JXVertDockPartition
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
	JXVertPartition(enclosure, widths, elasticIndex, minWidths,
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

JXVertDockPartition::~JXVertDockPartition()
{
	jdelete itsDockList;
}

/******************************************************************************
 CreateCompartment (virtual protected)

 ******************************************************************************/

JXWidgetSet*
JXVertDockPartition::CreateCompartment
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	JXWidgetSet* compartment =
		JXVertPartition::CreateCompartment(index, position, size);

	auto* tabGroup =
		jnew JXDockTabGroup(compartment, kHElastic, kVElastic, 0,0, 100,100);
	assert( tabGroup != nullptr );
	tabGroup->FitToEnclosure();

	auto* dock =
		jnew JXDockWidget(itsDirector, this, false, tabGroup,
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
JXVertDockPartition::FindDock
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

		JXHorizDockPartition* p;
		if (d->GetHorizChildPartition(&p) && p->FindDock(id, dock))
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
JXVertDockPartition::HasWindows()
	const
{
	return std::any_of(begin(*itsDockList), end(*itsDockList),
			[] (JXDockWidget* d) { return d->HasWindows(); });
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

bool
JXVertDockPartition::CloseAllWindows()
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
JXVertDockPartition::ReadSetup
	(
	std::istream&			input,
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
			SplitHoriz(i);

			JXHorizDockPartition* p;
			const bool isSplit = dock->GetHorizChildPartition(&p);
			assert( isSplit );
			p->ReadSetup(input, vers);
		}
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXVertDockPartition::WriteSetup
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

		JXHorizDockPartition* v;
		if (dock->GetHorizChildPartition(&v))
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
JXVertDockPartition::HandleMouseDown
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
		if (!FindCompartment(pt.y, &itsCompartmentIndex) ||
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
		JXVertPartition::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXVertDockPartition::Receive
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
		JXVertPartition::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateDockMenu (private)

 ******************************************************************************/

void
JXVertDockPartition::UpdateDockMenu()
{
	const bool canRemove =
		GetEnclosure() != GetWindow() || GetCompartmentCount() > 2;

	itsDockMenu->SetItemEnabled(kRemoveTopCmd,    canRemove);
	itsDockMenu->SetItemEnabled(kRemoveBottomCmd, canRemove);

	JIndex i;
	if (GetElasticIndex(&i))
	{
		itsDockMenu->SetItemEnabled(kSetTopElasticCmd,  i != itsCompartmentIndex );
		itsDockMenu->SetItemEnabled(kSetBottomElasticCmd, i != itsCompartmentIndex+1 );
		itsDockMenu->EnableItem(kSetAllElasticCmd);
	}
	else
	{
		itsDockMenu->EnableItem(kSetTopElasticCmd);
		itsDockMenu->EnableItem(kSetBottomElasticCmd);
		itsDockMenu->DisableItem(kSetAllElasticCmd);
	}
}

/******************************************************************************
 HandleDockMenu (private)

 ******************************************************************************/

void
JXVertDockPartition::HandleDockMenu
	(
	const JIndex index
	)
{
	if (itsCompartmentIndex == 0 || itsCompartmentIndex >= GetCompartmentCount())
	{
		return;
	}

	if (index == kSplitTopHorizCmd)
	{
		SplitHoriz(itsCompartmentIndex, nullptr, true);
	}
	else if (index == kSplitTopVertCmd)
	{
		InsertCompartment(itsCompartmentIndex+1, true);
	}
	else if (index == kRemoveTopCmd)
	{
		DeleteCompartment(itsCompartmentIndex);		// may delete us
	}

	else if (index == kSplitBottomHorizCmd)
	{
		SplitHoriz(itsCompartmentIndex+1, nullptr, true);
	}
	else if (index == kSplitBottomVertCmd)
	{
		InsertCompartment(itsCompartmentIndex+1, true);
	}
	else if (index == kRemoveBottomCmd)
	{
		DeleteCompartment(itsCompartmentIndex+1);	// may delete us
	}

	else if (index == kSetTopElasticCmd)
	{
		SetElasticIndex(itsCompartmentIndex);
	}
	else if (index == kSetBottomElasticCmd)
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
JXVertDockPartition::InsertCompartment
	(
	const JIndex	index,
	const bool	reportError
	)
{
	if (JPartition::InsertCompartment(index, 10, 10))
	{
		itsDirector->UpdateMinSize();
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("NoSpaceVert::JXVertDockPartition"));
	}
}

/******************************************************************************
 SplitHoriz

	returnPartition can be nullptr.

 ******************************************************************************/

bool
JXVertDockPartition::SplitHoriz
	(
	const JIndex			index,
	JXHorizDockPartition**	returnPartition,
	const bool			reportError
	)
{
	if (returnPartition != nullptr)
	{
		*returnPartition = nullptr;
	}

	JXDockWidget* parent = itsDockList->GetElement(index);
	assert( parent != nullptr );

	JXHorizDockPartition* p = nullptr;
	if (parent->GetHorizChildPartition(&p))
	{
		p->InsertCompartment(p->GetCompartmentCount()+1, reportError);
	}
	else
	{
		JXContainer* encl = GetCompartment(index);
		assert( encl != nullptr );

		const JCoordinate w = encl->GetApertureWidth() - kDragRegionSize;
		if (w < 2*JXDockWidget::kDefaultMinSize)
		{
			return false;
		}

		JArray<JCoordinate> widths;
		widths.AppendElement(w/2);
		widths.AppendElement(w/2 + w%2);

		JArray<JCoordinate> minWidths;
		minWidths.AppendElement(JXDockWidget::kDefaultMinSize);
		minWidths.AppendElement(JXDockWidget::kDefaultMinSize);

		p = jnew JXHorizDockPartition(itsDirector, parent, widths, 0, minWidths, encl,
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
				JGetUserNotification()->ReportError(JGetString("NoSpaceHoriz::JXVertDockPartition"));
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
JXVertDockPartition::DeleteCompartment
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
JXVertDockPartition::UpdateMinSize()
{
	JCoordinate minWidth  = JXDockWidget::kDefaultMinSize;
	JCoordinate minHeight = kDragRegionSize * (GetCompartmentCount()-1);

	const JSize count = itsDockList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JPoint minSize = (itsDockList->GetElement(i))->GetMinSize();
		minWidth             = JMax(minSize.x, minWidth);
		minHeight           += minSize.y;

		JPartition::SetMinCompartmentSize(i, minSize.y);
	}

	return JPoint(minWidth, minHeight);
}

/******************************************************************************
 SaveGeometryForLater (virtual protected)

 ******************************************************************************/

bool
JXVertDockPartition::SaveGeometryForLater
	(
	const JArray<JCoordinate>& sizes
	)
{
	return false;
}
