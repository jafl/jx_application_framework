/******************************************************************************
 CMStackDir.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_CMStackDir
#define _H_CMStackDir

#include <JXWindowDirector.h>

class JXTextMenu;
class CMCommandDirector;
class CMStackWidget;

class CMStackDir : public JXWindowDirector
{
public:

	CMStackDir(CMCommandDirector* supervisor);

	virtual	~CMStackDir();

	virtual void			Activate();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

	CMStackWidget*	GetStackWidget();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMCommandDirector*	itsCommandDir;
	CMStackWidget*		itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(CMCommandDirector* supervisor);
	void	UpdateWindowTitle(const JCharacter* binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMStackDir(const CMStackDir& source);
	const CMStackDir& operator=(const CMStackDir& source);
};


/******************************************************************************
 GetStackWidget

 ******************************************************************************/

inline CMStackWidget*
CMStackDir::GetStackWidget()
{
	return itsWidget;
}

#endif
