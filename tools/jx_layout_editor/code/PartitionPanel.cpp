/******************************************************************************
 PartitionPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "PartitionPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PartitionPanel::PartitionPanel
	(
	WidgetParametersDialog* dlog,

	const JArray<JCoordinate>&	minSizes,
	const JIndex				elasticIndex
	)
{
	auto* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,90);
	assert( container != nullptr );

	itsHasElasticIndexCB =
		jnew JXTextCheckbox(JGetString("itsHasElasticIndexCB::PartitionPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	itsHasElasticIndexCB->SetShortcuts(JGetString("itsHasElasticIndexCB::shortcuts::PartitionPanel::Panel"));

	itsElasticIndexInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 50,20);
	itsElasticIndexInput->SetIsRequired(false);
	itsElasticIndexInput->SetLowerLimit(1);

// end Panel

	dlog->AddPanel(this, container);

	if (elasticIndex > 0)
	{
		itsHasElasticIndexCB->SetState(true);
		itsElasticIndexInput->SetValue(elasticIndex);
	}
	else
	{
		itsElasticIndexInput->GetText()->SetText(JString::empty);
	}

	ListenTo(itsHasElasticIndexCB, std::function([this](const JXCheckbox::Pushed&)
	{
		UpdateDisplay();
		if (itsHasElasticIndexCB->IsChecked())
		{
			JXFocusWidgetTask::Focus(itsElasticIndexInput);
		}
	}));

	UpdateDisplay();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PartitionPanel::~PartitionPanel()
{
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
PartitionPanel::UpdateDisplay()
	const
{
	const bool elastic = itsHasElasticIndexCB->IsChecked();
	itsElasticIndexInput->SetActive(elastic);
	itsElasticIndexInput->SetIsRequired(elastic);
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
PartitionPanel::Validate()
	const
{
	// todo: ensure elasticIndex < # of min sizes

	return true;
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
PartitionPanel::GetValues
	(
	JArray<JCoordinate>*	minSizes,
	JIndex*					elasticIndex
	)
{
	if (itsHasElasticIndexCB->IsChecked())
	{
		JInteger v;
		const bool ok = itsElasticIndexInput->GetValue(&v);
		assert( ok );

		*elasticIndex = v;
	}
	else
	{
		*elasticIndex = 0;
	}
}
