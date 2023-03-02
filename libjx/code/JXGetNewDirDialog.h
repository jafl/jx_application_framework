/******************************************************************************
 JXGetNewDirDialog.h

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#include "JXGetStringDialog.h"
#include <jx-af/jcore/JString.h>

class JXGetNewDirDialog : public JXGetStringDialog
{
public:

	JXGetNewDirDialog(const JString& windowTitle, const JString& prompt,
					  const JString& initialName, const JString& basePath);

	~JXGetNewDirDialog() override;

	JString	GetNewDirName() const;

protected:

	bool	OKToDeactivate() override;

private:

	JString	itsBasePath;
};

#endif
