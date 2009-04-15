/******************************************************************************
 JXUNDialogBase.h

	Interface for the JXUNDialogBase class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUNDialogBase
#define _H_JXUNDialogBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JXPM.h>

class JXWindow;
class JXStaticText;
class JXImageWidget;

class JXUNDialogBase : public JXDialogDirector
{
public:

	JXUNDialogBase(JXDirector* supervisor);

	virtual ~JXUNDialogBase();

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JCharacter* message,
				 JXImageWidget* icon, const JXPM& xpm);

private:

	// not allowed

	JXUNDialogBase(const JXUNDialogBase& source);
	const JXUNDialogBase& operator=(const JXUNDialogBase& source);
};

#endif
