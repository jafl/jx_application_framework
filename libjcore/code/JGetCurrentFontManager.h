/******************************************************************************
 JGetCurrentFontManager.h

	Interface for the JGetCurrentFontManager class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JGetCurrentFontManager
#define _H_JGetCurrentFontManager

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
