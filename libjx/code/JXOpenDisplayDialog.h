/******************************************************************************
 JXOpenDisplayDialog.h

	Copyright (C) 1996-2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXOpenDisplayDialog
#define _H_JXOpenDisplayDialog

#include <JXGetStringDialog.h>

class JXOpenDisplayDialog : public JXGetStringDialog
{
public:

	JXOpenDisplayDialog(JXWindowDirector* supervisor);

	virtual ~JXOpenDisplayDialog();

	JIndex	GetDisplayIndex() const;

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JIndex	itsDisplayIndex;

private:

	// not allowed

	JXOpenDisplayDialog(const JXOpenDisplayDialog& source);
	const JXOpenDisplayDialog& operator=(const JXOpenDisplayDialog& source);
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
