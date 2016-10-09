/******************************************************************************
 JXHorizDockPartition.cpp

	Maintains a set of compartments arranged horizontally.  The user can dock
	a window in any compartment.

	BASE CLASS = JXHorizPartition

	Copyright (C) 2002-08 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXDockTabGroup.h>
#include <jGlobals.h>
#include <jAssert.h>

// Docking menu

static const JCharacter* kDockMenuStr =
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

// string ID's

static const JCharacter* kNoSpaceHorizID = "NoSpaceHoriz::JXHorizDockPartition";
static const JCharacter* kNoSpaceVertID  = "NoSpaceVert::JXHorizDockPartition";

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
	itsDockMenu(NULL),
	itsParentDock(parent)
{
	itsDockList = jnew JPtrArray<JXDockWidget>(JPtrArrayT::kForgetAll);
	assert( itsDockList != NULL );

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

	JXDockTabGroup* tabGroup =
		jnew JXDockTabGroup(compartment, kHElastic, kVElastic, 0,0, 100,100);
	assert( tabGroup != NULL );
	tabGroup->FitToEnclosure();

	JXDockWidget* dock =
		jnew JXDockWidget(itsDirector, this, kJTrue, tabGroup,
						 tabGroup->GetCardEnclosure(), kHElastic, kVElastic,
						 0,0, 100,100);
	assert( dock != NULL );
	dock->FitToEnclosure();
	itsDockList->InsertAtIndex(index, dock);

	tabGroup->SetDockWidget(dock);

	return compartment;
}

/******************************************************************************
 FindDock

 ******************************************************************************/

JBoolean
JXHorizDockPartition::FindDock
	(
	const JIndex	id,
	JXDockWidget**	dock
	)
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (d->GetID() == id)
			{
			*dock = d;
			return kJTrue;
			}

		JXVertDockPartition* p;
		if (d->GetVertChildPartition(&p) && p->FindDock(id, dock))
			{
			return kJTrue;
			}
		}

	*dock = NULL;
	return kJFalse;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

JBoolean
JXHorizDockPartition::HasWindows()
	const
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (d->HasWindows())
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

