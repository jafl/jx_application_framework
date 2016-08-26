/******************************************************************************
 CMVarTreeDir.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMVarTreeDir
#define _H_CMVarTreeDir

#include <JXWindowDirector.h>

class JTree;
class JXTextButton;
class JXTextMenu;
class CMLink;
class CMVarTreeWidget;
class CMCommandDirector;

class CMVarTreeDir : public JXWindowDirector
{
public:

public:

	CMVarTreeDir(CMCommandDirector* supervisor);

	virtual	~CMVarTreeDir();

	void	DisplayExpression(const JCharacter* expr);

	virtual void			Activate();
	virtual JBoolean		Deactivate();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JTree*				itsTree;		// owned by itsWidget
	CMVarTreeWidget*	itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JCharacter* binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMVarTreeDir(const CMVarTreeDir& source);
	const CMVarTreeDir& operator=(const CMVarTreeDir& source);
};

#endif
