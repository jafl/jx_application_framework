/******************************************************************************
 MenuPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "MenuPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include "MenuPanel-ArrowPosition.h"
#include "MenuPanel-ArrowDirection.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuPanel::MenuPanel
	(
	WidgetParametersDialog* dlog,

	const Menu::Type				type,
	const JString&					title,
	const JXMenu::ArrowPosition		pos,
	const JXMenu::ArrowDirection	dir,
	const JSize						colCount
	)
	:
	itsArrowPositionIndex(pos+1),
	itsArrowDirectionIndex(dir+1)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	auto* titleLabel =
		jnew JXStaticText(JGetString("titleLabel::MenuPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 40,20);
	titleLabel->SetToLabel(false);

	itsArrowPositionMenu =
		jnew JXTextMenu(JGetString("itsArrowPositionMenu::MenuPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 190,20);

	auto* columnCountLabel =
		jnew JXStaticText(JGetString("columnCountLabel::MenuPanel::Panel"), container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,50, 70,20);
	columnCountLabel->SetToLabel(false);

	itsArrowDirectionMenu =
		jnew JXTextMenu(JGetString("itsArrowDirectionMenu::MenuPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 190,20);

	itsTitleInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 380,20);
	itsTitleInput->SetIsRequired();

	itsColumnCountInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,50, 50,20);
	itsColumnCountInput->SetLowerLimit(1);

// end Panel

	dlog->AddPanel(this, container);

	itsTitleInput->GetText()->SetText(title);

	itsArrowPositionMenu->SetMenuItems(kArrowPositionMenuStr);
	itsArrowPositionMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsArrowPositionMenu->SetToPopupChoice(true, itsArrowPositionIndex);
	ConfigureArrowPositionMenu(itsArrowPositionMenu);

	ListenTo(itsArrowPositionMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsArrowPositionMenu->CheckItem(itsArrowPositionIndex);
	}));

	ListenTo(itsArrowPositionMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsArrowPositionIndex = msg.GetIndex();
	}));

	itsArrowDirectionMenu->SetMenuItems(kArrowDirectionMenuStr);
	itsArrowDirectionMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsArrowDirectionMenu->SetToPopupChoice(true, itsArrowDirectionIndex);
	ConfigureArrowDirectionMenu(itsArrowDirectionMenu);

	ListenTo(itsArrowDirectionMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsArrowDirectionMenu->CheckItem(itsArrowDirectionIndex);
	}));

	ListenTo(itsArrowDirectionMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsArrowDirectionIndex = msg.GetIndex();
	}));

	if (type == Menu::kImageType)
	{
		itsColumnCountInput->SetValue(colCount);
	}
	else
	{
		columnCountLabel->Hide();
		itsColumnCountInput->Hide();
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MenuPanel::~MenuPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
MenuPanel::GetValues
	(
	JString*				title,
	JXMenu::ArrowPosition*	pos,
	JXMenu::ArrowDirection*	dir,
	JSize*					colCount
	)
{
	*title = itsTitleInput->GetText()->GetText();
	*pos   = (JXMenu::ArrowPosition) (itsArrowPositionIndex-1);
	*dir   = (JXMenu::ArrowDirection) (itsArrowDirectionIndex-1);

	JInteger v;
	itsColumnCountInput->GetValue(&v);
	*colCount = v;
}
