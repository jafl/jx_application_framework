/******************************************************************************
 JGetCurrentFontManager.h

	Interface for the JGetCurrentFontManager class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JGetCurrentFontManager
#define _H_JGetCurrentFontManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

class JFontManager;

class JGetCurrentFontManager
{
public:

	JGetCurrentFontManager();

	virtual ~JGetCurrentFontManager();

	virtual const JFontManager*	GetCurrFontManager() const = 0;

private:

	// not allowed

	JGetCurrentFontManager(const JGetCurrentFontManager& source);
	const JGetCurrentFontManager& operator=(const JGetCurrentFontManager& source);
};

#endif
