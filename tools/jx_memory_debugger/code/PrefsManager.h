/******************************************************************************
 PrefsManager.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_PrefsManager
#define _H_PrefsManager

#include <jx-af/jx/JXPrefsManager.h>

class JPoint;
class JXWindow;
class JXPSPrinter;
class PrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kProgramVersionID = 1,
	kVersionCheckID,
	kgCSFSetupID,
	kStatsDirectorID,
	kStatsToolBarID,
	kStatsDirectorWindSizeID,
	kRecordToolBarID,
	kRecordDirectorWindSizeID,
	kPrintSetupID,
	kOpenCmdID
};

class PrefsManager : public JXPrefsManager
{
public:

	PrefsManager(bool* isNew);

	~PrefsManager() override;

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	bool	GetWindowSize(const JPrefID& id, JPoint* desktopLoc,
						  JCoordinate* width, JCoordinate* height) const;
	void	SaveWindowSize(const JPrefID& id, JXWindow* window);

	bool	LoadPrintSetup(JXPSPrinter* p) const;
	void	SavePrintSetup(const JXPSPrinter& p);

	JString	GetOpenFileCommand() const;
	void	SetOpenFileCommand(const JString& cmd);

protected:

	void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	void	SaveAllBeforeDestruct() override;
};

#endif
