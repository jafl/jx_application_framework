/******************************************************************************
 JXNewDirButton.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXNewDirButton
#define _H_JXNewDirButton

#include "jx-af/jx/JXTextButton.h"

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

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	JXImage*	itsFolderIcon;	// not owned
	JSize		itsLabelWidth;
};

#endif
