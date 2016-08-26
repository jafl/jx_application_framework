/******************************************************************************
 JXWebBrowser.h

	Copyright © 2000-2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWebBrowser
#define _H_JXWebBrowser

#include <JWebBrowser.h>
#include <JXSharedPrefObject.h>

class JXEditWWWPrefsDialog;

class JXWebBrowser : public JWebBrowser, public JXSharedPrefObject
{
public:

	JXWebBrowser();

	virtual ~JXWebBrowser();

	void	EditPrefs();

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output, const JFileVersion vers) const;

protected:

	virtual void	SaveCommands();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXEditWWWPrefsDialog*	itsPrefsDialog;
	JBoolean				itsSaveChangesFlag;		// optimization

private:

	// not allowed

	JXWebBrowser(const JXWebBrowser& source);
	const JXWebBrowser& operator=(const JXWebBrowser& source);
};

#endif
