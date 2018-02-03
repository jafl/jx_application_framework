/******************************************************************************
 GMailboxPathDialog.h

	Interface for the GMailboxPathDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GMailboxPathDialog
#define _H_GMailboxPathDialog

#include <JXChoosePathDialog.h>

class JXRadioGroup;

class GMailboxPathDialog : public JXChoosePathDialog
{
public:

	GMailboxPathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JCharacter* fileFilter,
					   const JBoolean showHidden, const JBoolean selectOnlyWritable);

	virtual ~GMailboxPathDialog();

	virtual void	BuildWindow(const JBoolean showHidden,
								const JCharacter* message = NULL);

	JBoolean		IsRelative();

private:

	JXRadioGroup* itsFormatRG;

private:

	// not allowed

	GMailboxPathDialog(const GMailboxPathDialog& source);
	const GMailboxPathDialog& operator=(const GMailboxPathDialog& source);
};

#endif
