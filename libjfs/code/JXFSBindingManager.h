/******************************************************************************
 JXFSBindingManager.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXFSBindingManager
#define _H_JXFSBindingManager

#include <jx-af/jcore/JPtrArray-JString.h>
#include "jx-af/jfs/JFSBinding.h"		// need defn of CommandType
#include <jx-af/jcore/JError.h>

class JXTimerTask;
class JFSBindingList;
class JXFSEditBindingsDialog;
class JXFSRunCommandDialog;
class JXFSRunFileDialog;
class JXFSRunScriptDialog;

class JXFSBindingManager : virtual public JBroadcaster
{
public:

	static void					Initialize();
	static JXFSBindingManager*	Instance();
	static void					Destroy();

	~JXFSBindingManager();

	static void		Exec(const JString& path);
	static void		Exec(const JString& fullProgramName,
						 const bool askForArgs);
	static void		Exec(const JPtrArray<JString>& fileList,
						 const bool ignoreBindings = false);
	static JError	Exec(const JString& path, const JString& cmd,
						 const JFSBinding::CommandType type);

	static void	EditBindings();

protected:

	JXFSBindingManager(JString* needUserCheck);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JFSBindingList*	itsBindingList;
	JXTimerTask*	itsUpdateBindingListTask;

	JXFSRunCommandDialog*	itsRunCmdDialog;

	JXFSRunFileDialog*		itsRunFileDialog;		// nullptr unless processing files
	JPtrArray<JFSBinding>*	itsFileList;			// nullptr unless waiting for itsRunFileDialog
	bool					itsIgnoreBindingsFlag;	// true => ask for every file
	JIndex					itsRunFileIndex;		// index in itsFileList of itsRunFileDialog

	JXFSRunScriptDialog*	itsRunScriptDialog;		// nullptr unless processing script
	JString					itsScriptPath;			// used when itsRunScriptDialog finishes

	JXFSEditBindingsDialog*	itsEditDialog;

	static JXFSBindingManager*	itsSelf;

private:

	bool	HasFiles() const;
	void		ProcessFiles();
	void		Exec(const JIndex startIndex, const JIndex endIndex);
	static void	BuildCommand(JString* cmd, const JString& q, const JString& u,
							 const JString& qf, const JString& uf);
	void		SaveBinding(const JString& fileName, const JString& cmd,
							const JFSBinding::CommandType type,
							const bool singleFile);

	static JListT::CompareResult
		ComparePatterns(JFSBinding* const & b1, JFSBinding* const & b2);
	static JListT::CompareResult
		CompareCommands(JFSBinding* const & b1, JFSBinding* const & b2);

	// not allowed

	JXFSBindingManager(const JXFSBindingManager&) = delete;
	JXFSBindingManager& operator=(const JXFSBindingManager&) = delete;
};


/******************************************************************************
 Initialize (static)

 ******************************************************************************/

inline void
JXFSBindingManager::Initialize()
{
	Instance();
}

#endif
