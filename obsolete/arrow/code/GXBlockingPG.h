/******************************************************************************
 GXBlockingPG.h

	Interface for the GXBlockingPG class.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_GXBlockingPG
#define _H_GXBlockingPG

#include <JXProgressDisplay.h>

class JXWindow;
class JXProgressIndicator;

class GXBlockingPG : public JXProgressDisplay
{
public:

	GXBlockingPG(JXTextButton* cancelButton, JXStaticText* counter,
				 JXProgressIndicator* indicator);

	virtual ~GXBlockingPG();

	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();

protected:

	virtual void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JCharacter* message,
									 const JBoolean allowCancel,
									 const JBoolean allowBackground);

private:

	JXWindow*	itsWindow;

private:

	// not allowed

	GXBlockingPG(const GXBlockingPG& source);
	const GXBlockingPG& operator=(const GXBlockingPG& source);
};

#endif
