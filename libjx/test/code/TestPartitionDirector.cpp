/******************************************************************************
 TestPartitionDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestPartitionDirector.h"
#include "TestCompartment.h"
#include "SetElasticDialog.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXHorizPartition.h>
#include <JXVertPartition.h>
#include <jXGlobals.h>
#include <jAssert.h>

const JSize kInitCompartmentCount = 3;
const JIndex kInitElasticIndex    = 2;
const JSize kInitSize             = 100;
const JSize kMinSize              = 50;
const JSize kMenuBarHeight        = 30;

// Horiz menu information

static const JCharacter* kHorizMenuTitleStr  = "Horiz";
static const JCharacter* kHorizMenuShortcuts = "#H";
static const JCharacter* kHorizMenuStr =
	"Prepend compartment %h p | Append compartment %h a"
	"%l | Set elastic compartment %h s";

enum
{
	kPrependHorizCmd = 1,
	kAppendHorizCmd,
	kSetHorizElasticIndexCmd
};

// Vert menu information

static const JCharacter* kVertMenuTitleStr  = "Vert";
static const JCharacter* kVertMenuShortcuts = "#V";
static const JCharacter* kVertMenuStr =
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
	itsSetElasticDialog = NULL;
	itsSetElasticType   = kHorizElastic;

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
JIndex i;

	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;

	for (i=1; i<=kInitCompartmentCount; i++)
		{
		sizes.AppendElement(kInitSize);
		minSizes.AppendElement(kMinSize);
		}

// begin JXLayout

    JXWindow* window = new JXWindow(this, 315,345, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 315,30);
    assert( menuBar != NULL );

    itsHorizPartition =
        new JXHorizPartition(sizes, kInitElasticIndex, minSizes, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 315,315);
    assert( itsHorizPartition != NULL );

// end JXLayout

	itsVertPartition =
		new JXVertPartition(sizes, kInitElasticIndex, minSizes,
					itsHorizPartition->GetCompartment(kInitElasticIndex),
					JXWidget::kHElastic, JXWidget::kVElastic,
					0,0, 10,315);
	assert( itsVertPartition != NULL );
	itsVertPartition->FitToEnclosure(kJTrue, kJTrue);

	window->SetTitle("Test Partitions");
	window->SetWMClass("testjx", "TestPartitionDirector");
	AdjustMinWindowSize();

	itsHorizMenu = menuBar->AppendTextMenu(kHorizMenuTitleStr);
	itsHorizMenu->SetShortcuts(kHorizMenuShortcuts);
	itsHorizMenu->SetMenuItems(kHorizMenuStr);
	itsHorizMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHorizMenu);

	itsVertMenu = menuBar->AppendTextMenu(kVertMenuTitleStr);
	itsVertMenu->SetShortcuts(kVertMenuShortcuts);
	itsVertMenu->SetMenuItems(kVertMenuStr);
	itsVertMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsVertMenu);

	for (i=1; i<=kInitCompartmentCount; i++)
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
		(JGetUserNotification())->ReportError("There is not enough space for another compartment.");
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
		(JGetUserNotification())->ReportError("There is not enough space for another compartment.");
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
		(JGetUserNotification())->ReportError("You can't remove the last compartment.");
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
		new TestCompartment(this, partition->GetCompartment(index),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 10,10);
	assert( obj != NULL );
	obj->FitToEnclosure(kJTrue, kJTrue);
}

/******************************************************************************
 AdjustMinWindowSize (private)

 ******************************************************************************/

void
TestPartitionDirector::AdjustMinWindowSize()
{
	(GetWindow())->SetMinSize(itsHorizPartition->GetMinTotalSize(),
							  kMenuBarHeight + itsVertPartition->GetMinTotalSize());
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestPartitionDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHorizMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHorizMenu();
		}
	else if (sender == itsHorizMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHorizMenu(selection->GetIndex());
		}

	else if (sender == itsVertMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVertMenu();
		}
	else if (sender == itsVertMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleVertMenu(selection->GetIndex());
		}

	else if (sender == itsSetElasticDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		const JBoolean ok = info->Successful();
		const JIndex newElasticIndex = itsSetElasticDialog->GetElasticIndex();
		if (ok && itsSetElasticType == kHorizElastic)
			{
			itsHorizPartition->SetElasticIndex(newElasticIndex);
			}
		else if (ok && itsSetElasticType == kVertElastic)
			{
			itsVertPartition->SetElasticIndex(newElasticIndex);
			}
		itsSetElasticDialog = NULL;
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
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
		assert( itsSetElasticDialog == NULL );

		JIndex elasticIndex;
		itsHorizPartition->GetElasticIndex(&elasticIndex);
		const JSize count = itsHorizPartition->GetCompartmentCount();

		itsSetElasticDialog = new SetElasticDialog(this, elasticIndex, count);
		assert( itsSetElasticDialog != NULL );
		ListenTo(itsSetElasticDialog);
		itsSetElasticDialog->BeginDialog();

		itsSetElasticType = kHorizElastic;
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
		assert( itsSetElasticDialog == NULL );

		JIndex elasticIndex;
		itsVertPartition->GetElasticIndex(&elasticIndex);
		const JSize count = itsVertPartition->GetCompartmentCount();

		itsSetElasticDialog = new SetElasticDialog(this, elasticIndex, count);
		assert( itsSetElasticDialog != NULL );
		ListenTo(itsSetElasticDialog);
		itsSetElasticDialog->BeginDialog();

		itsSetElasticType = kVertElastic;
		}
}
