/******************************************************************************
 GFGPrefsManager.h

	Copyright (C) 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGPrefsManager
#define _H_GFGPrefsManager

#include <JXPrefsManager.h>

class JString;
class GFGPrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kGFGProgramVersionID = 1,
	kGFGgCSFSetupID,
	kGFGMainDirectorID,
	kGFGMainToolBarID,
	kGFGHeaderCommentID,
	kGFGAuthorID,
	kGFGCopyrightID,
	kGFGSourceCommentID,
	kGFGConstructorCommentID,
	kGFGDestructorCommentID,
	kGFGFunctionCommentID,
	kGFGYearID,
	kGFGVersionCheckID
};

class GFGPrefsManager : public JXPrefsManager
{
public:

	GFGPrefsManager(JBoolean* isNew);

	virtual	~GFGPrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JString	GetHeaderComment(const JCharacter* classname = NULL) const;
	void	SetHeaderComment(const JCharacter* comment);

	JString	GetAuthor() const;
	void	SetAuthor(const JCharacter* author);

	JString	GetYear() const;
	void	SetYear(const JCharacter* year);

	JString	GetCopyright(const JBoolean replaceVars = kJFalse) const;
	void	SetCopyright(const JCharacter* copyright);

	JString	GetSourceComment(const JCharacter* classname = NULL, const JCharacter* base = NULL) const;
	void	SetSourceComment(const JCharacter* comment);

	JString	GetConstructorComment() const;
	void	SetConstructorComment(const JCharacter* comment);

	JString	GetDestructorComment() const;
	void	SetDestructorComment(const JCharacter* comment);

	JString	GetFunctionComment(const JCharacter* fnname = NULL, const JCharacter* access = NULL) const;
	void	SetFunctionComment(const JCharacter* comment);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GFGPrefsDialog*	itsDialog;

private:

	// not allowed

	GFGPrefsManager(const GFGPrefsManager& source);
	const GFGPrefsManager& operator=(const GFGPrefsManager& source);
};

#endif
