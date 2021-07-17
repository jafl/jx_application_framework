/******************************************************************************
 JXGetNewDirDialog.h

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#include "JXGetStringDialog.h"
#include <JString.h>

class JXGetNewDirDialog : public JXGetStringDialog
{
public:

	JXGetNewDirDialog(JXDirector* supervisor, const JString& windowTitle,
					  const JString& prompt, const JString& initialName,
					  const JString& basePath, const bool modal = true);

	virtual ~JXGetNewDirDialog();

	JString	GetNewDirName() const;

protected:

	virtual bool	OKToDeactivate();

private:

	JString	itsBasePath;

private:

	// not allowed

	JXGetNewDirDialog(const JXGetNewDirDialog& source);
	const JXGetNewDirDialog& operator=(const JXGetNewDirDialog& source);
};

#endif
