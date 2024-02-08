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

const JSize kInitSize = 100;
const JSize kMinSize  = 50;

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

#include "TestPartitionDirector-Horiz.h"
#include "TestPartitionDirector-Vert.h"

void
TestPartitionDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,340, JGetString("WindowTitle::TestPartitionDirector::JXLayout"));
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestPartitionDirector");

	JArray<JCoordinate> itsHorizPartition_sizes, itsHorizPartition_minSizes;
	itsHorizPartition_sizes.AppendItem(95);
	itsHorizPartition_minSizes.AppendItem(50);
	itsHorizPartition_sizes.AppendItem(109);
	itsHorizPartition_minSizes.AppendItem(50);
	itsHorizPartition_sizes.AppendItem(96);
	itsHorizPartition_minSizes.AppendItem(50);

	JArray<JCoordinate> itsVertPartition_sizes, itsVertPartition_minSizes;
	itsVertPartition_sizes.AppendItem(103);
	itsVertPartition_minSizes.AppendItem(50);
	itsVertPartition_sizes.AppendItem(101);
	itsVertPartition_minSizes.AppendItem(50);
	itsVertPartition_sizes.AppendItem(96);
	itsVertPartition_minSizes.AppendItem(50);

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 310,30);

	itsHorizPartition =
		jnew JXHorizPartition(itsHorizPartition_sizes, 2, itsHorizPartition_minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 310,310);

	itsVertPartition =
		jnew JXVertPartition(itsVertPartition_sizes, 2, itsVertPartition_minSizes, itsHorizPartition->GetCompartment(2),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 109,310);

// end JXLayout

	AdjustMinWindowSize();

	itsHorizMenu = itsMenuBar->AppendTextMenu(JGetString("MenuTitle::TestPartitionDirector_Horiz"));
	itsHorizMenu->SetMenuItems(kHorizMenuStr);
	itsHorizMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsHorizMenu->AttachHandlers(this,
		&TestPartitionDirector::UpdateHorizMenu,
		&TestPartitionDirector::HandleHorizMenu);
	ConfigureHorizMenu(itsHorizMenu);

	itsVertMenu = itsMenuBar->AppendTextMenu(JGetString("MenuTitle::TestPartitionDirector_Vert"));
	itsVertMenu->SetMenuItems(kVertMenuStr);
	itsVertMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsVertMenu->AttachHandlers(this,
		&TestPartitionDirector::UpdateVertMenu,
		&TestPartitionDirector::HandleVertMenu);
	ConfigureVertMenu(itsVertMenu);

	JIndex elasticIndex;
	itsHorizPartition->GetElasticIndex(&elasticIndex);

	for (JIndex i=1; i<=itsHorizPartition_sizes.GetItemCount(); i++)
	{
		if (i != elasticIndex)
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
	itsHorizPartition->DeleteCompartment(index);
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
		itsVertPartition->DeleteCompartment(index);
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
								itsMenuBar->GetFrameHeight() +
									itsVertPartition->GetMinTotalSize());
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

		if (dlog->DoDialog())
		{
			itsVertPartition->SetElasticIndex(dlog->GetElasticIndex());
		}
	}
}
