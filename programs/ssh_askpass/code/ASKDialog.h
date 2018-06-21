/******************************************************************************
 ASKDialog.h

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#ifndef _H_ASKDialog
#define _H_ASKDialog

#include <JXGetStringDialog.h>

class JXPasswordInput;

class ASKDialog : public JXGetStringDialog
{
public:

	ASKDialog(JXDirector* supervisor);

	virtual	~ASKDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	// not allowed

	ASKDialog(const ASKDialog& source);
	const ASKDialog& operator=(const ASKDialog& source);
};

#endif
