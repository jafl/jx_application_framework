/******************************************************************************
 GLPrefsMgr.h

	Interface for the GLPrefsMgr class

	Copyright (C) 1998 by Glenn W. Bach.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GLPrefsMgr
#define _H_GLPrefsMgr

#include <JXPrefsManager.h>
#include <JPtrArray.h>
#include <JString.h>

class JXWindow;
class JXPTPrinter;
class GLFitDirector;

enum
{
	kDelimiterPrefsID = 1,
	kFileModulePrefsID,
	kProgramVersionID,
	kPlotWindowSizePrefsID,
	kTableWindowSizePrefsID,
	kPTPrinterSetupID,
	kDataToolBarID,
	kFitToolBarID,
	kFitManagerID,
	kFitDirectorID,
	kChooseSaveFilePrefsID,
	kVersionCheckID
};

class GLPrefsMgr : public JXPrefsManager
{
public:

	GLPrefsMgr(bool* isNew);
	virtual ~GLPrefsMgr();

	void		EditPrefs();

	JString		GetGloveVersionStr() const;

	void		WritePrinterSetup(JXPTPrinter* printer);
	void		ReadPrinterSetup(JXPTPrinter* printer);

	void		WriteFitDirectorSetup(GLFitDirector* dir);
	void		ReadFitDirectorSetup(GLFitDirector* dir);

protected:

	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

private:

	void		GetWindowSize(const JPrefID id, JXWindow* window) const;
	void		SaveWindowSize(const JPrefID id, JXWindow* window);

	// not allowed

	GLPrefsMgr(const GLPrefsMgr& source);
	const GLPrefsMgr& operator=(const GLPrefsMgr& source);
};

#endif
