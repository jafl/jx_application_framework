/******************************************************************************
 JXHelpManager.h

	Copyright (C) 1997-2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXHelpManager
#define _H_JXHelpManager

#include <jTypes.h>

class JXHelpManager
{
public:

	JXHelpManager();

	virtual ~JXHelpManager();

	void	ShowTOC();
	void	ShowSection(const JUtf8Byte* name);
	void	ShowChangeLog();
	void	ShowCredits();

private:

	// not allowed

	JXHelpManager(const JXHelpManager& source);
	const JXHelpManager& operator=(const JXHelpManager& source);
};

#endif
