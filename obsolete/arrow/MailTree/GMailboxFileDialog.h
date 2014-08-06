/******************************************************************************
 GMailboxFileDialog.h

	Interface for the GMailboxFileDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMailboxFileDialog
#define _H_GMailboxFileDialog

#include <JXChooseFileDialog.h>

class JXRadioGroup;

class GMailboxFileDialog : public JXChooseFileDialog
{
public:

	GMailboxFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JCharacter* fileFilter, const JBoolean showHidden);

	virtual ~GMailboxFileDialog();

	virtual void	BuildWindow(const JCharacter* origName, const JCharacter* message);

	JBoolean		IsRelative();

private:

	JXRadioGroup* itsFormatRG;

private:

	// not allowed

	GMailboxFileDialog(const GMailboxFileDialog& source);
	const GMailboxFileDialog& operator=(const GMailboxFileDialog& source);
};

#endif
