/******************************************************************************
 JXPGDirectorBase.h

	Interface for the JXPGDirectorBase class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPGDirectorBase
#define _H_JXPGDirectorBase

#include "jx-af/jx/JXWindowDirector.h"

class JXWindow;
class JXStaticText;
class JXButton;

class JXPGDirectorBase : public JXWindowDirector
{
public:

	JXPGDirectorBase(JXDirector* supervisor);

	virtual ~JXPGDirectorBase();

	void	ProcessContinuing(const JString& value);
	void	ProcessFinished();

	virtual void		Activate();
	virtual bool	Close();

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JString& message,
				 const bool allowCancel, JXButton* cancelButton);

private:

	bool	itsExpectsCloseFlag;
	JXButton*	itsCancelButton;
};

#endif
