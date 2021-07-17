/******************************************************************************
 CBMDIServer.h

	Interface for the CBMDIServer class

	Copyright © 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBMDIServer
#define _H_CBMDIServer

#include <JXMDIServer.h>
#include <JPrefObject.h>

class CBMDIServer : public JXMDIServer, public JPrefObject
{
public:

	CBMDIServer();

	virtual ~CBMDIServer();

	static void	PrintCommandLineHelp();

	// called by CBEditGenPrefsDialog

	void	GetStartupOptions(bool* createEditor, bool* createProject,
							  bool* reopenLast, bool* chooseFile) const;
	void	SetStartupOptions(const bool createEditor, const bool createProject,
							  const bool reopenLast, const bool chooseFile);

protected:

	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

private:

	bool	itsCreateEditorFlag;
	bool	itsCreateProjectFlag;
	bool	itsReopenLastFlag;
	bool	itsChooseFileFlag;

private:

	void	DisplayManPage(JIndex* index, const JPtrArray<JString>& argList);
	void	AddFilesToSearch(JIndex* index, const JPtrArray<JString>& argList,
							 const bool clearFileList);
	void	DisplayFileDiffs(JIndex* index, const JPtrArray<JString>& argList,
							 const bool silent);
	void	DisplayVCSDiffs(const JUtf8Byte* type, JIndex* index,
							const JPtrArray<JString>& argList,
							const bool silent);

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
	bool* createEditor,
	bool* createProject,
	bool* reopenLast,
	bool* chooseFile
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
	const bool createEditor,
	const bool createProject,
	const bool reopenLast,
	const bool chooseFile
	)
{
	itsCreateEditorFlag  = createEditor;
	itsCreateProjectFlag = createProject;
	itsReopenLastFlag    = reopenLast;
	itsChooseFileFlag    = chooseFile;
}

#endif
