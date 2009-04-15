/******************************************************************************
 JXPGDirectorBase.h

	Interface for the JXPGDirectorBase class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPGDirectorBase
#define _H_JXPGDirectorBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXWindow;
class JXStaticText;
class JXButton;

class JXPGDirectorBase : public JXWindowDirector
{
public:

	JXPGDirectorBase(JXDirector* supervisor);

	virtual ~JXPGDirectorBase();

	void	ProcessContinuing(const JCharacter* value);
	void	ProcessFinished();

	virtual void		Activate();
	virtual JBoolean	Close();

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JCharacter* message,
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
