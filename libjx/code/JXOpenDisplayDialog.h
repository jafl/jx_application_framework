/******************************************************************************
 JXOpenDisplayDialog.h

	Copyright (C) 1996-2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXOpenDisplayDialog
#define _H_JXOpenDisplayDialog

#include "JXGetStringDialog.h"

class JXOpenDisplayDialog : public JXGetStringDialog
{
public:

	JXOpenDisplayDialog();

	~JXOpenDisplayDialog() override;

	JIndex	GetDisplayIndex() const;

protected:

	bool	OKToDeactivate() override;

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
