/******************************************************************************
 JXGetNewDirDialog.h

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXGetStringDialog.h>
#include <JString.h>

class JXGetNewDirDialog : public JXGetStringDialog
{
public:

	JXGetNewDirDialog(JXDirector* supervisor, const JCharacter* windowTitle,
					  const JCharacter* prompt, const JCharacter* initialName,
					  const JCharacter* basePath, const JBoolean modal = kJTrue);

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
