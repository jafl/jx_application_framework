/******************************************************************************
 JXSearchTextButton.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSearchTextButton
#define _H_JXSearchTextButton

#include "jx-af/jx/JXButton.h"

class JXSearchTextButton : public JXButton
{
public:

	JXSearchTextButton(const bool forward, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXSearchTextButton();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	const bool itsFwdFlag;
};

#endif
