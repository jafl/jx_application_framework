/******************************************************************************
 JXSearchTextButton.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSearchTextButton
#define _H_JXSearchTextButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXButton.h>

class JXSearchTextButton : public JXButton
{
public:

	JXSearchTextButton(const JBoolean forward, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~JXSearchTextButton();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

private:

	const JBoolean itsFwdFlag;

private:

	// not allowed

	JXSearchTextButton(const JXSearchTextButton& source);
	const JXSearchTextButton& operator=(const JXSearchTextButton& source);
};

#endif
