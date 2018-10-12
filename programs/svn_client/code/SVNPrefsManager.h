/******************************************************************************
 SVNPrefsManager.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_SVNPrefsManager
#define _H_SVNPrefsManager

#include <JXPrefsManager.h>

class JPoint;
class JString;
class JXWindow;
class SVNPrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kSVNProgramVersionID = 1,
	kSVNgCSFSetupID,
	kSVNMainToolBarID,
	kSVNDocMgrStateID,
	kSVNMainDirectorWindSizeID,
	kSVNIntegrationID,
	kSVNVersionCheckID,
	kSVNExpireTimeStampID,
	kSVNGetRepoDialogID
};

class SVNPrefsManager : public JXPrefsManager
{
public:

	// do not change ID's once they are assigned, because they are stored in files

	enum Integration
	{
		kCodeCrusader,
		kCmdLine,
		kCustom
	};

	enum Command
	{
		kCommitEditor,
		kDiffCmd,
		kReloadChangedCmd
	};

public:

	SVNPrefsManager(JBoolean* isNew);

	virtual	~SVNPrefsManager();

	JBoolean	GetExpirationTimeStamp(time_t* t) const;
	void		SetExpirationTimeStamp(const time_t t);

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	JBoolean	RestoreProgramState();
	void		SaveProgramState();
	void		ForgetProgramState();

	JBoolean	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
							  JCoordinate* width, JCoordinate* height) const;
	void		SaveWindowSize(const JPrefID& id, JXWindow* window);

	JBoolean	GetCommand(const Command cmdType, Integration* type, JString* cmd) const;

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion) override;
	virtual void	SaveAllBeforeDestruct() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SVNPrefsDialog*	itsPrefsDialog;

private:

	JBoolean	GetIntegration(Integration* type, JString* commitEditor,
							   JString* diffCmd, JString* reloadChangedCmd) const;
	void		SetIntegration(const Integration type, const JString& commitEditor,
							   const JString& diffCmd, const JString& reloadChangedCmd);

	// not allowed

	SVNPrefsManager(const SVNPrefsManager& source);
	const SVNPrefsManager& operator=(const SVNPrefsManager& source);
};

std::istream&	operator>>(std::istream& input, SVNPrefsManager::Integration& type);
std::ostream&	operator<<(std::ostream& output, const SVNPrefsManager::Integration type);

#endif
