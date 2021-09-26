/******************************************************************************
 MDPrefsManager.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_MDPrefsManager
#define _H_MDPrefsManager

#include <jx-af/jx/JXPrefsManager.h>

class JPoint;
class JXWindow;
class JXPSPrinter;
class MDPrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	kMDProgramVersionID = 1,
	kMDVersionCheckID,
	kMDgCSFSetupID,
	kMDStatsDirectorID,
	kMDStatsToolBarID,
	kMDStatsDirectorWindSizeID,
	kMDRecordToolBarID,
	kMDRecordDirectorWindSizeID,
	kMDPrintSetupID,
	kMDOpenCmdID
};

class MDPrefsManager : public JXPrefsManager
{
public:

	MDPrefsManager(bool* isNew);

	virtual	~MDPrefsManager();

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

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	virtual void	SaveAllBeforeDestruct() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	MDPrefsDialog*	itsPrefsDialog;
};

#endif
