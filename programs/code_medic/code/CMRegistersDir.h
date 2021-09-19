/******************************************************************************
 CMRegistersDir.h

	Copyright (C) 2011 by John Lindal.

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

	virtual void			Activate() override;
	virtual bool		Deactivate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

	void	Update(const JString& data);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMCommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	CMGetRegisters*		itsCmd;			// can be nullptr
	bool				itsShouldUpdateFlag;
	bool				itsNeedsUpdateFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);
	void	Update();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMRegistersDir(const CMRegistersDir& source);
	const CMRegistersDir& operator=(const CMRegistersDir& source);
};

#endif
