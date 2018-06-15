/******************************************************************************
 JXFileNameDisplay.h

	Interface for the JXFileNameDisplay class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileNameDisplay
#define _H_JXFileNameDisplay

#include "JXStaticText.h"

class JXFileNameDisplay : public JXStaticText
{
public:

	JXFileNameDisplay(const JString& text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~JXFileNameDisplay();

protected:

	virtual void		ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void		BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual JCoordinate	GetFTCMinContentSize(const JBoolean horizontal) const override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	JXFileNameDisplay(const JXFileNameDisplay& source);
	const JXFileNameDisplay& operator=(const JXFileNameDisplay& source);
};

#endif
