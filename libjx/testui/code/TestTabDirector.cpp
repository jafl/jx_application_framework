/******************************************************************************
 TestTabDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestTabDirector.h"
#include <JXWindow.h>
#include <JXTabGroup.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXChooseMonoFont.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFontManager.h>
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
	JXWindow* window = jnew JXWindow(this, 300,300, JString::empty);
	assert( window != NULL );

	window->SetTitle(JGetString("WindowTitle::TestTabDirector"));
	window->SetWMClass("testjx", "TestTabDirector");
	window->LockCurrentMinSize();

	itsTabGroup = jnew JXTabGroup(window, JXWidget::kHElastic, JXWidget::kVElastic,
								 5, 5, 290, 290);
	assert( itsTabGroup != NULL );

	JXContainer* card3 = itsTabGroup->AppendTab(JGetString("Tab3Label::TestTabDirector"));
	JXContainer* card1 = itsTabGroup->PrependTab(JGetString("Tab1Label::TestTabDirector"));
	JXContainer* card2 = itsTabGroup->InsertTab(2, JGetString("Tab2Label::TestTabDirector"));

	// card 1

	itsFontMenu =
		jnew JXFontNameMenu(JGetString("FontMenuTitle::TestTabDirector"), card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,30);
	assert( itsFontMenu != NULL );
	itsFontMenu->SetFontName(itsTabGroup->GetFont().GetName());
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu);

	const JString fontName = itsFontMenu->GetFontName();

	itsSizeMenu =
		jnew JXFontSizeMenu(fontName, JGetString("SizeMenuTitle::TestTabDirector"), card1,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 50,30);
	assert( itsSizeMenu != NULL );
	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu);

	itsEdgeRG = jnew JXRadioGroup(card1, JXWidget::kFixedLeft, JXWidget::kFixedTop,
								 20,100, 100,140);
	assert( itsEdgeRG != NULL );

	JXTextRadioButton* rb1 =
		jnew JXTextRadioButton(JXTabGroup::kTop, JGetString("TopRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,10, 80,20);
	assert( rb1 != NULL );

	JXTextRadioButton* rb2 =
		jnew JXTextRadioButton(JXTabGroup::kLeft, JGetString("LeftRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,40, 80,20);
	assert( rb2 != NULL );

	JXTextRadioButton* rb3 =
		jnew JXTextRadioButton(JXTabGroup::kBottom, JGetString("BottomRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,70, 80,20);
	assert( rb3 != NULL );

	JXTextRadioButton* rb4 =
		jnew JXTextRadioButton(JXTabGroup::kRight, JGetString("RightRBLabel::TestTabDirector"), itsEdgeRG,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  10,100, 80,20);
	assert( rb4 != NULL );

	itsEdgeRG->SelectItem(itsTabGroup->GetTabEdge());
	ListenTo(itsEdgeRG);

	// card 2

	itsAddTabButton =
		jnew JXTextButton(JGetString("NewTabButtonLabel::TestTabDirector"), card2,
						 JXWidget::kFixedLeft, JXWidget::kFixedBottom,
						 20, 20, 200, 30);
	assert( itsAddTabButton != NULL );
	ListenTo(itsAddTabButton);

	JXIntegerInput* input =
		jnew JXIntegerInput(card2, JXWidget::kHElastic, JXWidget::kFixedTop,
						   20, 60, 250, 20);
	assert( input != NULL );
	input->SetLimits(-10, 10);
	input->SetValue(0);

	// card 3

	itsMonoFontSample =
		jnew JXInputField(card3, JXWidget::kHElastic, JXWidget::kFixedTop,
						 20, 20, 250, 20);
	assert( itsMonoFontSample != NULL );
	itsMonoFontSample->SetText(JGetString("SampleText::TestTabDirector"));

	itsMonoFont =
		jnew JXChooseMonoFont(card3, JXWidget::kHElastic, JXWidget::kFixedTop,
							 20, 50, 300, 100);
	assert( itsMonoFont != NULL );
	ListenTo(itsMonoFont);

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
		JString s(itsNextTabIndex);
		s.Prepend(JGetString("TabLabelPrefix::TestTabDirector"));
		JXContainer* card = itsTabGroup->AppendTab(s, kJTrue);
		itsNextTabIndex++;

		JXTextButton* removeButton =
			jnew JXTextButton(JGetString("RemoveTabButtonLabel::TestTabDirector"), card,
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

	else if (sender == itsMonoFont && message.Is(JXChooseMonoFont::kFontChanged))
		{
		UpdateFontSample();
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
	itsMonoFontSample->SetFont(GetWindow()->GetFontManager()->GetFont(name, size));
}
