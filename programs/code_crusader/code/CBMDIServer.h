/******************************************************************************
 CBMDIServer.h

	Interface for the CBMDIServer class

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBMDIServer
#define _H_CBMDIServer

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMDIServer.h>
#include <JPrefObject.h>

class CBMDIServer : public JXMDIServer, public JPrefObject
{
public:

	CBMDIServer();

	virtual ~CBMDIServer();

	static void	PrintCommandLineHelp();

	// called by CBEditGenPrefsDialog

	void	GetStartupOptions(JBoolean* createEditor, JBoolean* createProject,
							  JBoolean* reopenLast, JBoolean* chooseFile) const;
	void	SetStartupOptions(const JBoolean createEditor, const JBoolean createProject,
							  const JBoolean reopenLast, const JBoolean chooseFile);

protected:

	virtual void	HandleMDIRequest(const JCharacter* dir,
									 const JPtrArray<JString>& argList);

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	JBoolean	itsCreateEditorFlag;
	JBoolean	itsCreateProjectFlag;
	JBoolean	itsReopenLastFlag;
	JBoolean	itsChooseFileFlag;

private:

	void	DisplayManPage(JIndex* index, const JPtrArray<JString>& argList);
	void	AddFilesToSearch(JIndex* index, const JPtrArray<JString>& argList,
							 const JBoolean clearFileList);
	void	DisplayFileDiffs(JIndex* index, const JPtrArray<JString>& argList,
							 const JBoolean silent);
	void	DisplayVCSDiffs(const JCharacter* type, JIndex* index,
							const JPtrArray<JString>& argList,
							const JBoolean silent);

	// not allowed

	CBMDIServer(const CBMDIServer& source);
	const CBMDIServer& operator=(const CBMDIServer& source);
};


/******************************************************************************
 Startup options

 ******************************************************************************/

inline void
CBMDIServer::GetStartupOptions
	(
	JBoolean* createEditor,
	JBoolean* createProject,
	JBoolean* reopenLast,
	JBoolean* chooseFile
	)
	const
{
	*createEditor  = itsCreateEditorFlag;
	*createProject = itsCreateProjectFlag;
	*reopenLast    = itsReopenLastFlag;
	*chooseFile    = itsChooseFileFlag;
}

inline void
CBMDIServer::SetStartupOptions
	(
	const JBoolean createEditor,
	const JBoolean createProject,
	const JBoolean reopenLast,
	const JBoolean chooseFile
	)
{
	itsCreateEditorFlag  = createEditor;
	itsCreateProjectFlag = createProject;
	itsReopenLastFlag    = reopenLast;
	itsChooseFileFlag    = chooseFile;
}

#endif
