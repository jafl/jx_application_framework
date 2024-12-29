/******************************************************************************
 JXFSBindingManager.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXFSBindingManager
#define _H_JXFSBindingManager

#include <jx-af/jcore/JPtrArray-JString.h>
#include "JFSBinding.h"		// for CommandType
#include <jx-af/jcore/JError.h>

class JXIdleTask;
class JFSBindingList;
class JXFSEditBindingsDialog;
class JXFSRunCommandDialog;
class JXFSRunFileDialog;

class JXFSBindingManager : virtual public JBroadcaster
{
public:

	static void					Initialize();
	static JXFSBindingManager*	Instance();
	static void					Destroy();

	~JXFSBindingManager() override;

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

private:

	JFSBindingList*	itsBindingList;
	JXIdleTask*		itsUpdateBindingListTask;

	JXFSRunCommandDialog*	itsRunCmdDialog;
	JXFSRunFileDialog*		itsRunFileDialog;	// nullptr unless processing files
	JXFSEditBindingsDialog*	itsEditDialog;		// nullptr unless editing

	static JXFSBindingManager*	itsSelf;

private:

	void		ProcessFiles(JPtrArray<JFSBinding>* fileList,
							 const bool ignoreBindings);
	void		Exec(const JPtrArray<JFSBinding>& fileList,
					 const JIndex startIndex, const JIndex endIndex);
	static void	BuildCommand(JString* cmd, const JString& q, const JString& u,
							 const JString& qf, const JString& uf);
	void		SaveBinding(const JString& fileName, const JString& cmd,
							const JFSBinding::CommandType type,
							const bool singleFile);

	static std::weak_ordering
		ComparePatterns(JFSBinding* const & b1, JFSBinding* const & b2);
	static std::weak_ordering
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
