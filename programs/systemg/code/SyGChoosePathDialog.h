/******************************************************************************
 SyGChoosePathDialog.h

	Interface for the SyGChoosePathDialog class

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGChoosePathDialog
#define _H_SyGChoosePathDialog

#include <JXChoosePathDialog.h>

class SyGChoosePathDialog : public JXChoosePathDialog
{
public:

	SyGChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JCharacter* fileFilter,
					   const JBoolean selectOnlyWritable);

	virtual ~SyGChoosePathDialog();

	void		BuildWindow(const JBoolean newWindow, const JCharacter* message = NULL);

	JBoolean	OpenInNewWindow() const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXTextButton*   itsSelectButton;
	JXTextCheckbox* itsNewWindowCB;
	JXTextButton*   itsOpenButton;

// end JXLayout

private:

	// not allowed

	SyGChoosePathDialog(const SyGChoosePathDialog& source);
	const SyGChoosePathDialog& operator=(const SyGChoosePathDialog& source);
};

#endif
