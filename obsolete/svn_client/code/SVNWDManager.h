/******************************************************************************
 SVNWDManager.h

	Copyright © 1997-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNWDManager
#define _H_SVNWDManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWDManager.h>

class SVNMainDirector;

class SVNWDManager : public JXWDManager
{
public:

	SVNWDManager(JXDisplay* display, const JBoolean wantShortcuts);

	virtual ~SVNWDManager();

	JBoolean			NewBrowser(SVNMainDirector** dir);
	JBoolean			GetBrowser(const JCharacter* path, SVNMainDirector** dir);
	JBoolean			GetBrowserForExactURL(const JCharacter* url, SVNMainDirector** dir);
	SVNMainDirector*	OpenDirectory(const JCharacter* path, JBoolean* wasOpen = NULL);

	// called by CBPrefsManager

	JBoolean	RestoreState(istream& input);
	JBoolean	SaveState(ostream& output) const;

private:

	// not allowed

	SVNWDManager(const SVNWDManager& source);
	const SVNWDManager& operator=(const SVNWDManager& source);
};

#endif
