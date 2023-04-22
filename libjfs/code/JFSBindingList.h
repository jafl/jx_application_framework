/******************************************************************************
 JFSBindingList.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JFSBindingList
#define _H_JFSBindingList

#include <jx-af/jcore/JContainer.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include "JFSBinding.h"			// for CommandType
#include <jx-af/jcore/JError.h>

class JFSBindingList : public JContainer
{
public:

	static JFSBindingList*	Create(JString* needUserCheck);

	~JFSBindingList() override;

	const JFSBinding*	GetBinding(const JIndex index) const;
	bool				GetBinding(const JString& fileName,
								   const JFSBinding** binding) const;

	JIndex	AddBinding(const JString& pattern, const JString& cmd,
					   const JFSBinding::CommandType type,
					   const bool singleFile,
					   const bool isSystem = false);
	bool	DeleteBinding(const JIndex index);

	bool	SetPattern(const JIndex index, const JString& pattern,
						   JIndex* newIndex);
	bool	SetCommand(const JIndex index, const JString& cmd);
	bool	SetCommandType(const JIndex index, const JFSBinding::CommandType type);
	bool	SetSingleFile(const JIndex index, const bool singleFile);
	void	ToggleSingleFile(const JIndex index);

	void	SetCommand(const JString& pattern, const JString& cmd,
					   const JFSBinding::CommandType type,
					   const bool singleFile);

	bool	WillUseDefaultCommand() const;
	void	ShouldUseDefaultCommand(const bool use);

	const JFSBinding*	GetDefaultCommand() const;
	void				SetDefaultCommand(const JString& cmd,
										  const JFSBinding::CommandType type,
										  const bool singleFile);

	const JString&	GetShellCommand() const;
	void			SetShellCommand(const JString& cmd);

	const JString&	GetWindowCommand() const;
	void			SetWindowCommand(const JString& cmd);

	bool	WillAutoUseShellCommand() const;
	void	ShouldAutoUseShellCommand(const bool use);

	JError	Save();
	bool	NeedsRevert() const;
	JString	Revert();
	void	RevertIfModified();

	static void	CleanFileName(JString* s);

protected:

	JFSBindingList(const JString& signalFileName, JString* needUserCheck);

private:

	JPtrArray<JFSBinding>*	itsBindingList;		// current bindings
	JPtrArray<JFSBinding>*	itsOverriddenList;	// overriden system bindings

	bool		itsUseDefaultFlag;		// true => use in GetCommandForFile()
	JFSBinding*	itsUserDefault;			// nullptr if none; hides itsSystemDefault
	JFSBinding*	itsSystemDefault;		// nullptr if none

	JString	itsShellCmd;
	JString	itsWindowCmd;
	bool	itsAutoShellFlag;

	const JString	itsSignalFileName;	// empty if only loaded system defaults
	time_t			itsSignalModTime;

private:

	JIndex	AddBinding(JFSBinding* pb);

	void	Load(std::istream& input, const bool isSystem);
	void	StoreDefault(JFSBinding* b, JFSBinding** slot);

	// not allowed

	JFSBindingList(const JFSBindingList&) = delete;
	JFSBindingList& operator=(const JFSBindingList&) = delete;
};


/******************************************************************************
 GetBinding

	We can't provide a non-const version because we can't allow others to
	change the bindings behind our back.

 ******************************************************************************/

inline const JFSBinding*
JFSBindingList::GetBinding
	(
	const JIndex index
	)
	const
{
	return itsBindingList->GetElement(index);
}

/******************************************************************************
 ToggleSingleFile

 ******************************************************************************/

inline void
JFSBindingList::ToggleSingleFile
	(
	const JIndex index
	)
{
	SetSingleFile(index, !(GetBinding(index))->IsSingleFileCommand());
}

/******************************************************************************
 Use default command

 ******************************************************************************/

inline bool
JFSBindingList::WillUseDefaultCommand()
	const
{
	return itsUseDefaultFlag;
}

inline void
JFSBindingList::ShouldUseDefaultCommand
	(
	const bool use
	)
{
	itsUseDefaultFlag = use;
}

/******************************************************************************
 Shell command

 ******************************************************************************/

inline const JString&
JFSBindingList::GetShellCommand()
	const
{
	return itsShellCmd;
}

inline void
JFSBindingList::SetShellCommand
	(
	const JString& cmd
	)
{
	itsShellCmd = cmd;
}

/******************************************************************************
 Window command

 ******************************************************************************/

inline const JString&
JFSBindingList::GetWindowCommand()
	const
{
	return itsWindowCmd;
}

inline void
JFSBindingList::SetWindowCommand
	(
	const JString& cmd
	)
{
	itsWindowCmd = cmd;
}

/******************************************************************************
 Automatically use shell command when necessary

 ******************************************************************************/

inline bool
JFSBindingList::WillAutoUseShellCommand()
	const
{
	return itsAutoShellFlag;
}

inline void
JFSBindingList::ShouldAutoUseShellCommand
	(
	const bool use
	)
{
	itsAutoShellFlag = use;
}

#endif
