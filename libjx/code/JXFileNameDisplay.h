/******************************************************************************
 JXFileNameDisplay.h

	Interface for the JXFileNameDisplay class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileNameDisplay
#define _H_JXFileNameDisplay

#include "jx-af/jx/JXStaticText.h"

class JXFileNameDisplay : public JXStaticText
{
public:

	JXFileNameDisplay(const JString& text, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~JXFileNameDisplay() override;

protected:

	void		ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void		BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;
	void		Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
