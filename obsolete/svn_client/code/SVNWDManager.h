/******************************************************************************
 SVNWDManager.h

	Copyright (C) 1997-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNWDManager
#define _H_SVNWDManager

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

	JBoolean	RestoreState(std::istream& input);
	JBoolean	SaveState(std::ostream& output) const;

private:

	// not allowed

	SVNWDManager(const SVNWDManager& source);
	const SVNWDManager& operator=(const SVNWDManager& source);
};

#endif
