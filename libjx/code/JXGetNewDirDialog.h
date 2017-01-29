/******************************************************************************
 JXGetNewDirDialog.h

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#include <JXGetStringDialog.h>
#include <JString.h>

class JXGetNewDirDialog : public JXGetStringDialog
{
public:

	JXGetNewDirDialog(JXDirector* supervisor, const JString& windowTitle,
					  const JString& prompt, const JString& initialName,
					  const JString& basePath, const JBoolean modal = kJTrue);

	virtual ~JXGetNewDirDialog();

	JString	GetNewDirName() const;

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JString	itsBasePath;

private:

	// not allowed

	JXGetNewDirDialog(const JXGetNewDirDialog& source);
	const JXGetNewDirDialog& operator=(const JXGetNewDirDialog& source);
};

#endif
