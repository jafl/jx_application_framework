/******************************************************************************
 JXGetCurrFontMgr.h

	Interface for the JXGetCurrFontMgr class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXGetCurrFontMgr
#define _H_JXGetCurrFontMgr

#include <JGetCurrentFontManager.h>

class JXGetCurrFontMgr : public JGetCurrentFontManager
{
public:

	JXGetCurrFontMgr();

	virtual ~JXGetCurrFontMgr();

	virtual const JFontManager*	GetCurrFontManager() const;

private:

	// not allowed

	JXGetCurrFontMgr(const JXGetCurrFontMgr& source);
	const JXGetCurrFontMgr& operator=(const JXGetCurrFontMgr& source);
};

#endif
