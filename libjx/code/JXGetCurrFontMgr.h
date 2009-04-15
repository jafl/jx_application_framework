/******************************************************************************
 JXGetCurrFontMgr.h

	Interface for the JXGetCurrFontMgr class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetCurrFontMgr
#define _H_JXGetCurrFontMgr

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
