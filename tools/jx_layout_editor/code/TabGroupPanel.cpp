/******************************************************************************
 TabGroupPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "TabGroupPanel.h"
#include "WidgetParametersDialog.h"
#include "TabTitleTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include "TabGroupPanel-Edge.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

TabGroupPanel::TabGroupPanel
	(
	WidgetParametersDialog* dlog,

	const JPtrArray<JString>&	titles,
	const JXTabGroup::Edge		edge
	)
	:
	itsEdgeIndex(edge+1)
{
	auto* window = dlog->GetWindow();

	itsData = jnew JStringTableData;
	itsData->AppendRows(titles.GetItemCount());
	itsData->AppendCols(1, &titles);

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);

	auto* scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 80,80);

	itsTable =
		jnew TabTitleTable(itsData, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 80,80);

	itsAddRowButton =
		jnew JXTextButton(JGetString("itsAddRowButton::TabGroupPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,10, 70,20);

	auto* hint =
		jnew JXStaticText(JGetString("hint::TabGroupPanel::Panel"), true, false, false, nullptr, container,
					JXWidget::kFixedRight, JXWidget::kVElastic, 200,10, 250,30);
	hint->SetBorderWidth(0);

	itsRemoveRowButton =
		jnew JXTextButton(JGetString("itsRemoveRowButton::TabGroupPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,40, 70,20);

	itsEdgeMenu =
		jnew JXTextMenu(JGetString("itsEdgeMenu::TabGroupPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,60, 240,30);

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

	itsEdgeMenu->SetTitleText(JGetString("MenuTitle::TabGroupPanel_Edge"));
	itsEdgeMenu->SetMenuItems(kEdgeMenuStr);
	itsEdgeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureEdgeMenu(itsEdgeMenu);
	itsEdgeMenu->SetToPopupChoice(true, itsEdgeIndex);

	ListenTo(itsEdgeMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsEdgeMenu->CheckItem(itsEdgeIndex);
	}));

	ListenTo(itsEdgeMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsEdgeIndex = msg.GetIndex();
	}));

	UpdateDisplay();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TabGroupPanel::~TabGroupPanel()
{
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
TabGroupPanel::UpdateDisplay()
	const
{
	itsRemoveRowButton->SetActive(
		itsTable->GetTableSelection().HasSelection() &&
		itsData->GetRowCount() > 1);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
TabGroupPanel::GetValues
	(
	JPtrArray<JString>*	titles,
	JXTabGroup::Edge*	edge
	)
{
	itsData->GetCol(1, titles);
	*edge = (JXTabGroup::Edge) (itsEdgeIndex-1);
}
