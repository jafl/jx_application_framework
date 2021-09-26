/******************************************************************************
 JXOpenDisplayDialog.h

	Copyright (C) 1996-2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXOpenDisplayDialog
#define _H_JXOpenDisplayDialog

#include "jx-af/jx/JXGetStringDialog.h"

class JXOpenDisplayDialog : public JXGetStringDialog
{
public:

	JXOpenDisplayDialog(JXWindowDirector* supervisor);

	virtual ~JXOpenDisplayDialog();

	JIndex	GetDisplayIndex() const;

protected:

	virtual bool	OKToDeactivate();

private:

	JIndex	itsDisplayIndex;
};


/******************************************************************************
 GetDisplayIndex

 ******************************************************************************/

inline JIndex
JXOpenDisplayDialog::GetDisplayIndex()
	const
{
	return itsDisplayIndex;
}

#endif
