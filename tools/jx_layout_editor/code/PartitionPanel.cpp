/******************************************************************************
 PartitionPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "PartitionPanel.h"
#include "WidgetParametersDialog.h"
#include "PartitionMinSizeTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/JIntegerTableData.h>
#include <jx-af/jcore/JTableSelection.h>
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

	itsData = jnew JIntegerTableData(10);
	itsData->AppendRows(minSizes.GetItemCount());
	itsData->AppendCols(1, &minSizes);

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);

	auto* scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 80,80);

	itsTable =
		jnew PartitionMinSizeTable(itsData, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 80,80);

	itsAddRowButton =
		jnew JXTextButton(JGetString("itsAddRowButton::PartitionPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,10, 70,20);

	auto* hint =
		jnew JXStaticText(JGetString("hint::PartitionPanel::Panel"), true, false, false, nullptr, container,
					JXWidget::kFixedRight, JXWidget::kVElastic, 200,10, 250,50);
	hint->SetBorderWidth(0);

	itsRemoveRowButton =
		jnew JXTextButton(JGetString("itsRemoveRowButton::PartitionPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,40, 70,20);

	itsHasElasticIndexCB =
		jnew JXTextCheckbox(JGetString("itsHasElasticIndexCB::PartitionPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 200,70, 110,20);
	itsHasElasticIndexCB->SetShortcuts(JGetString("itsHasElasticIndexCB::shortcuts::PartitionPanel::Panel"));

	itsElasticIndexInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,70, 50,20);
	itsElasticIndexInput->SetIsRequired(false);
	itsElasticIndexInput->SetLowerLimit(1);

// end Panel

	dlog->AddPanel(this, container);

	ListenTo(itsAddRowButton, std::function([this](const JXButton::Pushed&)
	{
		itsTable->GetTableSelection().ClearSelection();
		itsData->AppendRows(1);
		itsTable->BeginEditing(JPoint(1, itsData->GetRowCount()));
		UpdateDisplay();
	}));

	ListenTo(itsRemoveRowButton, std::function([this](const JXButton::Pushed&)
	{
		JTableSelection& s = itsTable->GetTableSelection();
		JPoint cell;
		if (s.GetFirstSelectedCell(&cell))
		{
			itsTable->CancelEditing();
			itsData->RemoveRow(cell.y);
		}
		UpdateDisplay();
	}));

	ListenTo(&itsTable->GetTableSelection(), std::function([this](const JTableData::RectChanged&)
	{
		UpdateDisplay();
	}));

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

	itsRemoveRowButton->SetActive(
		itsTable->GetTableSelection().HasSelection() &&
		itsData->GetRowCount() > 2);
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
PartitionPanel::Validate()
	const
{
	JInteger elasticIndex;
	if (itsHasElasticIndexCB->IsChecked() &&
		itsElasticIndexInput->GetValue(&elasticIndex) &&
		JSize(elasticIndex) > itsTable->GetRowCount())
	{
		JGetUserNotification()->ReportError(
			JGetString("InvalidElasticIndex::PartitionPanel"));
		JXFocusWidgetTask::Focus(itsElasticIndexInput);
		return false;
	}

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
	itsData->GetCol(1, minSizes);

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
