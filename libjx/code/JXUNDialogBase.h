/******************************************************************************
 JXUNDialogBase.h

	Interface for the JXUNDialogBase class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUNDialogBase
#define _H_JXUNDialogBase

#include "JXDialogDirector.h"
#include "JXPM.h"

class JXWindow;
class JXStaticText;
class JXImageWidget;

class JXUNDialogBase : public JXDialogDirector
{
public:

	JXUNDialogBase(JXDirector* supervisor);

	virtual ~JXUNDialogBase();

protected:

	void	Init(JXWindow* window, JXStaticText* text, const JString& message,
				 JXImageWidget* icon, const JXPM& xpm);
};

#endif
