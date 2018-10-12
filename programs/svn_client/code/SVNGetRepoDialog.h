/******************************************************************************
 SVNGetRepoDialog.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNGetRepoDialog
#define _H_SVNGetRepoDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXInputField;
class JXStringHistoryMenu;

class SVNGetRepoDialog : public JXDialogDirector, public JPrefObject
{
public:

	SVNGetRepoDialog(JXDirector* supervisor, const JString& windowTitle);

	virtual ~SVNGetRepoDialog();

	const JString&	GetRepo() const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXInputField*        itsRepoInput;
	JXStringHistoryMenu* itsRepoHistoryMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle);

	// not allowed

	SVNGetRepoDialog(const SVNGetRepoDialog& source);
	const SVNGetRepoDialog& operator=(const SVNGetRepoDialog& source);
};

#endif
