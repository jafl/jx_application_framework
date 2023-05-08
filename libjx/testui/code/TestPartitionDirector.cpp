/******************************************************************************
 TestPartitionDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestPartitionDirector.h"
#include "TestCompartment.h"
#include "SetElasticDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JSize kInitCompartmentCount = 3;
const JIndex kInitElasticIndex    = 2;
const JSize kInitSize             = 100;
const JSize kMinSize              = 50;
const JSize kMenuBarHeight        = 30;

// Horiz menu information

static const JUtf8Byte* kHorizMenuStr =
	"Prepend compartment %h p | Append compartment %h a"
	"%l | Set elastic compartment %h s";

enum
{
	kPrependHorizCmd = 1,
	kAppendHorizCmd,
	kSetHorizElasticIndexCmd
};

// Vert menu information

static const JUtf8Byte* kVertMenuStr =
	"Set elastic compartment %h s";

enum
{
	kSetVertElasticIndexCmd = 1
};

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPartitionDirector::TestPartitionDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPartitionDirector::~TestPartitionDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestPartitionDirector::BuildWindow()
{
	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;

	for (JIndex i=1; i<=kInitCompartmentCount; i++)
	{
		sizes.AppendElement(kInitSize);
		minSizes.AppendElement(kMinSize);
	}

// begin JXLayout

	auto* window = jnew JXWindow(this, 310,340, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 310,30);
	assert( menuBar != nullptr );

	itsHorizPartition =
		jnew JXHorizPartition(sizes, kInitElasticIndex, minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 310,310);
	assert( itsHorizPartition != nullptr );

// end JXLayout

	itsVertPartition =
		jnew JXVertPartition(sizes, kInitElasticIndex, minSizes,
					itsHorizPartition->GetCompartment(kInitElasticIndex),
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,0, 10,315);
	assert( itsVertPartition != nullptr );
	itsVertPartition->FitToEnclosure(true, true);

	window->SetTitle(JGetString("WindowTitle::TestPartitionDirector"));
	window->SetWMClass("testjx", "TestPartitionDirector");
	AdjustMinWindowSize();

	itsHorizMenu = menuBar->AppendTextMenu(JGetString("HorizMenuTitle::TestPartitionDirector"));
	itsHorizMenu->SetShortcuts(JGetString("HorizMenuShortcut::TestPartitionDirector"));
	itsHorizMenu->SetMenuItems(kHorizMenuStr);
	itsHorizMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsHorizMenu->AttachHandlers(this,
		&TestPartitionDirector::UpdateHorizMenu,
		&TestPartitionDirector::HandleHorizMenu);

	itsVertMenu = menuBar->AppendTextMenu(JGetString("VertMenuTitle::TestPartitionDirector"));
	itsVertMenu->SetShortcuts(JGetString("VertMenuShortcut::TestPartitionDirector"));
	itsVertMenu->SetMenuItems(kVertMenuStr);
	itsVertMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsVertMenu->AttachHandlers(this,
		&TestPartitionDirector::UpdateVertMenu,
		&TestPartitionDirector::HandleVertMenu);

	for (JIndex i=1; i<=kInitCompartmentCount; i++)
	{
		if (i != kInitElasticIndex)
		{
			CreateTestWidget(itsHorizPartition, i);
		}
		CreateTestWidget(itsVertPartition, i);
	}
}

/******************************************************************************
 InsertHorizCompartment

 ******************************************************************************/

void
TestPartitionDirector::InsertHorizCompartment
	(
	const JIndex index
	)
{
	JXContainer* newCompartment;
	if (itsHorizPartition->InsertCompartment(index, kInitSize, kMinSize,
											 &newCompartment))
	{
		AdjustMinWindowSize();
		CreateTestWidget(itsHorizPartition, index);
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("OutOfSpace::TestPartitionDirector"));
	}
}

/******************************************************************************
 DeleteHorizCompartment

 ******************************************************************************/

void
TestPartitionDirector::DeleteHorizCompartment
	(
	const JIndex index
	)
{
	itsHorizPartition->JPartition::DeleteCompartment(index);
}

/******************************************************************************
 InsertVertCompartment

 ******************************************************************************/

void
TestPartitionDirector::InsertVertCompartment
	(
	const JIndex index
	)
{
	JXContainer* newCompartment;
	if (itsVertPartition->InsertCompartment(index, kInitSize, kMinSize,
											&newCompartment))
	{
		AdjustMinWindowSize();
		CreateTestWidget(itsVertPartition, index);
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("OutOfSpace::TestPartitionDirector"));
	}
}

/******************************************************************************
 DeleteVertCompartment

 ******************************************************************************/

void
TestPartitionDirector::DeleteVertCompartment
	(
	const JIndex index
	)
{
	if (itsVertPartition->GetCompartmentCount() > 1)
	{
		itsVertPartition->JPartition::DeleteCompartment(index);
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("CannotRemoveLast::TestPartitionDirector"));
	}
}

/******************************************************************************
 CreateTestWidget (private)

 ******************************************************************************/

void
TestPartitionDirector::CreateTestWidget
	(
	JXPartition*	partition,
	const JIndex	index
	)
{
	TestCompartment* obj =
		jnew TestCompartment(this, partition->GetCompartment(index),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);
	assert( obj != nullptr );
	obj->FitToEnclosure(true, true);
}

/******************************************************************************
 AdjustMinWindowSize (private)

 ******************************************************************************/

void
TestPartitionDirector::AdjustMinWindowSize()
{
	GetWindow()->SetMinSize(itsHorizPartition->GetMinTotalSize(),
							  kMenuBarHeight + itsVertPartition->GetMinTotalSize());
}

/******************************************************************************
 UpdateHorizMenu (private)

 ******************************************************************************/

void
TestPartitionDirector::UpdateHorizMenu()
{
}

/******************************************************************************
 HandleHorizMenu (private)

 ******************************************************************************/

void
TestPartitionDirector::HandleHorizMenu
	(
	const JIndex index
	)
{
	if (index == kPrependHorizCmd)
	{
		InsertHorizCompartment(1);
	}
	else if (index == kAppendHorizCmd)
	{
		InsertHorizCompartment(itsHorizPartition->GetCompartmentCount()+1);
	}
	else if (index == kSetHorizElasticIndexCmd)
	{
		JIndex elasticIndex;
		itsHorizPartition->GetElasticIndex(&elasticIndex);
		const JSize count = itsHorizPartition->GetCompartmentCount();

		auto* dlog = jnew SetElasticDialog(elasticIndex, count);
		assert( dlog != nullptr );

		if (dlog->DoDialog())
		{
			itsHorizPartition->SetElasticIndex(dlog->GetElasticIndex());
		}
	}
}

/******************************************************************************
 UpdateVertMenu (private)

 ******************************************************************************/

void
TestPartitionDirector::UpdateVertMenu()
{
}

/******************************************************************************
 HandleVertMenu (private)

 ******************************************************************************/

void
TestPartitionDirector::HandleVertMenu
	(
	const JIndex index
	)
{
	if (index == kSetVertElasticIndexCmd)
	{
		JIndex elasticIndex;
		itsVertPartition->GetElasticIndex(&elasticIndex);
		const JSize count = itsVertPartition->GetCompartmentCount();

		auto* dlog = jnew SetElasticDialog(elasticIndex, count);
		assert( dlog != nullptr );

		if (dlog->DoDialog())
		{
			itsVertPartition->SetElasticIndex(dlog->GetElasticIndex());
		}
	}
}
