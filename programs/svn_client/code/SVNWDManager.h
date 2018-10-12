/******************************************************************************
 SVNWDManager.h

	Copyright (C) 1997-2008 by John Lindal.

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
	JBoolean			GetBrowser(const JString& path, SVNMainDirector** dir);
	JBoolean			GetBrowserForExactURL(const JString& url, SVNMainDirector** dir);
	SVNMainDirector*	OpenDirectory(const JString& path, JBoolean* wasOpen = nullptr);

	// called by CBPrefsManager

	JBoolean	RestoreState(std::istream& input);
	JBoolean	SaveState(std::ostream& output) const;

private:

	// not allowed

	SVNWDManager(const SVNWDManager& source);
	const SVNWDManager& operator=(const SVNWDManager& source);
};

#endif
