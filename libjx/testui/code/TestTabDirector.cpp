/******************************************************************************
 TestTabDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestTabDirector.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTabGroup.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXFontNameMenu.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXChooseMonoFont.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
	JXWindow* window = jnew JXWindow(this, 300,300, JString::empty);

	window->SetTitle(JGetString("WindowTitle::TestTabDirector"));
	window->SetWMClass("testjx", "TestTabDirector");
	window->LockCurrentMinSize();

	itsTabGroup = jnew JXTabGroup(window, JXWidget::kHElastic, JXWidget::kVElastic,
								 5, 5, 290, 290);

	JXContainer* card3 = itsTabGroup->AppendTab(JGetString("Tab3Label::TestTabDirector"));
	JXContainer* card1 = itsTabGroup->PrependTab(JGetString("Tab1Label::TestTabDirector"));
	JXContainer* card2 = itsTabGroup->InsertTab(2, JGetString("Tab2Label::TestTabDirector"));

	// card 1

	itsFontMenu =
		jnew JXFontNameMenu(JGetString("FontMenuTitle::TestTabDirector"), true, card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,30);
	assert( itsFontMenu != nullptr );
	itsFontMenu->SetFontName(itsTabGroup->GetFont().GetName());
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu, std::function([this](const JXFontNameMenu::NameChanged&)
	{
		const JString name = itsFontMenu->GetFontName();
		itsTabGroup->SetFontName(name);
		itsSizeMenu->SetFontName(name);
	}));

	const JString fontName = itsFontMenu->GetFontName();

	itsSizeMenu =
		jnew JXFontSizeMenu(fontName, JGetString("SizeMenuTitle::TestTabDirector"), card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 50,30);
	assert( itsSizeMenu != nullptr );
	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu, std::function([this](const JXFontSizeMenu::SizeChanged& msg)
	{
		itsTabGroup->SetFontSize(msg.GetSize());
	}));

	itsEdgeRG = jnew JXRadioGroup(card1, JXWidget::kFixedLeft, JXWidget::kFixedTop,
								 20,100, 100,140);

	JXTextRadioButton* rb1 =
		jnew JXTextRadioButton(JXTabGroup::kTop, JGetString("TopRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,10, 80,20);
	assert( rb1 != nullptr );

	JXTextRadioButton* rb2 =
		jnew JXTextRadioButton(JXTabGroup::kLeft, JGetString("LeftRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,40, 80,20);
	assert( rb2 != nullptr );

	JXTextRadioButton* rb3 =
		jnew JXTextRadioButton(JXTabGroup::kBottom, JGetString("BottomRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,70, 80,20);
	assert( rb3 != nullptr );

	JXTextRadioButton* rb4 =
		jnew JXTextRadioButton(JXTabGroup::kRight, JGetString("RightRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,100, 80,20);
	assert( rb4 != nullptr );

	itsEdgeRG->SelectItem(itsTabGroup->GetTabEdge());
	ListenTo(itsEdgeRG, std::function([this](const JXRadioGroup::SelectionChanged& msg)
	{
		itsTabGroup->SetTabEdge((JXTabGroup::Edge) msg.GetID());
	}));

	// card 2

	itsAddTabButton =
		jnew JXTextButton(JGetString("NewTabButtonLabel::TestTabDirector"), card2,
						 JXWidget::kFixedLeft, JXWidget::kFixedBottom,
						 20, 20, 200, 30);
	assert( itsAddTabButton != nullptr );
	ListenTo(itsAddTabButton);

	JXIntegerInput* input =
		jnew JXIntegerInput(card2, JXWidget::kHElastic, JXWidget::kFixedTop,
						   20, 60, 250, 20);
	input->SetLimits(-10, 10);
	input->SetValue(0);

	// card 3

	itsMonoFontSample =
		jnew JXInputField(card3, JXWidget::kHElastic, JXWidget::kFixedTop,
						 20, 20, 200, 20);
	itsMonoFontSample->GetText()->SetText(JGetString("SampleText::TestTabDirector"));

	itsMonoFont =
		jnew JXChooseMonoFont(card3, JXWidget::kHElastic, JXWidget::kFixedTop,
							 20, 50, 300, 100);
	ListenTo(itsMonoFont, std::function([this](const JXChooseMonoFont::FontChanged&)
	{
		UpdateFontSample();
	}));

	UpdateFontSample();
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
		JString s(itsNextTabIndex, 0);
		s.Prepend(JGetString("TabLabelPrefix::TestTabDirector"));
		JXContainer* card = itsTabGroup->AppendTab(s, true);
		itsNextTabIndex++;

		JXTextButton* removeButton =
			jnew JXTextButton(JGetString("RemoveTabButtonLabel::TestTabDirector"), card,
							 JXWidget::kFixedRight, JXWidget::kFixedTop,
							 20,20, 200, 30);
		assert( removeButton != nullptr );
		ListenTo(removeButton);

		card->ExpandToFitContent();
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

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFontSample (private)

 ******************************************************************************/

void
TestTabDirector::UpdateFontSample()
{
	JString name;
	JSize size;
	itsMonoFont->GetFont(&name, &size);
	itsMonoFontSample->SetFont(JFontManager::GetFont(name, size));
}
