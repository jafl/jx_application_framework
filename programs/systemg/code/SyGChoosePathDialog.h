/******************************************************************************
 SyGChoosePathDialog.h

	Interface for the SyGChoosePathDialog class

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SyGChoosePathDialog
#define _H_SyGChoosePathDialog

#include <JXChoosePathDialog.h>

class SyGChoosePathDialog : public JXChoosePathDialog
{
public:

	SyGChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JString& fileFilter,
					   const JBoolean selectOnlyWritable);

	virtual ~SyGChoosePathDialog();

	void		BuildWindow(const JBoolean newWindow, const JString& message = JString::empty);

	JBoolean	OpenInNewWindow() const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

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
