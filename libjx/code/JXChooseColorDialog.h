/******************************************************************************
 JXChooseColorDialog.h

	Interface for the JXChooseColorDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseColorDialog
#define _H_JXChooseColorDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <jColor.h>

class JXSlider;
class JXIntegerInput;
class JXFlatRect;

class JXChooseColorDialog : public JXDialogDirector
{
public:

	JXChooseColorDialog(JXWindowDirector* supervisor, const JColorIndex colorIndex);

	virtual ~JXChooseColorDialog();

	JColorIndex	GetColor() const;

protected:

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

    JXSlider*       itsHSlider;
    JXSlider*       itsSSlider;
    JXSlider*       itsVSlider;
    JXFlatRect*     itsColorSample;
    JXIntegerInput* itsRInput;
    JXIntegerInput* itsGInput;
    JXIntegerInput* itsBInput;

// end JXLayout

private:

	void	BuildWindow(const JColorIndex colorIndex);
	void	UpdateColor(const JBoolean sliderChanged);

	// not allowed

	JXChooseColorDialog(const JXChooseColorDialog& source);
	const JXChooseColorDialog& operator=(const JXChooseColorDialog& source);
};

#endif
