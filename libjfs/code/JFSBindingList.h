/******************************************************************************
 JFSBindingList.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JFSBindingList
#define _H_JFSBindingList

#include <JContainer.h>
#include <JPtrArray-JString.h>
#include "JFSBinding.h"			// need defn of CommandType
#include <JError.h>

class JFSBindingList : public JContainer
{
public:

	static JFSBindingList*	Create(const JCharacter** needUserCheck);

	virtual	~JFSBindingList();

	const JFSBinding*	GetBinding(const JIndex index) const;
	JBoolean			GetBinding(const JCharacter* fileName,
								   const JFSBinding** binding);

	JIndex		AddBinding(const JCharacter* pattern, const JCharacter* cmd,
						   const JFSBinding::CommandType type,
						   const JBoolean singleFile,
						   const JBoolean isSystem = kJFalse);
	JBoolean	DeleteBinding(const JIndex index);

	JBoolean	SetPattern(const JIndex index, const JCharacter* pattern,
						   JIndex* newIndex);
	JBoolean	SetCommand(const JIndex index, const JCharacter* cmd);
	JBoolean	SetCommandType(const JIndex index, const JFSBinding::CommandType type);
	JBoolean	SetSingleFile(const JIndex index, const JBoolean singleFile);
	void		ToggleSingleFile(const JIndex index);

	void		SetCommand(const JCharacter* pattern, const JCharacter* cmd,
						   const JFSBinding::CommandType type,
						   const JBoolean singleFile);

	JBoolean	WillUseDefaultCommand() const;
	void		ShouldUseDefaultCommand(const JBoolean use);

	const JFSBinding*	GetDefaultCommand() const;
	void				SetDefaultCommand(const JCharacter* cmd,
										  const JFSBinding::CommandType type,
										  const JBoolean singleFile);

	const JString&	GetShellCommand() const;
	void			SetShellCommand(const JCharacter* cmd);

	const JString&	GetWindowCommand() const;
	void			SetWindowCommand(const JCharacter* cmd);

	JBoolean	WillAutoUseShellCommand() const;
	void		ShouldAutoUseShellCommand(const JBoolean use);

	JError				Save();
	JBoolean			NeedsRevert() const;
	const JCharacter*	Revert();
	void				RevertIfModified();

	static void	CleanFileName(JString* s);

protected:

	JFSBindingList(const JCharacter* signalFileName, const JCharacter** needUserCheck);

private:

	JPtrArray<JFSBinding>*	itsBindingList;		// current bindings
	JPtrArray<JFSBinding>*	itsOverriddenList;	// overriden system bindings

	JBoolean	itsUseDefaultFlag;		// kJTrue => use in GetCommandForFile()
	JFSBinding*	itsUserDefault;			// NULL if none; hides itsSystemDefault
	JFSBinding*	itsSystemDefault;		// NULL if none

	JString		itsShellCmd;
	JString		itsWindowCmd;
	JBoolean	itsAutoShellFlag;

	const JString	itsSignalFileName;	// empty if only loaded system defaults
	time_t			itsSignalModTime;

private:

	JIndex	AddBinding(JFSBinding* pb);

	void	Load(std::istream& input, const JBoolean isSystem);
	void	StoreDefault(JFSBinding* b, JFSBinding** slot);

	// not allowed

	JFSBindingList(const JFSBindingList& source);
	const JFSBindingList& operator=(const JFSBindingList& source);
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

inline JBoolean
JFSBindingList::WillUseDefaultCommand()
	const
{
	return itsUseDefaultFlag;
}

inline void
JFSBindingList::ShouldUseDefaultCommand
	(
	const JBoolean use
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
	const JCharacter* cmd
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
	const JCharacter* cmd
	)
{
	itsWindowCmd = cmd;
}

/******************************************************************************
 Automatically use shell command when necessary

 ******************************************************************************/

inline JBoolean
JFSBindingList::WillAutoUseShellCommand()
	const
{
	return itsAutoShellFlag;
}

inline void
JFSBindingList::ShouldAutoUseShellCommand
	(
	const JBoolean use
	)
{
	itsAutoShellFlag = use;
}

#endif
