/******************************************************************************
 JFSBinding.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JFSBinding
#define _H_JFSBinding

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JRange.h>

class JRegex;

class JFSBinding
{
public:

	enum CommandType
	{
		kRunPlain = 0,
		kRunInShell,
		kRunInWindow	// preempts shell
	};

public:

	JFSBinding(const JString& pattern, const JString& cmd,
			   const CommandType type, const bool singleFile,
			   const bool isSystem);
	JFSBinding(std::istream& input, const JFileVersion vers, const bool isSystem,
			   bool* isDefault, bool* del);

	virtual	~JFSBinding();

	bool		IsContentBinding() const;
	bool		Match(const JString& fileName, const JString& content) const;

	const JString&	GetPattern() const;
	void			SetPattern(const JString& pattern);

	const JString&	GetCommand(CommandType* type, bool* singleFile) const;
	void			SetCommand(const JString& cmd, const CommandType type,
							   const bool singleFile);

	CommandType		GetCommandType() const;
	void			SetCommandType(const CommandType type);

	bool		IsSingleFileCommand() const;
	void			SetSingleFileCommand(const bool singleFile);

	bool		IsSystemBinding() const;

	static void		ConvertCommand(JString* cmd);
	static JRegex*	CreateContentRegex();
	static bool	WillBeRegex(const JString& pattern);

	static JFSBinding::CommandType	GetCommandType(const bool shell,
												   const bool window);

	static JListT::CompareResult
		ComparePatterns(JFSBinding* const & n1, JFSBinding* const & n2);

private:

	JString			itsPattern;			// can be empty
	JString			itsCmd;				// can be empty
	CommandType		itsCmdType;
	bool			itsSingleFileFlag;
	const bool		itsIsSystemFlag;

	JRegex*			itsNameRegex;		// can be nullptr
	JRegex*			itsContentRegex;	// can be nullptr
	JString			itsLiteralPrefix;	// optimization

private:

	void	UpdateRegex();
	void	CalcLiteralPrefix();

	// not allowed

	JFSBinding(const JFSBinding&) = delete;
	JFSBinding& operator=(const JFSBinding&) = delete;
};


// global functions

std::ostream& operator<<(std::ostream&, const JFSBinding&);

std::istream& operator>>(std::istream& input, JFSBinding::CommandType& type);
std::ostream& operator<<(std::ostream& output, const JFSBinding::CommandType type);


/******************************************************************************
 Pattern

 ******************************************************************************/

inline const JString&
JFSBinding::GetPattern()
	const
{
	return itsPattern;
}

inline void
JFSBinding::SetPattern
	(
	const JString& pattern
	)
{
	itsPattern = pattern;
	UpdateRegex();
}

/******************************************************************************
 Command

 ******************************************************************************/

inline const JString&
JFSBinding::GetCommand
	(
	CommandType*	type,
	bool*		singleFile
	)
	const
{
	*type       = itsCmdType;
	*singleFile = itsSingleFileFlag;
	return itsCmd;
}

inline void
JFSBinding::SetCommand
	(
	const JString&		cmd,
	const CommandType	type,
	const bool		singleFile
	)
{
	itsCmd            = cmd;
	itsCmdType        = type;
	itsSingleFileFlag = singleFile;
}

/******************************************************************************
 Command type

 ******************************************************************************/

inline JFSBinding::CommandType
JFSBinding::GetCommandType()
	const
{
	return itsCmdType;
}

inline void
JFSBinding::SetCommandType
	(
	const CommandType type
	)
{
	itsCmdType = type;
}

/******************************************************************************
 Single file

 ******************************************************************************/

inline bool
JFSBinding::IsSingleFileCommand()
	const
{
	return itsSingleFileFlag;
}

inline void
JFSBinding::SetSingleFileCommand
	(
	const bool singleFile
	)
{
	itsSingleFileFlag = singleFile;
}

/******************************************************************************
 IsContentBinding

 ******************************************************************************/

inline bool
JFSBinding::IsContentBinding()
	const
{
	return itsContentRegex != nullptr;
}

/******************************************************************************
 IsSystemBinding

 ******************************************************************************/

inline bool
JFSBinding::IsSystemBinding()
	const
{
	return itsIsSystemFlag;
}

#endif
