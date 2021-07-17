/******************************************************************************
 GFGPrefsManager.h

	Copyright (C) 2002 by Glenn W. Bach.

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

	GFGPrefsManager(bool* isNew);

	virtual	~GFGPrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JString	GetHeaderComment(const JString& classname = JString::empty) const;
	void	SetHeaderComment(const JString& comment);

	JString	GetAuthor() const;
	void	SetAuthor(const JString& author);

	JString	GetYear() const;
	void	SetYear(const JString& year);

	JString	GetCopyright(const bool replaceVars = false) const;
	void	SetCopyright(const JString& copyright);

	JString	GetSourceComment(const JString& classname = JString::empty, const JString& base = JString::empty) const;
	void	SetSourceComment(const JString& comment);

	JString	GetConstructorComment() const;
	void	SetConstructorComment(const JString& comment);

	JString	GetDestructorComment() const;
	void	SetDestructorComment(const JString& comment);

	JString	GetFunctionComment(const JString& fnname = JString::empty, const JString& access = JString::empty) const;
	void	SetFunctionComment(const JString& comment);

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	virtual void	SaveAllBeforeDestruct() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GFGPrefsDialog*	itsDialog;

private:

	// not allowed

	GFGPrefsManager(const GFGPrefsManager& source);
	const GFGPrefsManager& operator=(const GFGPrefsManager& source);
};

#endif
