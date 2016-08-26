/******************************************************************************
 CBCommandManager.h

	Copyright © 2001-02 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCommandManager
#define _H_CBCommandManager

#include <JPrefObject.h>
#include <JBroadcaster.h>
#include <JString.h>

class JProcess;
class JXTextMenu;
class CBCommand;
class CBProjectDocument;
class CBTextDocument;
class CBExecOutputDocument;
class CBCompileDocument;
class CBDocumentManager;
class CBBuildManager;

class CBCommandManager : public JPrefObject, virtual public JBroadcaster
{
public:

	struct CmdInfo
	{
		JString*	path;
		JString*	cmd;
		JString*	name;

		JBoolean	isMake;
		JBoolean	isVCS;
		JBoolean	saveAll;
		JBoolean	oneAtATime;
		JBoolean	useWindow;
		JBoolean	raiseWindowWhenStart;
		JBoolean	beepWhenFinished;

		JString*	menuText;
		JString*	menuShortcut;
		JString*	menuID;
		JBoolean	separator;

		CmdInfo()
			:
			path(NULL), cmd(NULL), name(NULL),
			isMake(kJFalse), isVCS(kJFalse), saveAll(kJFalse),
			oneAtATime(kJTrue), useWindow(kJTrue),
			raiseWindowWhenStart(kJFalse), beepWhenFinished(kJFalse),
			menuText(NULL), menuShortcut(NULL), menuID(NULL), separator(kJFalse)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				JString* mt, JString* ms, JString* mi)
			:
			path(p), cmd(c), name(cn),
			isMake(kJFalse), isVCS(kJFalse), saveAll(kJFalse),
			oneAtATime(kJTrue), useWindow(kJTrue),
			raiseWindowWhenStart(kJFalse), beepWhenFinished(kJFalse),
			menuText(mt), menuShortcut(ms), menuID(mi), separator(kJFalse)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				const JBoolean mk, const JBoolean vcs,
				const JBoolean sa, const JBoolean oaat,
				const JBoolean uw, const JBoolean raise, const JBoolean beep,
				JString* mt, JString* ms, JString* mi, const JBoolean sep)
			:
			path(p), cmd(c), name(cn),
			isMake(mk), isVCS(vcs), saveAll(sa), oneAtATime(oaat), useWindow(uw),
			raiseWindowWhenStart(raise), beepWhenFinished(beep),
			menuText(mt), menuShortcut(ms), menuID(mi), separator(sep)
		{ };

		CmdInfo	Copy() const;
		void	Free();
	};

	class CmdList : public JArray<CmdInfo>
	{
	public:

		void	DeleteAll();
	};

public:

	CBCommandManager(CBDocumentManager* docMgr = NULL);

	virtual	~CBCommandManager();

	void	Exec(const JIndex cmdIndex, CBProjectDocument* projDoc,
				 CBTextDocument* textDoc);
	void	Exec(const JIndex cmdIndex, CBProjectDocument* projDoc,
				 const JPtrArray<JString>& fullNameList,
				 const JArray<JIndex>& lineIndexList);

	static void	Exec(const CmdInfo& info, CBProjectDocument* projDoc,
					 CBTextDocument* textDoc);
	static void	Exec(const CmdInfo& info, CBProjectDocument* projDoc,
					 const JPtrArray<JString>& fullNameList,
					 const JArray<JIndex>& lineIndexList);

	JBoolean	Prepare(const char* cmdName, CBProjectDocument* projDoc,
						CBTextDocument* textDoc, CBCommand** cmd, CmdInfo** returnInfo,
						JPtrArray<JString>* cmdList);
	JBoolean	Prepare(const char* cmdName, CBProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList,
						CBCommand** cmd, CmdInfo** returnInfo,
						JPtrArray<JString>* cmdList);

	JBoolean	GetMakeDependCmdStr(CBProjectDocument* projDoc, const JBoolean reportError,
									JString* cmdStr) const;
	JBoolean	MakeDepend(CBProjectDocument* projDoc,
						   CBExecOutputDocument* compileDoc,
						   CBCommand** resultCmd);

	const JString&	GetMakeDependCommand() const;
	void			SetMakeDependCommand(const JCharacter* cmd);

	JBoolean	Substitute(CBProjectDocument* projDoc, const JBoolean reportError,
						   JString* cmdStr) const;

	JSize		GetCommandCount() const;
	CmdList*	GetCommandList();

	void	AppendCommand(const JCharacter* path, const JCharacter* cmd,
						  const JCharacter* name,
						  const JBoolean isMake, const JBoolean isVCS,
						  const JBoolean saveAll,
						  const JBoolean oneAtATime, const JBoolean useWindow,
						  const JBoolean raise, const JBoolean beep,
						  const JCharacter* menuText, const JCharacter* menuShortcut,
						  const JBoolean separator);

	void	UpdateAllCommandMenus();
	void	AppendMenuItems(JXTextMenu* menu, const JBoolean hasProject) const;

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

	void	ConvertCompileDialog(istream& input, const JFileVersion vers,
								 CBBuildManager* buildMgr,
								 const JBoolean readWindGeom);
	void	ConvertRunDialog(istream& input, const JFileVersion vers,
							 const JBoolean readWindGeom);

	// stored in project template

	void	ReadTemplate(istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers);
	void	WriteTemplate(ostream& output) const;

	// used by CBCommand

	CBCompileDocument*				GetCompileDoc(CBProjectDocument* projDoc);
	static CBExecOutputDocument*	GetOutputDoc();

	// used by CBCommandSelection & CBCommandTable

	static JFileVersion	GetCurrentCmdInfoFileVersion();
	static CmdInfo		ReadCmdInfo(istream& input, const JFileVersion vers);
	static void			WriteCmdInfo(ostream& output, const CmdInfo& info);

	// used by CBEditCommandsDialog

	void	UpdateMenuIDs();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CmdList*			itsCmdList;
	JString				itsMakeDependCmd;
	CBCompileDocument*	itsCompileDoc;		// not owned

	static JPtrArray<CBTextDocument>	theExecDocList;

private:

	void	InitCommandList();
	void	UpdateFileMarkers(const JBoolean convertFromAncient, JString* s);

	static JBoolean	Prepare(const CmdInfo& info, CBProjectDocument* projDoc,
							CBTextDocument* textDoc, CBCommand** cmd,
							JPtrArray<JString>* cmdList);
	static JBoolean	Prepare(const CmdInfo& info, CBProjectDocument* projDoc,
							const JPtrArray<JString>& fullNameList,
							const JArray<JIndex>& lineIndexList, CBCommand** cmd,
							JPtrArray<JString>* cmdList);
	JBoolean		FindCommandName(const JCharacter* name, CmdInfo* info) const;
	static JBoolean	Substitute(JString* cmdPath, JString* cmd, CBProjectDocument* projDoc,
							   const JCharacter* fullName, const JIndex lineIndex,
							   const JBoolean onDisk,
							   const JBoolean reportError = kJTrue);
	static JBoolean	Add(const JCharacter* path, const JCharacter* cmdStr,
						const CmdInfo& info, CBProjectDocument* projDoc,
						CBTextDocument* textDoc,
						const JPtrArray<JString>* fullNameList,
						const JArray<JIndex>* lineIndexList,
						CBCommand** cmd, JPtrArray<JString>* cmdList);

	JString	GetUniqueMenuID();

	JBoolean	DocumentDeleted(JBroadcaster* sender);

	void	UpgradeCommand(CmdInfo* info);

	// not allowed

	CBCommandManager(const CBCommandManager& source);
	const CBCommandManager& operator=(const CBCommandManager& source);

public:

	// JBroadcaster messages

	static const JCharacter* kUpdateCommandMenu;

	class UpdateCommandMenu : public JBroadcaster::Message
		{
		public:

			UpdateCommandMenu()
				:
				JBroadcaster::Message(kUpdateCommandMenu)
				{ };
		};
};


/******************************************************************************
 Make depend

 ******************************************************************************/

inline const JString&
CBCommandManager::GetMakeDependCommand()
	const
{
	return itsMakeDependCmd;
}

inline void
CBCommandManager::SetMakeDependCommand
	(
	const JCharacter* cmd
	)
{
	itsMakeDependCmd = cmd;
}

/******************************************************************************
 GetCommandCount

 ******************************************************************************/

inline JSize
CBCommandManager::GetCommandCount()
	const
{
	return itsCmdList->GetElementCount();
}

/******************************************************************************
 GetCommandList

 ******************************************************************************/

inline CBCommandManager::CmdList*
CBCommandManager::GetCommandList()
{
	return itsCmdList;
}

/******************************************************************************
 UpdateAllCommandMenus

 ******************************************************************************/

inline void
CBCommandManager::UpdateAllCommandMenus()
{
	Broadcast(UpdateCommandMenu());
}

#endif
