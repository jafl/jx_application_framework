/******************************************************************************
 CMBreakpointsDir.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMBreakpointsDir
#define _H_CMBreakpointsDir

#include <JXWindowDirector.h>

class JXTextMenu;
class JXColHeaderWidget;
class CMCommandDirector;
class CMBreakpointTable;

class CMBreakpointsDir : public JXWindowDirector
{
public:

	CMBreakpointsDir(CMCommandDirector* supervisor);

	virtual	~CMBreakpointsDir();

	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

	CMCommandDirector*	GetCommandDirector();
	CMBreakpointTable*	GetBreakpointTable();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMCommandDirector*	itsCommandDir;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

// begin tablelayout

	CMBreakpointTable* itsTable;
	JXColHeaderWidget* itsColHeader;

// end tablelayout

private:

	void	BuildWindow(CMCommandDirector* supervisor);
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMBreakpointsDir(const CMBreakpointsDir& source);
	const CMBreakpointsDir& operator=(const CMBreakpointsDir& source);
};


/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

inline CMCommandDirector*
CMBreakpointsDir::GetCommandDirector()
{
	return itsCommandDir;
}

/******************************************************************************
 GetBreakpointTable

 ******************************************************************************/

inline CMBreakpointTable*
CMBreakpointsDir::GetBreakpointTable()
{
	return itsTable;
}

#endif