JBoolean
JXHorizDockPartition::CloseAllWindows()
{
	const JSize count = GetCompartmentCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* d = itsDockList->NthElement(i);
		assert( d != NULL );

		if (!d->CloseAllWindows())
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXHorizDockPartition::ReadSetup
	(
	istream&			input,
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
		JBoolean hasElasticIndex;
		JIndex elasticIndex;
		input >> hasElasticIndex >> elasticIndex;
		if (hasElasticIndex)
			{
			SetElasticIndex(elasticIndex);
			}
		}

	for (JIndex i=1; i<=compartmentCount; i++)
		{
		JXDockWidget* dock = itsDockList->NthElement(i);
		assert( dock != NULL );

		JIndex id;
		input >> id;
		dock->SetID(id);

		if (vers >= 2)
			{
			(dock->GetTabGroup())->ReadSetup(input);
			}

		JBoolean hasPartition;
		input >> hasPartition;
		if (hasPartition)
			{
			SplitVert(i);

			JXVertDockPartition* p;
			const JBoolean isSplit = dock->GetVertChildPartition(&p);
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
	ostream& output
	)
	const
{
	const JSize count = GetCompartmentCount();
	output << ' ' << count;

	WriteGeometry(output);

	JIndex elasticIndex;
	output << ' ' << GetElasticIndex(&elasticIndex);
	output << ' ' << elasticIndex;

	for (JIndex i=1; i<=count; i++)
		{
		JXDockWidget* dock = itsDockList->NthElement(i);
		assert( dock != NULL );

		output << ' ' << dock->GetID();

		(dock->GetTabGroup())->WriteSetup(output);

		JXVertDockPartition* v;
		if (dock->GetVertChildPartition(&v))
			{
			output << ' ' << kJTrue;
			v->WriteSetup(output);
			}
		else
			{
			output << ' ' << kJFalse;
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

		if (itsDockMenu == NULL)
			{
			itsDockMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
			assert( itsDockMenu != NULL );
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
	else if (sender == itsDockMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
	const JBoolean canRemove =
		JI2B(GetEnclosure() != GetWindow() || GetCompartmentCount() > 2);

	itsDockMenu->SetItemEnable(kRemoveLeftCmd,  canRemove);
	itsDockMenu->SetItemEnable(kRemoveRightCmd, canRemove);

	JIndex i;
	if (GetElasticIndex(&i))
		{
		itsDockMenu->SetItemEnable(kSetLeftElasticCmd,  JI2B( i != itsCompartmentIndex ));
		itsDockMenu->SetItemEnable(kSetRightElasticCmd, JI2B( i != itsCompartmentIndex+1 ));
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
		InsertCompartment(itsCompartmentIndex+1, kJTrue);
		}
	else if (index == kSplitLeftVertCmd)
		{
		SplitVert(itsCompartmentIndex, NULL, kJTrue);
		}
	else if (index == kRemoveLeftCmd)
		{
		DeleteCompartment(itsCompartmentIndex);		// may jdelete us
		}

	else if (index == kSplitRightHorizCmd)
		{
		InsertCompartment(itsCompartmentIndex+1, kJTrue);
		}
	else if (index == kSplitRightVertCmd)
		{
		SplitVert(itsCompartmentIndex+1, NULL, kJTrue);
		}
	else if (index == kRemoveRightCmd)
		{
		DeleteCompartment(itsCompartmentIndex+1);	// may jdelete us
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
	const JBoolean	reportError
	)
{
	if (JPartition::InsertCompartment(index, 10, 10))
		{
		itsDirector->UpdateMinSize();
		}
	else if (reportError)
		{
		(JGetUserNotification())->ReportError(JGetString(kNoSpaceHorizID));
		}
}

/******************************************************************************
 SplitVert

	returnPartition can be NULL.

 ******************************************************************************/

JBoolean
JXHorizDockPartition::SplitVert
	(
	const JIndex			index,
	JXVertDockPartition**	returnPartition,
	const JBoolean			reportError
	)
{
	if (returnPartition != NULL)
		{
		*returnPartition = NULL;
		}

	JXDockWidget* parent = itsDockList->NthElement(index);
	assert( parent != NULL );

	JXVertDockPartition* p = NULL;
	if (parent->GetVertChildPartition(&p))
		{
		p->InsertCompartment(p->GetCompartmentCount()+1, reportError);
		}
	else
		{
		JXContainer* encl = GetCompartment(index);
		assert( encl != NULL );

		const JCoordinate h = encl->GetApertureHeight() - kDragRegionSize;
		if (h < 2*JXDockWidget::kDefaultMinSize)
			{
			return kJFalse;
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
		assert( p != NULL );

		JXDockWidget* d1 = p->GetDockWidget(1);
		assert( d1 != NULL );

		if (parent->TransferAll(d1))
			{
			parent->SetChildPartition(p);
			}
		else
			{
			d1->TransferAll(parent);
			jdelete p;
			p = NULL;

			if (reportError)
				{
				(JGetUserNotification())->ReportError(JGetString(kNoSpaceVertID));
				}
			}
		}

	if (returnPartition != NULL)
		{
		*returnPartition = p;
		}
	return JI2B( p != NULL );
}

/******************************************************************************
 DeleteCompartment

	*** This function can jdelete us!

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
		assert( itsParentDock != NULL );

		JXDockWidget* child = itsDockList->FirstElement();
		assert( child != NULL );

		itsParentDock->SetChildPartition(NULL);		// so docking will be allowed
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
		const JPoint minSize = (itsDockList->NthElement(i))->GetMinSize();
		minWidth            += minSize.x;
		minHeight            = JMax(minSize.y, minHeight);

		JPartition::SetMinCompartmentSize(i, minSize.x);
		}

	return JPoint(minWidth, minHeight);
}
