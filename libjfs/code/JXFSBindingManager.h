/******************************************************************************
 JXFSBindingManager.h

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXFSBindingManager
#define _H_JXFSBindingManager

#include <JPtrArray-JString.h>
#include "JFSBinding.h"		// need defn of CommandType
#include <JError.h>

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

	virtual	~JXFSBindingManager();

	static void		Exec(const JCharacter* path);
	static void		Exec(const JCharacter* fullProgramName,
						 const JBoolean askForArgs);
	static void		Exec(const JPtrArray<JString>& fileList,
						 const JBoolean ignoreBindings = kJFalse);
	static JError	Exec(const JCharacter* path, const JCharacter* cmd,
						 const JFSBinding::CommandType type);

	static void	EditBindings();

protected:

	JXFSBindingManager(const JCharacter** needUserCheck);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JFSBindingList*	itsBindingList;
	JXTimerTask*	itsUpdateBindingListTask;

	JXFSRunCommandDialog*	itsRunCmdDialog;

	JXFSRunFileDialog*		itsRunFileDialog;		// NULL unless processing files
	JPtrArray<JFSBinding>*	itsFileList;			// NULL unless waiting for itsRunFileDialog
	JBoolean				itsIgnoreBindingsFlag;	// kJTrue => ask for every file
	JIndex					itsRunFileIndex;		// index in itsFileList of itsRunFileDialog

	JXFSRunScriptDialog*	itsRunScriptDialog;		// NULL unless processing script
	JString					itsScriptPath;			// used when itsRunScriptDialog finishes

	JXFSEditBindingsDialog*	itsEditDialog;

	static JXFSBindingManager*	itsSelf;

private:

	JBoolean	HasFiles() const;
	void		ProcessFiles();
	void		Exec(const JIndex startIndex, const JIndex endIndex);
	static void	BuildCommand(JString* cmd, const JCharacter* q, const JCharacter* u,
							 const JCharacter* qf, const JCharacter* uf);
	void		SaveBinding(const JCharacter* fileName, const JCharacter* cmd,
							const JFSBinding::CommandType type,
							const JBoolean singleFile);

	static JListT::CompareResult
		ComparePatterns(JFSBinding* const & b1, JFSBinding* const & b2);
	static JListT::CompareResult
		CompareCommands(JFSBinding* const & b1, JFSBinding* const & b2);

	// not allowed

	JXFSBindingManager(const JXFSBindingManager& source);
	const JXFSBindingManager& operator=(const JXFSBindingManager& source);
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
