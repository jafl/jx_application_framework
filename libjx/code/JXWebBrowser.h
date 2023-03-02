/******************************************************************************
 JXWebBrowser.h

	Copyright (C) 2000-2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWebBrowser
#define _H_JXWebBrowser

#include <jx-af/jcore/JWebBrowser.h>
#include "JXSharedPrefObject.h"

class JXWebBrowser : public JWebBrowser, public JXSharedPrefObject
{
public:

	JXWebBrowser();

	~JXWebBrowser() override;

	void	EditPrefs();

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output, const JFileVersion vers) const override;

protected:

	void	SaveCommands() override;

private:

	bool	itsSaveChangesFlag;		// optimization
};

#endif
