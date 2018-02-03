/******************************************************************************
 GMDummyText.h

	Interface for the GMDummyText class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMDummyText
#define _H_GMDummyText


#include <JXStaticText.h>

class JXPTPrinter;

class GMDummyText : public JXStaticText
{
public:

public:

	GMDummyText(JXPTPrinter* printer,
				const JCharacter* text, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMDummyText();

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	JXPTPrinter*	itsPTPrinter;

private:

	// not allowed

	GMDummyText(const GMDummyText& source);
	const GMDummyText& operator=(const GMDummyText& source);

public:
};

#endif
