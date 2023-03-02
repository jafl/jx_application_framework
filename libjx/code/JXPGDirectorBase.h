/******************************************************************************
 JXPGDirectorBase.h

	Interface for the JXPGDirectorBase class

	Copyright (C) 1997-2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPGDirectorBase
#define _H_JXPGDirectorBase

#include "JXWindowDirector.h"

class JXWindow;
class JXStaticText;
class JXButton;

class JXPGDirectorBase : public JXWindowDirector
{
public:

	JXPGDirectorBase(const bool modal);

	~JXPGDirectorBase() override;

	void	ProcessContinuing(const JString& value);
	void	ProcessFinished();

	void	Activate() override;
	bool	Close() override;

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JString& message,
				 const bool allowCancel, JXButton* cancelButton);

private:

	const bool	itsModalFlag;
	bool		itsExpectsCloseFlag;
	JXButton*	itsCancelButton;
};

#endif
