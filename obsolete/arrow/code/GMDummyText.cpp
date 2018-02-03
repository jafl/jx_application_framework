/******************************************************************************
 GMDummyText.cc

	BASE CLASS = public JXStaticText

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <GMDummyText.h>
#include <GMGlobals.h>

#include <JXPTPrinter.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GMDummyText::GMDummyText
	(
	JXPTPrinter*		printer,
	const JCharacter*	text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
   JXStaticText(text, enclosure, hSizing, vSizing, x, y, w, h),
   itsPTPrinter(printer)
{
	SetPTPrinter(itsPTPrinter);
	Hide();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMDummyText::~GMDummyText()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GMDummyText::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPTPrinter && message.Is(JPrinter::kPrintSetupFinished))
		{
		JXStaticText::Receive(sender, message);
		SetText("");
		}

	else
		{
		JXStaticText::Receive(sender, message);
		}

}
