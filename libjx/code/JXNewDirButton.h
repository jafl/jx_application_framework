/******************************************************************************
 JXNewDirButton.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXNewDirButton
#define _H_JXNewDirButton

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextButton.h>

class JXImage;

class JXNewDirButton : public JXTextButton
{
public:

	JXNewDirButton(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXNewDirButton();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

private:

	JXImage*	itsFolderIcon;
	JSize		itsLabelWidth;

private:

	// not allowed

	JXNewDirButton(const JXNewDirButton& source);
	const JXNewDirButton& operator=(const JXNewDirButton& source);
};

#endif
