/******************************************************************************
 JXGetNewDirDialog.h

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#include "jx-af/jx/JXGetStringDialog.h"
#include <jx-af/jcore/JString.h>

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
};

#endif
