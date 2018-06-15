/******************************************************************************
 JXSearchTextButton.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSearchTextButton
#define _H_JXSearchTextButton

#include "JXButton.h"

class JXSearchTextButton : public JXButton
{
public:

	JXSearchTextButton(const JBoolean forward, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXSearchTextButton();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	const JBoolean itsFwdFlag;

private:

	// not allowed

	JXSearchTextButton(const JXSearchTextButton& source);
	const JXSearchTextButton& operator=(const JXSearchTextButton& source);
};

#endif
