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

	SVNWDManager(JXDisplay* display, const bool wantShortcuts);

	virtual ~SVNWDManager();

	bool			NewBrowser(SVNMainDirector** dir);
	bool			GetBrowser(const JString& path, SVNMainDirector** dir);
	bool			GetBrowserForExactURL(const JString& url, SVNMainDirector** dir);
	SVNMainDirector*	OpenDirectory(const JString& path, bool* wasOpen = nullptr);

	// called by CBPrefsManager

	bool	RestoreState(std::istream& input);
	bool	SaveState(std::ostream& output) const;

private:

	// not allowed

	SVNWDManager(const SVNWDManager& source);
	const SVNWDManager& operator=(const SVNWDManager& source);
};

#endif
