/******************************************************************************
 CMLocalVarsDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMLocalVarsDir
#define _H_CMLocalVarsDir

#include <JXWindowDirector.h>

class JTree;
class JXTextButton;
class JXTextMenu;
class CMLink;
class CMVarTreeWidget;
class CMCommandDirector;
class CMGetLocalVars;

class CMLocalVarsDir : public JXWindowDirector
{
public:

public:

	CMLocalVarsDir(CMCommandDirector* supervisor);

	virtual	~CMLocalVarsDir();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JTree*				itsTree;		// owned by itsWidget
	CMVarTreeWidget*	itsWidget;
	CMGetLocalVars*		itsGetLocalsCmd;
	bool			itsNeedsUpdateFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);

	void	Update();
	void	Rebuild();
	void	FlushOldData();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMLocalVarsDir(const CMLocalVarsDir& source);
	const CMLocalVarsDir& operator=(const CMLocalVarsDir& source);
};

#endif
