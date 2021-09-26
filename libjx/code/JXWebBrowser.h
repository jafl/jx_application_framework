/******************************************************************************
 JXWebBrowser.h

	Copyright (C) 2000-2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWebBrowser
#define _H_JXWebBrowser

#include <jx-af/jcore/JWebBrowser.h>
#include "jx-af/jx/JXSharedPrefObject.h"

class JXEditWWWPrefsDialog;

class JXWebBrowser : public JWebBrowser, public JXSharedPrefObject
{
public:

	JXWebBrowser();

	virtual ~JXWebBrowser();

	void	EditPrefs();

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output, const JFileVersion vers) const override;

protected:

	virtual void	SaveCommands() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXEditWWWPrefsDialog*	itsPrefsDialog;
	bool					itsSaveChangesFlag;		// optimization
};

#endif
