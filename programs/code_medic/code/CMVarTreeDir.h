/******************************************************************************
 CMVarTreeDir.h

	Copyright (C) 2001 by John Lindal.

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

	void	DisplayExpression(const JString& expr);

	virtual void			Activate() override;
	virtual bool		Deactivate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

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
	void	UpdateWindowTitle(const JString& binaryName);

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
