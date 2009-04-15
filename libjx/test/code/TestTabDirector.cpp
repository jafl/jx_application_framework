/******************************************************************************
 TestTabDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestTabDirector.h"
#include <JXWindow.h>
#include <JXTabGroup.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestTabDirector::TestTabDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();

	itsNextTabIndex = itsTabGroup->GetTabCount() + 1;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTabDirector::~TestTabDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestTabDirector::BuildWindow()
{
	JXWindow* window = new JXWindow(this, 300,300, "");
	assert( window != NULL );
	SetWindow(window);

	window->SetTitle("Test Tab Group");
	window->SetWMClass("testjx", "TestTabDirector");
	window->LockCurrentMinSize();

	itsTabGroup = new JXTabGroup(window, JXWidget::kHElastic, JXWidget::kVElastic,
								 5, 5, 290, 290);
	assert( itsTabGroup != NULL );

	JXWidgetSet* card3 = itsTabGroup->AppendTab("Testing");
	JXWidgetSet* card1 = itsTabGroup->PrependTab("First tab");
	JXWidgetSet* card2 = itsTabGroup->InsertTab(2, "Tab #2");

	// card 1

	itsFontMenu =
		new JXFontNameMenu("Font:", card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,30);
	assert( itsFontMenu != NULL );
	itsFontMenu->SetFontName(itsTabGroup->GetFontName());
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu);

	const JString fontName = itsFontMenu->GetFontName();

	itsSizeMenu =
		new JXFontSizeMenu(fontName, "Size:", card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 50,30);
	assert( itsSizeMenu != NULL );
	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu);

	itsEdgeRG = new JXRadioGroup(card1, JXWidget::kFixedLeft, JXWidget::kFixedTop,
								 20,100, 100,140);
	assert( itsEdgeRG != NULL );

	JXTextRadioButton* rb1 =
		new JXTextRadioButton(JXTabGroup::kTop, "Top", itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,10, 80,20);
	assert( rb1 != NULL );

	JXTextRadioButton* rb2 =
		new JXTextRadioButton(JXTabGroup::kLeft, "Left", itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,40, 80,20);
	assert( rb2 != NULL );

	JXTextRadioButton* rb3 =
		new JXTextRadioButton(JXTabGroup::kBottom, "Bottom", itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,70, 80,20);
	assert( rb3 != NULL );

	JXTextRadioButton* rb4 =
		new JXTextRadioButton(JXTabGroup::kRight, "Right", itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,100, 80,20);
	assert( rb4 != NULL );

	itsEdgeRG->SelectItem(itsTabGroup->GetTabEdge());
	ListenTo(itsEdgeRG);

	// card 2

	itsAddTabButton =
		new JXTextButton("Add new tab", card2,
						 JXWidget::kFixedLeft, JXWidget::kFixedBottom,
						 20, 20, 200, 30);
	assert( itsAddTabButton != NULL );
	ListenTo(itsAddTabButton);

	// card 3

	JXIntegerInput* input =
		new JXIntegerInput(card3, JXWidget::kHElastic, JXWidget::kFixedTop,
						   20, 20, 250, 20);
	assert( input != NULL );
	input->SetLimits(-10, 10);
	input->SetValue(0);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestTabDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddTabButton && message.Is(JXButton::kPushed))
		{
		JString s(itsNextTabIndex);
		s.Prepend("Tab #");
		JXWidgetSet* card = itsTabGroup->AppendTab(s, kJTrue);
		itsNextTabIndex++;

		JXTextButton* removeButton =
			new JXTextButton("Remove this tab", card,
							 JXWidget::kFixedRight, JXWidget::kFixedTop,
							 20,20, 200, 30);
		assert( removeButton != NULL );
		ListenTo(removeButton);
		}
	else if (message.Is(JXButton::kPushed))
		{
		JIndex i;
		if (itsTabGroup->GetCurrentTabIndex(&i))
			{
			itsTabGroup->ShowTab(2);
			itsTabGroup->DeleteTab(i);
			}
		}

	else if (sender == itsFontMenu && message.Is(JXFontNameMenu::kNameChanged))
		{
		const JString name = itsFontMenu->GetFontName();
		itsTabGroup->SetFontName(name);
		itsSizeMenu->SetFontName(name);
		}
	else if (sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged))
		{
		itsTabGroup->SetFontSize(itsSizeMenu->GetFontSize());
		}

	else if (sender == itsEdgeRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		itsTabGroup->SetTabEdge((JXTabGroup::Edge) itsEdgeRG->GetSelectedItem());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}
