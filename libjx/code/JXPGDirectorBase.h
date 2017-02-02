/******************************************************************************
 JXPGDirectorBase.h

	Interface for the JXPGDirectorBase class

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPGDirectorBase
#define _H_JXPGDirectorBase

#include <JXWindowDirector.h>

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
	virtual JBoolean	Close();

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JString& message,
				 const JBoolean allowCancel, JXButton* cancelButton);

private:

	JBoolean	itsExpectsCloseFlag;
	JXButton*	itsCancelButton;

private:

	// not allowed

	JXPGDirectorBase(const JXPGDirectorBase& source);
	const JXPGDirectorBase& operator=(const JXPGDirectorBase& source);
};

#endif
