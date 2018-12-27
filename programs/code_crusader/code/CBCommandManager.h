/******************************************************************************
 CBCommandManager.h

	Copyright (C) 2001-02 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCommandManager
#define _H_CBCommandManager

#include <JPrefObject.h>
#include <JString.h>
#include <JStack.h>
#include <JPtrQueue.h>

class JProcess;
class JXTextMenu;
class CBCommand;
class CBProjectDocument;
class CBTextDocument;
class CBExecOutputDocument;
class CBCompileDocument;
class CBDocumentManager;
class CBBuildManager;

typedef JPtrArray<JPtrArray<JString> >							CBCmdQueue;
typedef JStack<const JCharacter*, JArray<const JCharacter*> >	CBFunctionStack;

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
			path(nullptr), cmd(nullptr), name(nullptr),
			isMake(kJFalse), isVCS(kJFalse), saveAll(kJFalse),
			oneAtATime(kJTrue), useWindow(kJTrue),
			raiseWindowWhenStart(kJFalse), beepWhenFinished(kJFalse),
			menuText(nullptr), menuShortcut(nullptr), menuID(nullptr), separator(kJFalse)
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

	CBCommandManager(CBDocumentManager* docMgr = nullptr);

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

	JBoolean	Prepare(const JString& cmdName, CBProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList,
						CBCommand** cmd, CmdInfo** returnInfo,
						CBFunctionStack* fnStack);

	JBoolean	GetMakeDependCmdStr(CBProjectDocument* projDoc, const JBoolean reportError,
									JString* cmdStr) const;
	JBoolean	MakeDepend(CBProjectDocument* projDoc,
						   CBExecOutputDocument* compileDoc,
						   CBCommand** resultCmd);

	const JString&	GetMakeDependCommand() const;
	void			SetMakeDependCommand(const JString& cmd);

	JBoolean	Substitute(CBProjectDocument* projDoc, const JBoolean reportError,
						   JString* cmdStr) const;

	JSize		GetCommandCount() const;
	CmdList*	GetCommandList();

	void	AppendCommand(const JString& path, const JString& cmd,
						  const JString& name,
						  const JBoolean isMake, const JBoolean isVCS,
						  const JBoolean saveAll,
						  const JBoolean oneAtATime, const JBoolean useWindow,
						  const JBoolean raise, const JBoolean beep,
						  const JString& menuText, const JString& menuShortcut,
						  const JBoolean separator);

	void	UpdateAllCommandMenus();
	void	AppendMenuItems(JXTextMenu* menu, const JBoolean hasProject) const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	static JBoolean	ReadCommands(std::istream& input,
								 JString* makeDependCmd, CmdList* cmdList,
								 JFileVersion* returnVers = nullptr);

	void	ConvertCompileDialog(std::istream& input, const JFileVersion vers,
								 CBBuildManager* buildMgr,
								 const JBoolean readWindGeom);
	void	ConvertRunDialog(std::istream& input, const JFileVersion vers,
							 const JBoolean readWindGeom);

	// stored in project template

	void	ReadTemplate(std::istream& input, const JFileVersion tmplVers,
						 const JFileVersion projVers);
	void	WriteTemplate(std::ostream& output) const;

	// used by CBCommand

	CBCompileDocument*				GetCompileDoc(CBProjectDocument* projDoc);
	static CBExecOutputDocument*	GetOutputDoc();

	// used by CBCommandSelection & CBCommandTable

	static JFileVersion	GetCurrentCmdInfoFileVersion();
	static CmdInfo		ReadCmdInfo(std::istream& input, const JFileVersion vers);
	static void			WriteCmdInfo(std::ostream& output, const CmdInfo& info);

	// used by CBEditCommandsDialog

	void	UpdateMenuIDs();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CmdList*			itsCmdList;
	JString				itsMakeDependCmd;
	CBCompileDocument*	itsCompileDoc;		// not owned

	static JPtrArray<CBTextDocument>	theExecDocList;

private:

	void	InitCommandList();
	void	UpdateFileMarkers(const JBoolean convertFromAncient, JString* s);

	static JBoolean	Prepare(const CmdInfo& info, CBProjectDocument* projDoc,
							const JPtrArray<JString>& fullNameList,
							const JArray<JIndex>& lineIndexList, CBCommand** cmd,
							CBFunctionStack* fnStack);
	JBoolean		FindCommandName(const JString& name, CmdInfo* info) const;
	static JBoolean	Parse(const JString& cmd, CBCmdQueue* cmdQueue,
						  CBFunctionStack* fnStack);
	static JBoolean	BuildCmdPath(JString* cmdPath, CBProjectDocument* projDoc,
								 const JString& fullName, const JBoolean reportError);
	static JBoolean	ProcessCmdQueue(const JString& cmdPath, const CBCmdQueue& cmdQueue,
									const CmdInfo& info, CBProjectDocument* projDoc,
									const JPtrArray<JString>& fullNameList,
									const JArray<JIndex>& lineIndexList,
									const JBoolean reportError,
									CBCommand** cmd, CBFunctionStack* fnStack);
	static JBoolean	Substitute(JString* arg, CBProjectDocument* projDoc,
							   const JString& fullName, const JIndex lineIndex,
							   const JBoolean reportError);
	static JBoolean	Add(const JString& path, const JPtrArray<JString>& cmdArgs,
						const CmdInfo& info, CBProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList,
						CBCommand** cmd, CBFunctionStack* fnStack);
	static JBoolean	UsesFile(const JString& arg);

	JString	GetUniqueMenuID();

	JBoolean	DocumentDeleted(JBroadcaster* sender);

	static void	UpgradeCommand(CmdInfo* info);

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
	const JString& cmd
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
