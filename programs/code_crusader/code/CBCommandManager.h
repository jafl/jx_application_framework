/******************************************************************************
 CBCommandManager.h

	Copyright © 2001-02 by John Lindal.

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

typedef JPtrArray<JPtrArray<JString> >						CBCmdQueue;
typedef JStack<const JUtf8Byte*, JArray<const JUtf8Byte*> >	CBFunctionStack;

class CBCommandManager : public JPrefObject, virtual public JBroadcaster
{
public:

	struct CmdInfo
	{
		JString*	path;
		JString*	cmd;
		JString*	name;

		bool	isMake;
		bool	isVCS;
		bool	saveAll;
		bool	oneAtATime;
		bool	useWindow;
		bool	raiseWindowWhenStart;
		bool	beepWhenFinished;

		JString*	menuText;
		JString*	menuShortcut;
		JString*	menuID;
		bool	separator;

		CmdInfo()
			:
			path(nullptr), cmd(nullptr), name(nullptr),
			isMake(false), isVCS(false), saveAll(false),
			oneAtATime(true), useWindow(true),
			raiseWindowWhenStart(false), beepWhenFinished(false),
			menuText(nullptr), menuShortcut(nullptr), menuID(nullptr), separator(false)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				JString* mt, JString* ms, JString* mi)
			:
			path(p), cmd(c), name(cn),
			isMake(false), isVCS(false), saveAll(false),
			oneAtATime(true), useWindow(true),
			raiseWindowWhenStart(false), beepWhenFinished(false),
			menuText(mt), menuShortcut(ms), menuID(mi), separator(false)
		{ };

		CmdInfo(JString* p, JString* c, JString* cn,
				const bool mk, const bool vcs,
				const bool sa, const bool oaat,
				const bool uw, const bool raise, const bool beep,
				JString* mt, JString* ms, JString* mi, const bool sep)
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

	bool	Prepare(const JString& cmdName, CBProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList,
						CBCommand** cmd, CmdInfo** returnInfo,
						CBFunctionStack* fnStack);

	bool	GetMakeDependCmdStr(CBProjectDocument* projDoc, const bool reportError,
									JString* cmdStr) const;
	bool	MakeDepend(CBProjectDocument* projDoc,
						   CBExecOutputDocument* compileDoc,
						   CBCommand** resultCmd);

	const JString&	GetMakeDependCommand() const;
	void			SetMakeDependCommand(const JString& cmd);

	bool	Substitute(CBProjectDocument* projDoc, const bool reportError,
						   JString* cmdStr) const;

	JSize		GetCommandCount() const;
	CmdList*	GetCommandList();

	void	AppendCommand(const JString& path, const JString& cmd,
						  const JString& name,
						  const bool isMake, const bool isVCS,
						  const bool saveAll,
						  const bool oneAtATime, const bool useWindow,
						  const bool raise, const bool beep,
						  const JString& menuText, const JString& menuShortcut,
						  const bool separator);

	void	UpdateAllCommandMenus();
	void	AppendMenuItems(JXTextMenu* menu, const bool hasProject) const;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	static bool	ReadCommands(std::istream& input,
								 JString* makeDependCmd, CmdList* cmdList,
								 JFileVersion* returnVers = nullptr);

	void	ConvertCompileDialog(std::istream& input, const JFileVersion vers,
								 CBBuildManager* buildMgr,
								 const bool readWindGeom);
	void	ConvertRunDialog(std::istream& input, const JFileVersion vers,
							 const bool readWindGeom);

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
	void	UpdateFileMarkers(const bool convertFromAncient, JString* s);

	static bool	Prepare(const CmdInfo& info, CBProjectDocument* projDoc,
							const JPtrArray<JString>& fullNameList,
							const JArray<JIndex>& lineIndexList, CBCommand** cmd,
							CBFunctionStack* fnStack);
	bool		FindCommandName(const JString& name, CmdInfo* info) const;
	static bool	Parse(const JString& cmd, CBCmdQueue* cmdQueue,
						  CBFunctionStack* fnStack);
	static bool	BuildCmdPath(JString* cmdPath, CBProjectDocument* projDoc,
								 const JString& fullName, const bool reportError);
	static bool	ProcessCmdQueue(const JString& cmdPath, const CBCmdQueue& cmdQueue,
									const CmdInfo& info, CBProjectDocument* projDoc,
									const JPtrArray<JString>& fullNameList,
									const JArray<JIndex>& lineIndexList,
									const bool reportError,
									CBCommand** cmd, CBFunctionStack* fnStack);
	static bool	Substitute(JString* arg, CBProjectDocument* projDoc,
							   const JString& fullName, const JIndex lineIndex,
							   const bool reportError);
	static bool	Add(const JString& path, const JPtrArray<JString>& cmdArgs,
						const CmdInfo& info, CBProjectDocument* projDoc,
						const JPtrArray<JString>& fullNameList,
						const JArray<JIndex>& lineIndexList,
						CBCommand** cmd, CBFunctionStack* fnStack);
	static bool	UsesFile(const JString& arg);

	JString	GetUniqueMenuID();

	bool	DocumentDeleted(JBroadcaster* sender);

	static void	UpgradeCommand(CmdInfo* info);

	// not allowed

	CBCommandManager(const CBCommandManager& source);
	const CBCommandManager& operator=(const CBCommandManager& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kUpdateCommandMenu;

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
