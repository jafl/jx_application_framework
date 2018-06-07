/******************************************************************************
 THXBaseConvDirector.cpp

	Window to convert non-negative integers from one base to another.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXBaseConvDirector.h"
#include "THXBaseConvMenu.h"
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
#include <jStrStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

THXBaseConvDirector::THXBaseConvDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

THXBaseConvDirector::THXBaseConvDirector
	(
	std::istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();

	JSize fromBase, toBase;
	input >> fromBase >> toBase;
	itsFromBase->SetBase(fromBase);
	itsToBase->SetBase(toBase);

	JString fromValue;
	input >> fromValue;
	itsFromValue->SetText(fromValue);

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
	std::ostream& output
	)
	const
{
	output << itsFromBase->GetBase();
	output << ' ' << itsToBase->GetBase();
	output << ' ' << itsFromValue->GetText();

	output << ' ';
	GetWindow()->WriteGeometry(output);

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

	JXWindow* window = jnew JXWindow(this, 300,130, "");
	assert( window != nullptr );
	SetWindow(window);

	itsCloseButton =
		jnew JXTextButton("Close", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 180,100, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts("#W^[");

	itsFromValue =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 70,20);
	assert( itsFromValue != nullptr );

	JXStaticText* obj1 =
		jnew JXStaticText("Convert from:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 90,20);
	assert( obj1 != nullptr );

	itsHelpButton =
		jnew JXTextButton("Help", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,100, 60,20);
	assert( itsHelpButton != nullptr );

	itsFromBase =
		jnew THXBaseConvMenu(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,15, 90,30);
	assert( itsFromBase != nullptr );

	JXStaticText* obj2 =
		jnew JXStaticText("Result:", window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 90,20);
	assert( obj2 != nullptr );

	itsToBase =
		jnew THXBaseConvMenu(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,55, 90,30);
	assert( itsToBase != nullptr );

	itsToValue =
		jnew JXStaticText("", kJFalse, kJTrue, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,60, 70,20);
	assert( itsToValue != nullptr );

// end JXLayout

	window->SetTitle("Base Conversion");
	window->SetWMClass(THXGetWMClassInstance(), THXGetBaseConvWindowClass());
	window->LockCurrentMinSize();
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(kJTrue);
	window->PlaceAsDialogWindow();

	JXDisplay* display = GetDisplay();
	JXImage* icon      = jnew JXImage(display, display->GetColormap(), thx_base_conv_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsFromValue->ShouldBroadcastAllTextChanged(kJTrue);		// want every keypress

	ListenTo(itsFromValue);
	ListenTo(itsFromBase);
	ListenTo(itsToBase);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
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
		(JXGetHelpManager())->ShowSection(kTHXBaseConvHelpName);
		}

	else if (sender == itsFromValue &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		Convert();
		}
	else if ((sender == itsFromBase || sender == itsToBase) &&
			 message.Is(THXBaseConvMenu::kBaseChanged))
		{
		Convert();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Convert (private)

 ******************************************************************************/

static const JRegex hexRegex = "^[[:space:]]*0[xX]";

void
THXBaseConvDirector::Convert()
{
	const JInteger fromBase = itsFromBase->GetBase();

	const JString& fromStr = itsFromValue->GetText();
	if (fromBase != 16 && hexRegex.Match(fromStr))
		{
		itsFromBase->SetBase(16);	// calls us again
		return;
		}

	JUInt value;
	if (!fromStr.ConvertToUInt(&value, fromBase))
		{
		itsToValue->SetText("");
		return;
		}

	JString toValue;
	const JInteger toBase = itsToBase->GetBase();
	if (value == 0)
		{
		toValue = "0";
		}
	else if (toBase == 2)
		{
		do
			{
			if (value & 0x01)
				{
				toValue.PrependCharacter('1');
				}
			else
				{
				toValue.PrependCharacter('0');
				}
			value = value >> 1;
			}
			while (value != 0);
		}
	else
		{
		ostrstream s;
		s << setbase(toBase) << value << ends;
		toValue = s.str();
		JUnfreeze(s);
		}

	if (toBase == 16)
		{
		toValue.ToUpper();
		toValue.Prepend("0x");
		}

	itsToValue->SetText(toValue);
	itsToValue->SelectAll();
}
