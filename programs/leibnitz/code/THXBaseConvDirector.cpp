/******************************************************************************
 THXBaseConvDirector.cpp

	Window to convert non-negative integers from one base to another.
	Since we only support four bases, we list all of them.

	BASE CLASS = JXWindowDirector

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <thxStdInc.h>
#include "THXBaseConvDirector.h"
#include "thxGlobals.h"
#include "thxHelpText.h"
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
	itsIgnoreTextFlag(kJFalse)
{
	BuildWindow();
}

THXBaseConvDirector::THXBaseConvDirector
	(
	istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor
	)
	:
	JXWindowDirector(supervisor),
	itsIgnoreTextFlag(kJFalse)
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
			case  2:  its2Input->SetText(fromValue); break;
			case  8:  its8Input->SetText(fromValue); break;
			case 10: its10Input->SetText(fromValue); break;
			case 16: its16Input->SetText(fromValue); break;
			}
		}
	else
		{
		JString s;
		input >> s;
		its10Input->SetText(s);
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);

	JBoolean active;
	input >> active;
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
	ostream& output
	)
	const
{
	output << its10Input->GetText();

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << IsActive();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "thx_base_conv_window.xpm"

void
THXBaseConvDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 180,160, "");
    assert( window != NULL );
    SetWindow(window);

    itsCloseButton =
        new JXTextButton(JGetString("itsCloseButton::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 100,130, 60,20);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts(JGetString("itsCloseButton::THXBaseConvDirector::shortcuts::JXLayout"));

    its10Input =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 80,20, 80,20);
    assert( its10Input != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 60,20);
    assert( obj1_JXLayout != NULL );

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 60,20);
    assert( itsHelpButton != NULL );
    itsHelpButton->SetShortcuts(JGetString("itsHelpButton::THXBaseConvDirector::shortcuts::JXLayout"));

    its2Input =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 80,50, 80,20);
    assert( its2Input != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 60,20);
    assert( obj2_JXLayout != NULL );

    its8Input =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 80,70, 80,20);
    assert( its8Input != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 60,20);
    assert( obj3_JXLayout != NULL );

    its16Input =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 80,90, 80,20);
    assert( its16Input != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::THXBaseConvDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 60,20);
    assert( obj4_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Base Conversion");
	window->SetWMClass(THXGetWMClassInstance(), THXGetBaseConvWindowClass());
	window->LockCurrentMinSize();
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(kJTrue);
	window->PlaceAsDialogWindow();

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, display->GetColormap(), thx_base_conv_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	its2Input->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress
	ListenTo(its2Input);

	its8Input->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress
	ListenTo(its8Input);

	its10Input->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress
	ListenTo(its10Input);

	its16Input->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress
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
		(GetWindow())->Close();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kTHXBaseConvHelpName);
		}

	else
		{
		if (!itsIgnoreTextFlag &&
			(message.Is(JTextEditor::kTextSet) ||
			 message.Is(JTextEditor::kTextChanged)))
			{
			itsIgnoreTextFlag = kJTrue;
			Convert(sender);
			itsIgnoreTextFlag = kJFalse;
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
	JXInputField* input = NULL;
	JSize base          = 0;

	for (JIndex i=0; i<kTHXBaseCount; i++)
		{
		if (sender == itsInput[i])
			{
			input = itsInput[i];
			base  = kBase[i];
			break;
			}
		}

	if (input == NULL)
		{
		return;
		}

	JUInt value;
	if (!(input->GetText()).ConvertToUInt(&value, base))
		{
		for (JIndex i=0; i<kTHXBaseCount; i++)
			{
			if (itsInput[i] != input)
				{
				itsInput[i]->SetText("");
				}
			}
		return;
		}

	for (JIndex i=0; i<kTHXBaseCount; i++)
		{
		if (itsInput[i] != input)
			{
			const JString s(value, kBase[i], kJTrue);
			itsInput[i]->SetText(s);
			}
		}
}
