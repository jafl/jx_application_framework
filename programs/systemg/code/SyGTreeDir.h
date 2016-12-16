/******************************************************************************
 SyGTreeDir.h

	Interface for the SyGTreeDir class

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGTreeDir
#define _H_SyGTreeDir

#include <JXWindowDirector.h>

class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class JXTextButton;

class SyGTreeSet;
class SyGFileTreeTable;
class SyGFolderDragSource;
class SyGPathInput;

class SyGTreeDir : public JXWindowDirector
{
public:

	SyGTreeDir(const JCharacter* startPath);

	virtual ~SyGTreeDir();

	virtual JBoolean		Close();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

	const JString&		GetDirectory() const;
	SyGTreeSet*			GetTreeSet() const;
	SyGFileTreeTable*	GetTable() const;

	void	SaveState();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SyGTreeSet*	itsTreeSet;
	JBoolean	itsAutoMountFlag;	// kJTrue => unmount when close

	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	JXToolBar*           itsToolBar;
	SyGPathInput*        itsPathInput;
	SyGFolderDragSource* itsDragSrc;
	JXTextButton*        itsUpButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* startPath);
	void	WriteState(std::ostream& output);

	void	HandleHelpMenu(const JIndex index);
	void	HandlePrefsMenu(const JIndex index);

	// not allowed

	SyGTreeDir(const SyGTreeDir& source);
	const SyGTreeDir& operator=(const SyGTreeDir& source);
};


/******************************************************************************
 GetTreeSet

 ******************************************************************************/

inline SyGTreeSet*
SyGTreeDir::GetTreeSet()
	const
{
	return itsTreeSet;
}

#endif
