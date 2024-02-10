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
// begin JXLayout

	auto* window = jnew JXWindow(this, 300,300, JGetString("WindowTitle::TestTabDirector::JXLayout"));
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestTabDirector");

	itsTabGroup =
		jnew JXTabGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,5, 290,290);
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab1::TestTabDirector::JXLayout"));
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab2::TestTabDirector::JXLayout"));
	itsTabGroup->AppendTab(JGetString("itsTabGroup::tab3::TestTabDirector::JXLayout"));

	itsAddTabButton =
		jnew JXTextButton(JGetString("itsAddTabButton::TestTabDirector::JXLayout"), itsTabGroup->GetTabEnclosure(2),
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 240,30);

	itsFontMenu =
		jnew JXFontNameMenu(JGetString("FontMenuTitle::TestTabDirector"), true, itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 240,30);

	itsMonoFont =
		jnew JXChooseMonoFont(itsTabGroup->GetTabEnclosure(3),
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,50, 230,60);

	itsSizeMenu =
		jnew JXFontSizeMenu(itsFontMenu, JGetString("SizeMenuTitle::TestTabDirector"), itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 240,30);

	itsEdgeRG =
		jnew JXRadioGroup(itsTabGroup->GetTabEnclosure(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 240,140);

	auto* rb1 =
		jnew JXTextRadioButton(JXTabGroup::kTop, JGetString("rb1::TestTabDirector::JXLayout"), itsEdgeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 210,20);
	assert( rb1 != nullptr );

	auto* rb2 =
		jnew JXTextRadioButton(JXTabGroup::kLeft, JGetString("rb2::TestTabDirector::JXLayout"), itsEdgeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 210,20);
	assert( rb2 != nullptr );

	auto* rb3 =
		jnew JXTextRadioButton(JXTabGroup::kBottom, JGetString("rb3::TestTabDirector::JXLayout"), itsEdgeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 210,20);
	assert( rb3 != nullptr );

	auto* rb4 =
		jnew JXTextRadioButton(JXTabGroup::kRight, JGetString("rb4::TestTabDirector::JXLayout"), itsEdgeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,100, 210,20);
	assert( rb4 != nullptr );

	auto* input =
		jnew JXIntegerInput(itsTabGroup->GetTabEnclosure(2),
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,60, 240,20);
	input->SetIsRequired(false);
	input->SetLowerLimit(-10);
	input->SetUpperLimit(10);

	itsMonoFontSample =
		jnew JXInputField(itsTabGroup->GetTabEnclosure(3),
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 240,20);
	itsMonoFontSample->SetFont(JFontManager::GetDefaultMonospaceFont());

// end JXLayout

	window->LockCurrentMinSize();

	itsFontMenu->SetFontName(itsTabGroup->GetFont().GetName());
	itsFontMenu->SetToPopupChoice();
	ListenTo(itsFontMenu, std::function([this](const JXFontNameMenu::NameChanged&)
	{
		const JString name = itsFontMenu->GetFontName();
		itsTabGroup->SetFontName(name);
		itsSizeMenu->SetFontName(name);
	}));

	itsSizeMenu->SetToPopupChoice();
	ListenTo(itsSizeMenu, std::function([this](const JXFontSizeMenu::SizeChanged& msg)
	{
		itsTabGroup->SetFontSize(msg.GetSize());
	}));

	itsEdgeRG->SelectItem(itsTabGroup->GetTabEdge());
	ListenTo(itsEdgeRG, std::function([this](const JXRadioGroup::SelectionChanged& msg)
	{
		itsTabGroup->SetTabEdge((JXTabGroup::Edge) msg.GetID());
	}));

	ListenTo(itsAddTabButton);

	input->SetValue(0);

	itsMonoFontSample->GetText()->SetText(JGetString("SampleText::TestTabDirector"));

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
