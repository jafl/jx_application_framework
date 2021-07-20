/******************************************************************************
 THXBaseConvDirector.cpp

	Window to convert non-negative integers from one base to another.
	Since we only support four bases, we list all of them.

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "THXBaseConvDirector.h"
#include "thxGlobals.h"
#include <JXHelpManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXImage.h>
#include <JRegex.h>
#include <JString.h>
#include <jAssert.h>

static const JString::Base kBase[] =
{
	JString::kBase2, JString::kBase8, JString::kBase10, JString::kBase16
};

/******************************************************************************
 Constructor

 ******************************************************************************/

THXBaseConvDirector::THXBaseConvDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	itsIgnoreTextFlag(false)
{
	BuildWindow();
}

THXBaseConvDirector::THXBaseConvDirector
	(
	std::istream&		input,
	const JFileVersion	vers,
	JXDirector*			supervisor
	)
	:
	JXWindowDirector(supervisor),
	itsIgnoreTextFlag(false)
{
	BuildWindow();

	if (vers <= 8)
		{
		JSize fromBase, toBase;
		input >> fromBase >> toBase;

		JString fromValue;
		input >> fromValue;

		switch (fromBase)
			{
			case  2:  its2Input->GetText()->SetText(fromValue); break;
			case  8:  its8Input->GetText()->SetText(fromValue); break;
			case 10: its10Input->GetText()->SetText(fromValue); break;
			case 16: its16Input->GetText()->SetText(fromValue); break;
			}
		}
	else
		{
		JString s;
		input >> s;
		its10Input->GetText()->SetText(s);
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);

	bool active;
	input >> JBoolFromString(active);
	if (active)
		{
		Activate();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXBaseConvDirector::~THXBaseConvDirector()
{
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
THXBaseConvDirector::WriteState
	(
	std::ostream& output
	)
	const
{
	output << its10Input->GetText()->GetText();

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << JBoolToString(IsActive());
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_base_conv_window.xpm"

void
THXBaseConvDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 180,160, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 100,130, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::THXBaseConvDirector::shortcuts::JXLayout"));

	its10Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,20, 80,20);
	assert( its10Input != nullptr );

	auto* base10Label =
		jnew JXStaticText(JGetString("base10Label::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 60,20);
	assert( base10Label != nullptr );
	base10Label->SetToLabel();

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::THXBaseConvDirector::shortcuts::JXLayout"));

	its2Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,50, 80,20);
	assert( its2Input != nullptr );

	auto* base2Label =
		jnew JXStaticText(JGetString("base2Label::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 60,20);
	assert( base2Label != nullptr );
	base2Label->SetToLabel();

	its8Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,70, 80,20);
	assert( its8Input != nullptr );

	auto* base8Label =
		jnew JXStaticText(JGetString("base8Label::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 60,20);
	assert( base8Label != nullptr );
	base8Label->SetToLabel();

	its16Input =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,90, 80,20);
	assert( its16Input != nullptr );

	auto* base16Label =
		jnew JXStaticText(JGetString("base16Label::THXBaseConvDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 60,20);
	assert( base16Label != nullptr );
	base16Label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::THXBaseConvDirector"));
	window->SetWMClass(THXGetWMClassInstance(), THXGetBaseConvWindowClass());
	window->LockCurrentMinSize();
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	window->PlaceAsDialogWindow();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, thx_base_conv_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	ListenTo(its2Input);
	ListenTo(its8Input);
	ListenTo(its10Input);
	ListenTo(its16Input);

	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);

	// remember to update kBase[]

	itsInput[0] = its2Input;
	itsInput[1] = its8Input;
	itsInput[2] = its10Input;
	itsInput[3] = its16Input;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
THXBaseConvDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		GetWindow()->Close();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("THXBaseConvHelp");
		}

	else
		{
		if (!itsIgnoreTextFlag &&
			(message.Is(JStyledText::kTextSet) ||
			 message.Is(JStyledText::kTextChanged)))
			{
			itsIgnoreTextFlag = true;
			Convert(sender);
			itsIgnoreTextFlag = false;
			}

		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Convert (private)

 ******************************************************************************/

void
THXBaseConvDirector::Convert
	(
	JBroadcaster* sender
	)
{
	JXInputField* input = nullptr;
	JSize base          = 0;

	for (JUnsignedOffset i=0; i<kTHXBaseCount; i++)
		{
		if (sender == itsInput[i])
			{
			input = itsInput[i];
			base  = kBase[i];
			break;
			}
		}

	if (input == nullptr)
		{
		return;
		}

	JUInt value;
	if (!input->GetText()->GetText().ConvertToUInt(&value, base))
		{
		for (JUnsignedOffset i=0; i<kTHXBaseCount; i++)
			{
			if (itsInput[i] != input)
				{
				itsInput[i]->GetText()->SetText(JString::empty);
				}
			}
		return;
		}

	for (JUnsignedOffset i=0; i<kTHXBaseCount; i++)
		{
		if (itsInput[i] != input)
			{
			const JString s(value, kBase[i], true);
			itsInput[i]->GetText()->SetText(s);
			}
		}
}
