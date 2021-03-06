/******************************************************************************
 CMRegistersDir.h

	Copyright (C) 2011 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMRegistersDir
#define _H_CMRegistersDir

#include <JXWindowDirector.h>

class JXTextMenu;
class JXStaticText;
class CMGetRegisters;
class CMCommandDirector;

class CMRegistersDir : public JXWindowDirector
{
public:

	CMRegistersDir(CMCommandDirector* supervisor);

	virtual	~CMRegistersDir();

	virtual void			Activate();
	virtual JBoolean		Deactivate();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

	void	Update(const JCharacter* data);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMCommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	CMGetRegisters*		itsCmd;			// can be NULL
	JBoolean			itsShouldUpdateFlag;
	JBoolean			itsNeedsUpdateFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JCharacter* binaryName);
	void	Update();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMRegistersDir(const CMRegistersDir& source);
	const CMRegistersDir& operator=(const CMRegistersDir& source);
};

#endif
