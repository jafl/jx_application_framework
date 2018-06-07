/******************************************************************************
 JFSBinding.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JFSBinding
#define _H_JFSBinding

#include <JPtrArray-JString.h>

class JRegex;
class JIndexRange;

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

	JFSBinding(const JCharacter* pattern, const JCharacter* cmd,
			   const CommandType type, const JBoolean singleFile,
			   const JBoolean isSystem);
	JFSBinding(std::istream& input, const JFileVersion vers, const JBoolean isSystem,
			   JBoolean* isDefault, JBoolean* del);

	virtual	~JFSBinding();

	JBoolean		IsContentBinding() const;
	JBoolean		Match(const JString& fileName, const JString& content) const;

	const JString&	GetPattern() const;
	void			SetPattern(const JCharacter* pattern);

	const JString&	GetCommand(CommandType* type, JBoolean* singleFile) const;
	void			SetCommand(const JCharacter* cmd, const CommandType type,
							   const JBoolean singleFile);

	CommandType		GetCommandType() const;
	void			SetCommandType(const CommandType type);

	JBoolean		IsSingleFileCommand() const;
	void			SetSingleFileCommand(const JBoolean singleFile);

	JBoolean		IsSystemBinding() const;

	static void		ConvertCommand(JString* cmd);
	static JRegex*	CreateContentRegex();
	static JBoolean	WillBeRegex(const JString& pattern);

	static JFSBinding::CommandType	GetCommandType(const JBoolean shell,
												   const JBoolean window);

	static JListT::CompareResult
		ComparePatterns(JFSBinding* const & n1, JFSBinding* const & n2);

private:

	JString			itsPattern;			// can be empty
	JString			itsCmd;				// can be empty
	CommandType		itsCmdType;
	JBoolean		itsSingleFileFlag;
	const JBoolean	itsIsSystemFlag;

	JRegex*			itsNameRegex;		// can be nullptr
	JRegex*			itsContentRegex;	// can be nullptr
	JString			itsLiteralPrefix;	// optimization

private:

	void	UpdateRegex();
	void	CalcLiteralPrefix();

	// not allowed

	JFSBinding(const JFSBinding& source);
	const JFSBinding& operator=(const JFSBinding& source);
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
	const JCharacter* pattern
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
	JBoolean*		singleFile
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
	const JCharacter*	cmd,
	const CommandType	type,
	const JBoolean		singleFile
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

inline JBoolean
JFSBinding::IsSingleFileCommand()
	const
{
	return itsSingleFileFlag;
}

inline void
JFSBinding::SetSingleFileCommand
	(
	const JBoolean singleFile
	)
{
	itsSingleFileFlag = singleFile;
}

/******************************************************************************
 IsContentBinding

 ******************************************************************************/

inline JBoolean
JFSBinding::IsContentBinding()
	const
{
	return JI2B(itsContentRegex != nullptr);
}

/******************************************************************************
 IsSystemBinding

 ******************************************************************************/

inline JBoolean
JFSBinding::IsSystemBinding()
	const
{
	return itsIsSystemFlag;
}

#endif
